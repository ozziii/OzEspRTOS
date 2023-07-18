#ifndef PWM_SWITCH_P_H
#define PWM_SWITCH_P_H

#include "plugins/switch_p.h"



#define PLUGIN_PWM_SWITCH_STR_PWM "PWM"


#define PLUGIN_PWM_SWITCH_MIN 0
#define PLUGIN_PWM_SWITCH_MAX 1024

/**************************************************************************
*            
*     PWM Switch; If set, read status by external Pin (READ PIN)
*
***************************************************************************/


class pwm_switch : public switch_p
{
public:
    pwm_switch(params_t init);

    static pwm_switch * COTR(params_t init) { return new pwm_switch(init); }

    static params_t PARAM()
    {
        params_t param =  switch_p::PARAM();

        param.insert({PLUGIN_PWM_SWITCH_STR_PWM, PLUGIN_STR_NAN});

        return param;
    }

protected:
    void _io_action(uint8_t state) override;

    uint16_t _pwm_value;

};

#endif