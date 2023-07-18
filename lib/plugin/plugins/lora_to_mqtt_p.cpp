
#include "lora_to_mqtt_p.h"

#define LORA_PLUGIN_TASK_MEMORY 2048
#define LORA_PLUGIN_TASK_PRIORITY 2

void lora_to_mqtt_loop_task(void *pvParameters)
{
    OZ_LOGI("LoRa", "Loop Task is started");
    lora_to_mqtt *self = reinterpret_cast<lora_to_mqtt *>(pvParameters);
    while (true)
    {
        e32_receve_struct_t message;
        if (xQueueReceive(self->get_queue(), &message, portMAX_DELAY))
        {
            self->execute_lora(message);
        }
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

    if (this->_uart_number >= 3)
    {
        OZ_LOGW(this->name().c_str(), "Uart mnumber not valid");
        return;
    }

    uint8_t tx = this->get_int_parameter(LORATOMQTT_STR_TX);
    uint8_t rx = this->get_int_parameter(LORATOMQTT_STR_RX);

    if (!GPIO_IS_VALID_GPIO(tx) || !GPIO_IS_VALID_GPIO(rx))
    {
        OZ_LOGW(this->name().c_str(), "No Tx and Rx pin valid");
        return;
    }

    e32_pin_t pin;
    pin.AUX_REVERSE = false;
    pin.M0 = (gpio_num_t)this->_m0;
    pin.M1 = (gpio_num_t)this->_m1;
    pin.AUX = (gpio_num_t)this->_aux;
    pin.Tx = (gpio_num_t)tx;
    pin.Rx = (gpio_num_t)rx;

    OZ_LOGI(this->name().c_str(), "Set Pin Tx:%u Rx:%u", tx, rx);

    auto err = this->_lora.begin(this->_uart_number, pin);

    if (err != E32_ERR_SUCCESS)
    {
        OZ_LOGW(this->name().c_str(), "E32 begin error");
        return;
    }

    e32_configuration_t e32_config = this->_lora.get_config();

    if (e32_config.error != E32_ERR_SUCCESS)
    {
        OZ_LOGW(this->name().c_str(), "Error get config [%d]", e32_config.error);
        return;
    }

    OZ_LOGI(this->name().c_str(), "PARITY[%X] BAUD[%X] AIR[%X] FIXED[%X] IO[%X] WAKE[%X] FEC[%X] PW[%X] ADDL[%X] ADDH[%X] CH[%X]",
             e32_config.uart_parity,
             e32_config.uart_baud_rate,
             e32_config.air_data_rate,
             e32_config.fixed_mode,
             e32_config.io_driver_mode,
             e32_config.wake_up_time,
             e32_config.fec_enabled,
             e32_config.transmission_power,
             e32_config.address_L,
             e32_config.address_H,
             e32_config.channel);
    /*
        bool must_config = false; // Update config parameter

        if (must_config)
        {
            OZ_LOGV(this->name().c_str(),"Update E32 Configuration");
            e32_config.address_H = 0;
            e32_config.address_L = 0;
            e32_config.channel = 3;
            e32_config.air_data_rate = E32_PARAMETER_AIRATE_24;
            e32_config.fec_enabled = E32_OPTION_FEC_ON;
            e32_config.fixed_mode = E32_OPTION_MODE_FIXED;
            e32_config.io_driver_mode = E32_OPTION_IOMODE_PUSHPULL;
            e32_config.transmission_power = E32_OPTION_TXPOWER_4;
            e32_config.uart_baud_rate = E32_PARAMETER_BAUD_9600;
            e32_config.uart_parity = E32_PARAMETER_8N1;
            e32_config.wake_up_time = E32_OPTION_WAKEUP_1750;

            e32_errno_t err = this->_lora.set_permanent_config(e32_config);

            if (err != E32_ERR_SUCCESS)
            {
                OZ_LOGW(this->name().c_str(), "Error set config [%u]", err);
                return;
            }
            else
            {
                OZ_LOGV(this->name().c_str(), "Update success!");
            }
        }
        */

    e32cp_config_t config;
    config.address = 0;
    config.channel = 3;
    config.bootloader_random = false;
    config.key_length = LORA_KEY_LENGTH;
    config.pre_shared_key = (uint8_t *)LORA_PRESHARED_KEY;
    config.lora = &(this->_lora);

    if (this->_protocoll.begin(config) != E32CP_ERR_SUCCESS)
    {
        OZ_LOGE(this->name().c_str(), "Protocol begin error!! ");
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

    e32cp_errno_t err = this->_protocoll.wake_up_asleep(loramessage, client_address, E32_SERVER_CHANNEL);

    if (err == E32CP_ERR_SUCCESS)
    {
        OZ_LOGD(this->name().c_str(), "Wake command ok: [%s] to client : [%u] ", loramessage.c_str(), client_address);
    }
    else
    {
        OZ_LOGE(this->name().c_str(), "Problem occured on sending LoRa [%d]", err);
        OZ_LOGE(this->name().c_str(), "Address : %u , Channel: %u", client_address, E32_SERVER_CHANNEL);
    }
}

void lora_to_mqtt::execute_lora(e32_receve_struct_t message)
{

    String lora_message = this->_protocoll.receve_from(message);

    if (lora_message.length() == 0)
    {
        OZ_LOGW(this->name().c_str(), "LoRa Recieve empty message!!");
        return;
    }

    std::vector<String> message_array = oz_utils::splitString(lora_message.c_str(), LORA_MESSAGE_SEPARATOR_CHAR);

    if (message_array.size() != 2) // Format must be payload&topic
    {
        OZ_LOGW(this->name().c_str(), "LoRa Recieve Wrong message format [%s]", lora_message);
        return;
    }

    OZ_LOGV(this->name().c_str(), "LoRa Recieve [%s]->[%s]", message_array[1].c_str(), message_array[0].c_str());
    OZMQTT.send(message_array[1], message_array[0]);
}

QueueHandle_t lora_to_mqtt::get_queue() { return this->_lora.get_queue(); }