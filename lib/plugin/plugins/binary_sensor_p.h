#ifndef BINARIY_SENSOR_H
#define BINARIY_SENSOR_H

#include "plugin_base.h"

/********************************************************************************************
*            
*     Binay Sensor
*
*     Pull ->>  up  |   down  |  ""
*
********************************************************************************************/

class binary_sensor : public plugin_base
{
public:
    binary_sensor(params_t init);

    void _interrupt(uint8_t pin) override;
  
    void _execute_sensor() override;

    void _force_update() override;

    static binary_sensor * COTR(params_t  init) { return new binary_sensor(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_PIN,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
            {PLUGIN_BASE_STR_LOGIC,"1"},
            {PLUGIN_BASE_STR_PULL,""},
            {PLUGIN_BASE_STR_CLASS,""},
        };
    }
protected:
    int16_t _pin;
    bool _logic_ON;
    bool _state = false;

    String _topic_state;
};

#endif