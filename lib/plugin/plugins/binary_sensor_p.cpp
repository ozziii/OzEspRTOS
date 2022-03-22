#include "binary_sensor_p.h"

//{[*]/[NAME]/[read pin]/[delay (s)]=0/[read logic 1:0]=1/[Pull up 1:0]=0}
binary_sensor::binary_sensor(params_t init) : plugin_base(init)
{

    this->_pin = this->get_int_parameter(PLUGIN_BASE_STR_PIN);
    if (!GPIO_IS_VALID_GPIO(this->_pin))
    {
        OZ_LOGW(this->name().c_str(), "Pin is not valid GPIO");
        return;
    }

    this->_logic_ON = this->get_int_parameter(PLUGIN_BASE_STR_LOGIC);
 
    uint8_t input = this->gpioInputMode(this->get_string_parameter(PLUGIN_BASE_STR_PULL).c_str());
    
    pinMode(this->_pin, input);

    this->attach_interrupt(this->_pin, CHANGE);
    this->_topic_state = this->buildTopic(MQTT_COMMAND_STATE);
    this->_initialized = true;

    OZ_LOGI(this->name().c_str(), "Initilized ok! Pin : %u Delay: %lu", this->_pin, this->sensor_delay());
}

void binary_sensor::_interrupt(uint8_t Pin)
{
    if (this->_pin != Pin)
        return;

    const char *state = digitalRead(this->_pin) == this->_logic_ON ? MQTT_STATE_ON : MQTT_STATE_OFF;
    OZMQTT.send(this->_topic_state.c_str(), state);
}

void binary_sensor::_execute_sensor()
{
    bool new_state = digitalRead(this->_pin) == this->_logic_ON;

    if (new_state != this->_state)
    {
        this->_state = new_state;
        const char *str_state = this->_state ? MQTT_STATE_ON : MQTT_STATE_OFF;
        OZMQTT.send(this->_topic_state.c_str(), str_state);
    }
}

void binary_sensor::_force_update()
{
    this->_state = digitalRead(this->_pin) == this->_logic_ON;
    const char *str_state = this->_state ? MQTT_STATE_ON : MQTT_STATE_OFF;
    OZMQTT.send(this->_topic_state.c_str(), str_state);
}
