#ifndef SWITCH_P_H
#define SWITCH_P_H

#include "plugin_base.h"

#define SWITCH_STR_RADPIN "Read_Pin"
#define SWITCH_STR_RADLOGIC "Read_Logic"

/******************************************************************************************************************************************
*
*     Single digital pin control; If set, read status by external Pin (READ PIN)
*
*********************************************************************************************************************************************/

class switch_p : public plugin_base
{
public:
    switch_p(params_t init);

    static switch_p *COTR(params_t init) { return new switch_p(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY, "0"},

            {PLUGIN_BASE_STR_PIN, PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_LOGIC, "1"},
            {PLUGIN_BASE_STR_DEFSTATE, "0"},

            {SWITCH_STR_RADPIN, "-1"},
            {SWITCH_STR_RADLOGIC, "1"},
            {PLUGIN_BASE_STR_INTERRUPT, "0"},
            {PLUGIN_BASE_STR_CLASS,""},
        };
    }

protected:
    void _send_response(String Topic, String Message) override;

    void _interrupt(uint8_t pin) override;

    void _execute_sensor() override;

    void _force_update() override;

    void _switch_state(uint8_t state);

    void _publish_state(bool ifNotChange);

    bool _read_state();

    virtual void _io_action(uint8_t state);

    String topic_state, topic_action;
    int8_t _action_pin, _read_pin;
    bool _action_logic = true, _read_logic = true, _enable_interrupt = false, _state = false;
};

#endif