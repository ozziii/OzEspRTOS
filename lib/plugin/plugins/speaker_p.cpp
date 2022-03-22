#include "speaker_p.h"

#include "wifi_oz.h"

static void udp_stop_data_recive_isr(TimerHandle_t xTimer)
{
    speaker *plugin = (speaker *)pvTimerGetTimerID(xTimer);
    plugin->wdt_count++;
}

uint16_t map_volume(uint16_t volume)
{
    return map(volume, 0, 100, 120, 230);
}

// 0    1      2    3     4      5     6      7      9     10
//#/[NAME]/[BCK]/[RLC]/[DIN]/[PVDD]/[PDN]/[RESET]/[SCL]?/[SDA]?
speaker::speaker(params_t init) : plugin_base(init)
{

    uint8_t bck_pin = this->get_int_parameter(SPEAKER_STR_BCK);
    uint8_t rlc_pin = this->get_int_parameter(SPEAKER_STR_RLC);
    uint8_t din_pin = this->get_int_parameter(SPEAKER_STR_DIN);
    uint8_t pvdd_pin = this->get_int_parameter(SPEAKER_STR_PVDD);
    uint8_t pdn_pin = this->get_int_parameter(SPEAKER_STR_PDN);
    uint8_t reset_pin = this->get_int_parameter(SPEAKER_STR_RST);

    int8_t scl_pin = this->get_int_parameter(SPEAKER_STR_SCL);
    int8_t sda_pin = this->get_int_parameter(SPEAKER_STR_SDA);

    if (!GPIO_IS_VALID_GPIO(scl_pin))
        scl_pin = -1;
    if (!GPIO_IS_VALID_GPIO(sda_pin))
        sda_pin = -1;

    this->_speaker_dac = new TAS5731(
        reset_pin,
        pdn_pin,
        pvdd_pin,
        true,
        sda_pin,
        scl_pin);

    this->_topic_volume = this->buildTopic(MQTT_COMMAND_VOLUME);
    this->_topic_volume_state = this->buildTopic(MQTT_COMMAND_VOLUME_STATE);
    this->_topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
    this->_topic_state = this->buildTopic(MQTT_COMMAND_STATE);

    this->subscribe_topic(this->_topic_volume);
    this->subscribe_topic(this->_topic_action);
    this->subscribe_topic(MULTICAST_PLUGIN_SERVER_STATE_TOPIC);

    this->_stop_task_handle = xTimerCreate(
        "WDT_SPEAKER_TIKER",
        1,
        pdFALSE,
        this,
        udp_stop_data_recive_isr);

    this->volume = SPEAKER_PLUGIN_DEFAULT_GAIN;

    this->_mr.begin(MULTIROOM_SPEAKER, SPEAKER_PLUGIN_MULTICAST_PORT, I2S_NUM_0, bck_pin, rlc_pin, din_pin);

    this->_state = SPEAKER_STATE_STOP;

    this->_initialized = true;

    OZ_LOGI(this->name().c_str(), "Init ok");
};

void speaker::_force_update()
{
    if (this->_state == SPEAKER_STATE_STOP)
    {
        OZMQTT.send(MULTICAST_PLUGIN_CLIENT_REQEST_TOPIC, "");
    }
}

void speaker::_send_response(String Topic, String Message)
{
    if (Topic.equals(this->_topic_volume))
    {
        this->volume = atoi(Message.c_str());
        this->_speaker_dac->setVolume(map_volume(volume));
        this->_public_state();
    }
    else if (Topic.equals(this->_topic_action))
    {
        if (Message.equals(MQTT_STATE_ON) && this->_state == SPEAKER_STATE_OFF)
        {
            this->_state = SPEAKER_STATE_STOP;
            OZMQTT.send(MULTICAST_PLUGIN_CLIENT_REQEST_TOPIC, "");
            this->_public_state();
        }
        else if (Message.equals(MQTT_STATE_OFF))
        {
            switch (this->_state)
            {
            case SPEAKER_STATE_STOP:
                this->stop_task();
                this->_state = SPEAKER_STATE_OFF;
                OZMQTT.send(MULTICAST_PLUGIN_CLIENT_STOP_TOPIC, WIFIOZ.getAddress().c_str());
                this->_public_state();
                break;
            case SPEAKER_STATE_PLAY:
                this->stop_task();
                this->_state = SPEAKER_STATE_OFF;
                OZMQTT.send(MULTICAST_PLUGIN_CLIENT_STOP_TOPIC, WIFIOZ.getAddress().c_str());
                this->_public_state();
                break;
            default:
                break;
            }
        }
    }
    else if (Topic.equals(MULTICAST_PLUGIN_SERVER_STATE_TOPIC))
    {
        if (Message.equals(MULTICAST_PLUGIN_PLAY_MESSAGE))
        {
            switch (this->_state)
            {
            case SPEAKER_STATE_STOP:
                this->start_task();
                OZMQTT.send(MULTICAST_PLUGIN_CLIENT_READY_TOPIC, WIFIOZ.getAddress().c_str());
                this->_state = SPEAKER_STATE_PLAY;
                this->_public_state();
                break;
            case SPEAKER_STATE_PLAY:
                this->start_task();
                OZMQTT.send(MULTICAST_PLUGIN_CLIENT_READY_TOPIC, WIFIOZ.getAddress().c_str());
                break;
            default:
                break;
            }
        }
        else if (Message.equals(MULTICAST_PLUGIN_STOP_MESSAGE) && this->_state == SPEAKER_STATE_PLAY)
        {
            this->_state = SPEAKER_STATE_STOP;
            this->stop_task();
            this->_public_state();
        }
    }
}

void speaker::_execute_sensor()
{
    if (this->wdt_count > MAX_WDT_SPEAKER_COUNT)
    {
        this->_state = SPEAKER_STATE_STOP;
        this->stop_task();
        this->wdt_count = 0;
        this->_public_state();
    }
}

void speaker::_public_state()
{
    char *char_value = new char[5];
    ultoa(this->volume, char_value, 10);
    OZMQTT.send(this->_topic_volume_state, char_value);
    delete[] char_value;
    switch (this->_state)
    {
    case SPEAKER_STATE_STOP:
        OZMQTT.send(this->_topic_state, MULTICAST_PLUGIN_STOP_MESSAGE);
        break;
    case SPEAKER_STATE_PLAY:
        OZMQTT.send(this->_topic_state, MULTICAST_PLUGIN_PLAY_MESSAGE);
        break;
    case SPEAKER_STATE_OFF:
        OZMQTT.send(this->_topic_state, MQTT_STATE_OFF);
        break;
    }
}

void speaker::start_task()
{
    if (this->_mr.start(NULL, this->_stop_task_handle))
    {
        this->wdt_count = 0;
        this->_start_tas5731();
    }
}

void speaker::stop_task()
{
    if (this->_mr.stop())
    {
        this->_stop_tas5731();
    }
}

void speaker::_start_tas5731()
{
    this->_speaker_dac->Initialization();
    this->_speaker_dac->set_registers(Default, Size(Default));
    this->_speaker_dac->set_bit_per_sample(16);
    this->_speaker_dac->Exit_Shutdown();
    delay(500);
    this->_speaker_dac->setVolume(map_volume(volume));
}

void speaker::_stop_tas5731()
{
    this->_speaker_dac->Enter_Shutdown();
    this->_speaker_dac->Power_Down();
}

unsigned long speaker::sensor_delay()
{
    return 10000;
}