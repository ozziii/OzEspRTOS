#ifndef VMC_PLUGIN_H
#define VMC_PLUGIN_H

#include <plugin_base.h>

#define VMC_STR_EN       "Enable"
#define VMC_STR_POWER     "Power"
#define VMC_STR_DIR      "Direction"
#define VMC_STR_ENLOGIC  "Enable_Logic"
#define VMC_STR_DIRLOGIC "Dir_Logic"
/**
*            
*     Ventilazione Meccanica Controllata 
*
*      mqtt message {   state,power,delay   }
*
*      state in [auto,in,out,off]
*      power 0 to 100
*      delay in second
*/
class vmc_plugin : public plugin_base
{
public:
    vmc_plugin(params_t  init);

    static vmc_plugin *COTR(params_t init) { return new vmc_plugin(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {VMC_STR_EN       ,PLUGIN_STR_NAN},
            {VMC_STR_POWER    ,PLUGIN_STR_NAN},
            {VMC_STR_DIR      ,PLUGIN_STR_NAN},
            {VMC_STR_ENLOGIC  ,"1"},
            {VMC_STR_DIRLOGIC ,"1"}
        };
    }

protected:
    void _send_response(String Topic, String Message) override;

    uint16_t map_power(String power);

private:
    // PIN DEFINITION
    uint8_t _enable_pin, _pwm_pin, _direction_pin;

    // LOGIC DEFINITION
    bool _direction_logic_in, _enable_logic_on;

    // SCWITCH DIRECTION THREAD
    TimerHandle_t _switch_timer;

    // mqtt topic
    String topic_state, topic_action, _mode;
};

#endif