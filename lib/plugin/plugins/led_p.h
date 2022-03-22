#ifndef LED_PLUGIN_H
#define LED_PLUGIN_H

#include "plugin_base.h"

#define LED_PLUGIN_STR_WITE "Wite"
#define LED_PLUGIN_STR_RED "Red"
#define LED_PLUGIN_STR_GREEN "Green"
#define LED_PLUGIN_STR_BLUE "Blue"
#define LED_PLUGIN_STR_DIMMER_LOGIC "Dimmer_Logic"
#define LED_PLUGIN_STR_ONOFFPIN "OnOff_Pin"
#define LED_PLUGIN_STR_ONOFFLOGIC "OnOff_Logic"

/******************************************************************************************************************************************
 *
 *     WITE AND RGB LED CONTROL
 *
 *     MQTT Message: { "state": "ON"/"OFF"  , "brightness": [0-255] }
 *
 *********************************************************************************************************************************************/

class led_plugin : public plugin_base
{
public:
    led_plugin(params_t init);

    static led_plugin *COTR(params_t init) { return new led_plugin(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {LED_PLUGIN_STR_WITE, "-1"},
            {LED_PLUGIN_STR_RED, "-1"},
            {LED_PLUGIN_STR_GREEN, "-1"},
            {LED_PLUGIN_STR_BLUE, "-1"},
            {LED_PLUGIN_STR_DIMMER_LOGIC, "1"},
            {LED_PLUGIN_STR_ONOFFPIN, "-1"},
            {LED_PLUGIN_STR_ONOFFLOGIC, "1"}};
    }

protected:
    void _send_response(String Topic, String Message) override;

    void _force_update() override;

private:
    int8_t wite_pin, red_pin, green_pin, blue_pin, onoff_pin;

    bool IsOn, onoff_pin_logic;

    uint8_t brightness, red, green, blue;

    uint16_t min, max;

    String topic_state;
    String topic_action;

    void _publish();
    void _update();
    void _setPWM(uint8_t Brightness);
};
#endif