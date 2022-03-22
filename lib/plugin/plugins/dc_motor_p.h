#ifndef DC_MOTOR_P_H
#define DC_MOTOR_P_H

#include "plugin_base.h"

#define MOTOR_MAX_RUN_TIME 20000


#define PLUGIN_DC_MOTOR_JSON_DOCUMENT_SIZE 512

#define DC_MOTOR_STR_PIN_FORWARD     "Fwd_Pin"
#define DC_MOTOR_STR_PIN_BACKWARD    "Bac_Pin"
#define DC_MOTOR_STR_BUTTON_FORWARD  "Fwd_Button"
#define DC_MOTOR_STR_BUTTON_BACKWARD "Bac_Button"
#define DC_MOTOR_STR_LIMIT_FORWARD   "Fwd_Limit"
#define DC_MOTOR_STR_LIMIT_BACKWARD  "Bac_Limit"
#define DC_MOTOR_STR_BUTTON_LOGIC    "Button_Logic"
#define DC_MOTOR_STR_LIMIT_LOGIC     "Limit_Logic"
#define DC_MOTOR_STR_POWER           "Power"

#define RINCORSA

/**
 *            
 *     BISTABLE RELAY PIN STATE
 * 
 *      MQTT Message: { "power": [-100:100] , "time": [ms] }
 * 
 */
class dc_motor : public plugin_base
{
public:
    dc_motor(params_t init);

    static dc_motor * COTR(params_t init) { return new dc_motor(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME       ,PLUGIN_STR_NAN},
            {DC_MOTOR_STR_PIN_FORWARD   ,PLUGIN_STR_NAN},
            {DC_MOTOR_STR_PIN_BACKWARD  ,PLUGIN_STR_NAN},
            {DC_MOTOR_STR_BUTTON_FORWARD,"-1"},
            {DC_MOTOR_STR_BUTTON_BACKWARD,"-1"},
            {DC_MOTOR_STR_LIMIT_FORWARD ,"-1"},
            {DC_MOTOR_STR_LIMIT_BACKWARD ,"-1"},
            {DC_MOTOR_STR_BUTTON_LOGIC  ,"1"},
            {DC_MOTOR_STR_LIMIT_LOGIC   ,"1"},
            {DC_MOTOR_STR_POWER         ,"100"}
        };
    }


    void _stop();

protected:
    void _send_response(String Topic, String Message) override;

    void _interrupt(uint8_t pin) override;

    void _forward(uint8_t power,unsigned long time=0);
    void _backward(uint8_t power,unsigned long time=0);

    int16_t _forward_button,_backward_button, _forward_pin,_backward_pin,_forward_limit,_backward_limit;

    bool _logic_button,_logic_limit;

    uint8_t _base_power;

    TimerHandle_t _stop_timer;

    String topic_action;
};









#endif
