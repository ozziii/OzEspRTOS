#ifndef BUZZER_P_H
#define BUZZER_P_H

#include "plugin_base.h"

#define BELL_COMMA_SEPARATOR_CHAR 44

static const uint8_t bell_plugin_Key = 10;
/******************************************************************************************************************************************
*            
*     Buzzer 
*
*     MQTT COMMAND   ON  |   OFF |    [time ON/10 ms],[time OFF/10 ms],[repetitions]
*
*********************************************************************************************************************************************/
class buzzer : public plugin_base
{
public:
    buzzer(params_t init);

    static buzzer * COTR(params_t init) { return new buzzer(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_PIN,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_LOGIC,"1"}
        };
    }

protected:
    void _send_response(String Topic, String Message) override;

private:
    int16_t _pin;
    bool _logic;
    String _topic_action;
};

#endif