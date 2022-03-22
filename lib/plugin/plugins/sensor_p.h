#ifndef SENSOR_P_H
#define SENSOR_P_H

#include <vector>

#include "plugin_base.h"

struct sensor_values
{
    String topic;
    uint8_t decimal;
    float max;
    float min;
    float value;
};


/**
 * 
 *  Abstract sensor plugin for ambient mesurament (Temperature, humidity, pressure, Air Quality, DrewPoint)
 * 
 */
class sensor : public plugin_base
{
public:
    sensor(params_t init);

protected:
    void _execute_sensor() override;

    virtual void _sensor_update() = 0;

    std::vector<sensor_values> _values;

    String _topic_state;
};

#endif