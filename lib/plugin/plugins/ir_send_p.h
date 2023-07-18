#ifndef IR_SEND_P_H
#define IR_SEND_P_H

#include "plugin_base.h"

#define IR_SEND_STR_FREQUENCY "FrequencyKh"
#define IR_SEND_STR_DUTY_CYCLE "DutyCicle"
#define IR_SEND_STR_HASSIO_CODE "HassioCode"
#define IR_SEND_STR_REVERSE     "Reverse"


#define IR_SEND_MICROS_PER_TICK 50
#define IR_SEND_MESSAGE_SEPARATOR_CHAR 44

/******************************************************************************************************************************************
 *
 *     IR send 
 *
 *     MQTT Message: OnTime,OffTime,OnTime,OffTime,......,OnTime
 *
 *********************************************************************************************************************************************/

class ir_send_plugin : public plugin_base
{
public:
    ir_send_plugin(params_t init);

    static ir_send_plugin *COTR(params_t init) { return new ir_send_plugin(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_PIN, "-1"},
            {IR_SEND_STR_FREQUENCY, "38"},
            {IR_SEND_STR_DUTY_CYCLE, "30"},
            {IR_SEND_STR_HASSIO_CODE, ""},
            {IR_SEND_STR_REVERSE, "0"},
            };
    }

protected:
    void _send_response(String Topic, String Message) override;

private:
    int8_t _pin, _frequency, _dutycicle, _channel;

    uint32_t _on_state,_off_state;

    bool _reverse;
    
    String _topic_action;

    uint8_t _str_to_buffer(String command, uint8_t * buffer, size_t length);

    void _sendRaw(uint8_t * buffer, size_t  length);

    void _turn_on();

    void _turn_off();
};
#endif