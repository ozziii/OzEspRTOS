#include "ram_sensor_p.h"

freeram::freeram(params_t init) : sensor(init)
{
    this->_values.push_back(
        {MQTT_SENSOR_BYTE,  // Topic
         0,                                          // decimal
         std::numeric_limits<float>::max(),        // max
         0,                                        // min
         0});                                        // value

    OZ_LOGI(this->name().c_str(),"Initialization ok!");
    this->_initialized = true;
}

void freeram::_sensor_update()
{
    this->_values[0].value = ESP.getFreeHeap();
}