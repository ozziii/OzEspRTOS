#include "plugin_base.h"

#include "gpio_oz.h"
#include "utils.h"

plugin_base::plugin_base(params_t Parameters)
{
    this->mutex = xSemaphoreCreateMutex();
    this->_parameters = Parameters;
}

bool plugin_base::is_initialized() { return this->_initialized && this->name().length() > 0; }

void plugin_base::interrupt(uint8_t pin)
{
    xSemaphoreTake(this->mutex, portMAX_DELAY);
    _interrupt(pin);
    xSemaphoreGive(this->mutex);
}

void plugin_base::send_response(String topic, String message)
{
    xSemaphoreTake(this->mutex, portMAX_DELAY);
    _send_response(topic, message);
    xSemaphoreGive(this->mutex);
}

void plugin_base::execute_sensor()
{
    // called on Interrupt
    xSemaphoreTake(this->mutex, portMAX_DELAY);
    _execute_sensor();
    xSemaphoreGive(this->mutex);
}

void plugin_base::force_update()
{
    xSemaphoreTake(this->mutex, portMAX_DELAY);
    _force_update();
    xSemaphoreGive(this->mutex);
}

std::vector<String> plugin_base::get_mqtt_topic() { return this->_mqtt_topics; }

std::vector<uint8_t> plugin_base::get_interrupt_pins() { return this->_interrups_pin; }

String plugin_base::buildTopic(String action)
{
    String stret;
    stret += SETTING(DB_SETTING_ESPNAME);
    stret += char(MQTT_TOPIC_SEPARATOR_CHAR);
    stret += this->name();

    if (action.length() > 0)
    {
        stret += char(MQTT_TOPIC_SEPARATOR_CHAR);
        stret += action;
    }

    return stret;
}

String plugin_base::get_string_parameter(String parameter_name)
{
    if (this->_parameters.find(parameter_name) != this->_parameters.end())
    {
        return this->_parameters[parameter_name];
    }
    else
    {
        return "";
    }
}

int plugin_base::get_int_parameter(String parameter_name)
{
    if (this->_parameters.find(parameter_name) != this->_parameters.end())
    {
        return atoi(this->_parameters[parameter_name].c_str());
    }
    else
    {
        return -1;
    }
}

float plugin_base::get_float_parameter(String parameter_name)
{
    if (this->_parameters.find(parameter_name) != this->_parameters.end())
    {
        return atof(this->_parameters[parameter_name].c_str());
    }
    else
    {
        return 0;
    }
}

unsigned long plugin_base::get_long_parameter(String parameter_name)
{
    if (this->_parameters.find(parameter_name) != this->_parameters.end())
    {
        return atol(this->_parameters[parameter_name].c_str());
    }
    else
    {
        return 0;
    }
}

unsigned long  plugin_base::sensor_delay()
{
    return this->get_int_parameter(PLUGIN_BASE_STR_DELAY)*1000;
}

String plugin_base::name()
{
    return this->get_string_parameter(PLUGIN_BASE_STR_NAME);
}

void plugin_base::subscribe_topic(String topic)
{
    this->_mqtt_topics.push_back(topic);
}

void plugin_base::attach_interrupt(uint8_t Pin, uint8_t Mode)
{
    if (digitalPinToInterrupt(Pin) > 0)
    {
        GPIOZ.setInterrups(Pin, Mode);
        this->_interrups_pin.push_back(Pin);
    }
}

void plugin_base::attach_touch(uint8_t Pin, uint16_t Threshold)
{
    if (digitalPinToTouchChannel(Pin) > 0)
    {
        GPIOZ.setTouchInterrups(Pin, Threshold);
        this->_interrups_pin.push_back(Pin);
    }
}

uint8_t plugin_base::gpioInputMode(const char *mode)
{
    if (strcasecmp(mode, PLUGIN_BASE_VAL_PULLUP) == 0)
        return INPUT_PULLUP;

    if (strcasecmp(mode, PLUGIN_BASE_VAL_PULLDOWN) == 0)
        return INPUT_PULLDOWN;

    return INPUT;
}

uint8_t plugin_base::getType()
{
    uint8_t ret = 0;

    if (this->_mqtt_topics.size() > 0)
        ret += TYPE_MQTT;

    if (sensor_delay() > 0)
        ret += TYPE_SENSOR;

    if (this->_interrups_pin.size() > 0)
        ret += TYPE_INTERRUPT;

    return ret;
}

void plugin_base::_force_update()
{
    _execute_sensor();
}