#include "pwm_switch_p.h"

pwm_switch::pwm_switch(params_t init) : switch_p(init)
{
    if (this->_initialized)
    {
        this->_initialized = false;

        this->_pwm_value = this->get_int_parameter(PLUGIN_PWM_SWITCH_STR_PWM);
        if (_pwm_value > PLUGIN_PWM_SWITCH_MIN && _pwm_value < PLUGIN_PWM_SWITCH_MAX)
        {
            GPIOZ.attach_PWM(this->_action_pin);
            this->_initialized = true;
        }
    }
}

void pwm_switch::_io_action(uint8_t state)
{
    GPIOZ.pwm_write(this->_action_pin,this->_pwm_value*state);
}