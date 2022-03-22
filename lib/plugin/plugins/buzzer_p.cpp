#include "buzzer_p.h"

// {#/[NAME]/[Pin]/[Logic ON 1:0]=1}
buzzer::buzzer(params_t init) : plugin_base(init)
{

    this->_pin = this->get_int_parameter(PLUGIN_BASE_STR_PIN);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(this->_pin))
    {
        OZ_LOGW(this->name().c_str(), "Pin is not valid GPIO");
        return;
    }

    this->_logic = this->get_int_parameter(PLUGIN_BASE_STR_LOGIC);

    pinMode(this->_pin, OUTPUT);
    digitalWrite(this->_pin, !this->_logic);

    this->_topic_action = buildTopic(MQTT_COMMAND_ACTION);
    this->subscribe_topic(this->_topic_action);

    this->_initialized = true;

    OZ_LOGI(this->name().c_str(), "Initialized ok! Pin: %u ", this->_pin);
}

void buzzer::_send_response(String Topic, String Message)
{
    if (!Topic.equals(this->_topic_action))
        return;

    if (Message.equals(MQTT_STATE_OFF))
    {
        digitalWrite(this->_pin, !this->_logic);
        return;
    }

    if (Message.equals(MQTT_STATE_ON))
    {
        digitalWrite(this->_pin, this->_logic);
        return;
    }

    auto command = oz_utils::splitString(Message.c_str(), BELL_COMMA_SEPARATOR_CHAR);

    if (command.size() == 3)
    {
        uint16_t run = atoi(command[0].c_str());
        uint16_t stop = atoi(command[1].c_str());
        uint16_t times = atoi(command[2].c_str());

        OZ_LOGV(this->name().c_str(), "times: %u | stop: %u | run: %u ", times, stop, run);

        for (int i = 0; i < times; i++)
        {
            digitalWrite(this->_pin, this->_logic);
            delay(run);
            digitalWrite(this->_pin, !this->_logic);
            delay(stop);
        }
    }
}