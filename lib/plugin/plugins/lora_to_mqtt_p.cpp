
#include "lora_to_mqtt_p.h"


#define LORA_PLUGIN_TASK_MEMORY   2048   
#define LORA_PLUGIN_TASK_PRIORITY 2

/*
struct lora_on_message_parameter
{
    lora_to_mqtt *self;
    String message;
};

void lora_to_mqtt_message_task(void *pvParameters)
{
    lora_on_message_parameter *parameter = reinterpret_cast<lora_on_message_parameter *>(pvParameters);
    OZ_LOGV("LoRa", "Arrive Message %s",parameter->message.c_str());
    parameter->self->execute_lora(parameter->message);
    vTaskDelete(NULL);
}
*/
void lora_to_mqtt_loop_task(void *pvParameters)
{
    OZ_LOGI("LoRa", "Loop Task is started");
    lora_to_mqtt *self = reinterpret_cast<lora_to_mqtt *>(pvParameters);
    while (true)
    {
        String message = self->receive();
        if (message.length() > 0)
        {
            self->execute_lora(message);

            /*

            lora_on_message_parameter *parameter = new lora_on_message_parameter();
            parameter->self = self;
            parameter->message = meggase;

            xTaskCreate(
                lora_to_mqtt_message_task,
                "lora_to_mqtt_message_task",
                CORE_PLUGIN_TASK_MEMORY,
                parameter,
                CORE_PLUGIN_TASK_PRIORITY,
                NULL);
            */
        }
        vTaskDelay(pdMS_TO_TICKS(LORA_TASK_LOOP_DELAY));
    }
    vTaskDelete(NULL);
}

// 0    1     2  3  4        5        6     7
//#/[NAME]/AUX/M0/M1/[UART_NUMBER]/[Tx]?/[Rx]?
lora_to_mqtt::lora_to_mqtt(params_t init) : plugin_base(init)
{

    this->_aux = this->get_int_parameter(LORATOMQTT_STR_AUX);
    if (!GPIO_IS_VALID_GPIO(this->_aux))
    {
        OZ_LOGW(this->name().c_str(), "Aux Pin is not valid GPIO");
        return;
    }

    this->_m0 = this->get_int_parameter(LORATOMQTT_STR_M0);
    if (!GPIO_IS_VALID_GPIO(this->_m0))
    {
        OZ_LOGW(this->name().c_str(), "M0 Pin is not valid GPIO");
        return;
    }

    this->_m1 = this->get_int_parameter(LORATOMQTT_STR_M1);
    if (!GPIO_IS_VALID_GPIO(this->_m1))
    {
        OZ_LOGW(this->name().c_str(), "M1 Pin is not valid GPIO");
        return;
    }

    this->_uart_number = this->get_int_parameter(LORATOMQTT_STR_UART);

    if (this->_uart_number >= UART_NUM_MAX)
    {
        OZ_LOGW(this->name().c_str(), "Uart mnumber not valid");
        return;
    }

    this->_serial = new HardwareSerial(this->_uart_number);

    uint8_t tx = this->get_int_parameter(LORATOMQTT_STR_TX);
    uint8_t rx = this->get_int_parameter(LORATOMQTT_STR_RX);

    if (GPIO_IS_VALID_GPIO(tx) && GPIO_IS_VALID_GPIO(rx))
    {
        this->_lora = new LoRa_E32(tx, rx, this->_serial, this->_aux, this->_m0, this->_m1, UART_BPS_RATE_9600);
    }
    else
    {
        this->_lora = new LoRa_E32(this->_serial, this->_aux, this->_m0, this->_m1, UART_BPS_RATE_9600);
    }

    this->_protocoll = new e32cp();

    e32cp_config_t config;
    config.address = 0;
    config.channel = 3;
    config.bootloader_random = false;
    config.key_length = LORA_KEY_LENGTH;
    config.lora = this->_lora;
    config.pre_shared_key = (uint8_t*)LORA_PRESHARED_KEY;

    if (!this->_protocoll->begin(config))
    {
        OZ_LOGE(this->name().c_str(), "Protocol begin error!! ");
        return;
    }

    if (!this->_protocoll->configure())
    {
        OZ_LOGE(this->name().c_str(), "Protocol configuration error!! ");
        return;
    }

    this->_root_topic = buildTopic(LORA_TOPIC_SEND_STRING) + char(MQTT_TOPIC_SEPARATOR_CHAR) + char(MQTT_TOPIC_CHAR_ANY);
    this->subscribe_topic(this->_root_topic.c_str());


    xTaskCreate(
        lora_to_mqtt_loop_task,
        "lora_to_mqtt_loop_task",
        LORA_PLUGIN_TASK_MEMORY,
        this,
        LORA_PLUGIN_TASK_PRIORITY,
        NULL);

    OZ_LOGI(this->name().c_str(), "Configuration complete");
    OZ_LOGI(this->name().c_str(), " AUX:[%u] M0:[%u] M1:[%u] UART:[%u]",
            this->_aux,
            this->_m0,
            this->_m1,
            this->_uart_number);

    this->_espname = SETTING(DB_SETTING_ESPNAME);

    this->_initialized = true;
}

lora_to_mqtt::~lora_to_mqtt()
{
    delete this->_lora;
    delete this->_protocoll;
    delete this->_serial;
}

void lora_to_mqtt::_send_response(String Topic, String Payload)
{
    std::vector<String> topic_array = oz_utils::splitString(Topic.c_str(), MQTT_TOPIC_SEPARATOR_CHAR);

    if (topic_array.size() < 5)
        return;

    if (!topic_array[0].equals(this->_espname))
        return;

    if (!topic_array[1].equals(this->name()))
        return;

    if (!topic_array[2].equals(LORA_TOPIC_SEND_STRING))
        return;

    uint8_t client_address = atoi(topic_array[3].c_str());

    String loramessage = Payload + char(LORA_MESSAGE_SEPARATOR_CHAR);

    for (uint8_t i = 4; i < topic_array.size(); i++)
    {
        loramessage += topic_array[i];

        if (i != topic_array.size() - 1)
        {
            loramessage += char(LORA_TOPIC_SEPARATOR_CHAR);
        }
    }

    bool result = false;
    uint8_t count = 3;

    do
    {
        result = this->_protocoll->wake_up_asleep(loramessage, client_address);

        if(!result)
        {
            count --;
            delay(2000);
        }

    }while (!result && count > 0);
    


    if(result)
    {
        OZ_LOGD(this->name().c_str(), "Wake command ok: [%s] to client : [%u] ", loramessage.c_str(), client_address);
    }
    else
    {
        OZ_LOGE(this->name().c_str(), "Problem occured on sending LoRa");
        OZ_LOGE(this->name().c_str(), "Address : %u , Channel: %u", client_address, E32_SERVER_CHANNEL);
    }
}

void lora_to_mqtt::execute_lora(String lora_message)
{
    if (lora_message.length() == 0)
    {
        OZ_LOGW(this->name().c_str(), "LoRa Recieve empty message!!");
        return;
    }

    std::vector<String> message_array = oz_utils::splitString(lora_message.c_str(), LORA_MESSAGE_SEPARATOR_CHAR);

    if (message_array.size() != 2)
    {
        OZ_LOGW(this->name().c_str(), "LoRa Recieve Wrong message format [%s]",lora_message );
        return;
    }

    OZ_LOGV(this->name().c_str(), "LoRa Recieve [%s]->[%s]", message_array[1].c_str(), message_array[0].c_str());
    OZMQTT.send(message_array[1], message_array[0]);
}

String lora_to_mqtt::receive()
{
    return this->_protocoll->recive();
}