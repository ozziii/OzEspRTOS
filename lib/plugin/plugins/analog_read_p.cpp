#include "analog_read_p.h"

analog_read::analog_read(params_t init) : plugin_base(init)
{
    analogSetAttenuation(ANALOG_READ_PLUGIN_ATTENUATION);
    //analogSetWidth(ANALOG_READ_PLUGIN_RESOLUTION);

    this->_topic_value = this->buildTopic(MQTT_SENSOR_VALUE);

    this->_pin = this->get_int_parameter(PLUGIN_BASE_STR_PIN);

    if(digitalPinToAnalogChannel(this->_pin) < 0){
        OZ_LOGW(this->name().c_str(), "Pin %u is not ADC pin!", this->_pin);
        return ;
    }

    this->_multiplier = this->get_float_parameter(ANALOG_READ_STR_MULTIPLIER);

    OZ_LOGI(this->name().c_str(), "Initialized ok! Pin: %u  Delay: %lu [ms]  Multiplier: %f ", this->_pin, this->sensor_delay(), this->_multiplier);
    this->_initialized = true;
}

void analog_read::_execute_sensor()
{
    char *char_value = new char[8];

    dtostrf(analogRead(this->_pin) * this->_multiplier, 6, 2, char_value);

    OZMQTT.send(this->_topic_value, char_value);

    delete[] char_value;
}
