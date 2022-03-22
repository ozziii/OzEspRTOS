#include "audio_server_p.h"

#include "WiFi.h"

// 0    1    2     3     4     5          6
//#/[NAME]/[EN]/[BCK]/[RLC]/[Data]/[IP1-IP2-IP3....]
audio_server::audio_server(params_t init) : plugin_base(init)
{

    this->_enable_pin = this->get_int_parameter(AUDIO_SERVER_STR_EN);
    if (!GPIO_IS_VALID_GPIO(this->_enable_pin))
    {
        OZ_LOGW(this->name().c_str(), "Enable pin is not valid GPIO");
        return;
    }

    int16_t bck_pin = this->get_int_parameter(AUDIO_SERVER_STR_BCK);
    if (!GPIO_IS_VALID_GPIO(bck_pin))
    {
        OZ_LOGW(this->name().c_str(), "BCK pin is not valid GPIO");
        return;
    }

    int16_t rlc_pin = this->get_int_parameter(AUDIO_SERVER_STR_RLC);
    if (!GPIO_IS_VALID_GPIO(rlc_pin))
    {
        OZ_LOGW(this->name().c_str(), "RLC pin is not valid GPIO");
        return;
    }

    int16_t din_pin = this->get_int_parameter(AUDIO_SERVER_STR_DATA);
    if (!GPIO_IS_VALID_GPIO(din_pin))
    {
        OZ_LOGW(this->name().c_str(), "DIN pin is not valid GPIO");
        return;
    }

    pinMode(this->_enable_pin, INPUT);
    this->attach_interrupt(this->_enable_pin, CHANGE);

    this->_mr.begin(MULTIROOM_SERVER, SPEAKER_PLUGIN_MULTICAST_PORT, I2S_NUM_0, bck_pin, rlc_pin, din_pin);

    this->subscribe_topic(MULTICAST_PLUGIN_CLIENT_READY_TOPIC);
    this->subscribe_topic(MULTICAST_PLUGIN_CLIENT_STOP_TOPIC);
    this->subscribe_topic(MULTICAST_PLUGIN_CLIENT_REQEST_TOPIC);

    this->_initialized = true;
    OZ_LOGI(this->name().c_str(), "Initialized ok! Pin: %u  ", this->_enable_pin);
}

void audio_server::_interrupt(uint8_t pin)
{
    if (!OZMQTT.is_connected())
        return;

    delay(200);
    if (digitalRead(this->_enable_pin))
    {
        this->_start_server();
    }
    else
    {
        this->_stop_server();
    }
}

void audio_server::_force_update()
{
    // ON CONNECTION Check if blue
    if (!digitalRead(this->_enable_pin))
        return;

    this->_start_server();
}

void audio_server::_send_response(String Topic, String Message)
{
    if (!digitalRead(this->_enable_pin))
        return;

    if (Topic.equals(MULTICAST_PLUGIN_CLIENT_READY_TOPIC))
    {
        IPAddress new_ip;
        if (new_ip.fromString(Message))
        {
            OZ_LOGV(this->name().c_str(), "Add client Ip: %s", Message.c_str());
            this->_mr.add_client(new_ip);
        }
        else
        {
            OZ_LOGW(this->name().c_str(), "Add client error %s", Message.c_str());
        }
    }
    else if (Topic.equals(MULTICAST_PLUGIN_CLIENT_STOP_TOPIC))
    {
        IPAddress new_ip;
        if (new_ip.fromString(Message))
        {
            OZ_LOGV(this->name().c_str(), "Remove client Ip: %s", Message.c_str());
            this->_mr.remove_client(new_ip);
        }
        else
        {
            OZ_LOGW(this->name().c_str(), "Remove client error %s", Message.c_str());
        }
    }
    else if (Topic.equals(MULTICAST_PLUGIN_CLIENT_REQEST_TOPIC))
    {
        OZMQTT.send(MULTICAST_PLUGIN_SERVER_STATE_TOPIC, MULTICAST_PLUGIN_PLAY_MESSAGE);
    }
}

bool audio_server::_start_server()
{
    if (this->_mr.start())
    {
        OZMQTT.send(MULTICAST_PLUGIN_SERVER_STATE_TOPIC, MULTICAST_PLUGIN_PLAY_MESSAGE);
        OZ_LOGV(this->name().c_str(), "Start Send thread RAM %u", ESP.getFreeHeap());
    }
    else
    {
        OZMQTT.send(MULTICAST_PLUGIN_SERVER_STATE_TOPIC, MULTICAST_PLUGIN_STOP_MESSAGE);
        OZ_LOGE(this->name().c_str(), "Error Start thread");
        esp_restart();
    }

    return true;
}

bool audio_server::_stop_server()
{
    if (this->_mr.stop())
    {
        OZMQTT.send(MULTICAST_PLUGIN_SERVER_STATE_TOPIC, MULTICAST_PLUGIN_STOP_MESSAGE);
        OZ_LOGV(this->name().c_str(), "Stop Send thread RAM %u", ESP.getFreeHeap());
    }
    else
    {
        OZMQTT.send(MULTICAST_PLUGIN_SERVER_STATE_TOPIC, MULTICAST_PLUGIN_STOP_MESSAGE);
        OZ_LOGE(this->name().c_str(), "Error Stop thread");
        esp_restart();
    }

    return true;
}