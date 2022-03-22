#ifndef ANALOG_READ_P_H
#define ANALOG_READ_P_H

#include "plugin_base.h"

#define ANALOG_READ_STR_MULTIPLIER "Multiplier"

/**       
 *     ANALOG READER
 */
class analog_read : public plugin_base
{
public:
    analog_read(params_t Parameters);

    static analog_read * COTR(params_t Parameters) { return new analog_read(Parameters); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
            {PLUGIN_BASE_STR_PIN,PLUGIN_STR_NAN},
            {ANALOG_READ_STR_MULTIPLIER,"1"},
            {PLUGIN_BASE_STR_CLASS,""},
        };
    }
protected:
    void _execute_sensor() override;

private:
    float _multiplier;
    uint8_t _pin;
    String _topic_value;
};

#endif