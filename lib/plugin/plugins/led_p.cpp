#include "led_p.h"

#include "ArduinoJson.h"

// 0   1            2                   3                      4                  5
//#/[NAME]/[Wite pin or R-G-B ]/[Dimmer Logic ON 1:0] = 1/[On-Off pin]=-1/[ON-Off Logic ON 1:0] = 1
led_plugin::led_plugin(params_t init) : plugin_base(init)
{

    this->wite_pin = this->get_int_parameter(LED_PLUGIN_STR_WITE);
    if (GPIO_IS_VALID_GPIO(this->wite_pin))
    {
        GPIOZ.attach_PWM(this->wite_pin);
        this->green_pin = -1;
        this->blue_pin = -1;
        this->red_pin = -1;
    }
    else
    {
        this->wite_pin = - 1;

        this->green_pin = this->get_int_parameter(LED_PLUGIN_STR_GREEN);
        this->blue_pin = this->get_int_parameter(LED_PLUGIN_STR_BLUE);
        this->red_pin = this->get_int_parameter(LED_PLUGIN_STR_RED);

        if (!GPIO_IS_VALID_OUTPUT_GPIO(this->red_pin))
        {
            OZ_LOGW(this->name().c_str(), "Red pin is not valid GPIO");
            return;
        }

        if (!GPIO_IS_VALID_OUTPUT_GPIO(this->green_pin))
        {
            OZ_LOGW(this->name().c_str(), "Green pin is not valid GPIO");
            return;
        }

        if (!GPIO_IS_VALID_OUTPUT_GPIO(this->blue_pin))
        {
            OZ_LOGW(this->name().c_str(), "Blue pin is not valid GPIO");
            return;
        }

        GPIOZ.attach_PWM(this->red_pin);
        GPIOZ.attach_PWM(this->green_pin);
        GPIOZ.attach_PWM(this->blue_pin);
    }

    bool dimmer_logic = this->get_int_parameter(LED_PLUGIN_STR_DIMMER_LOGIC);

    this->min = dimmer_logic ? PLUGIN_LED_MIN_PWM : PLUGIN_LED_MAX_PWM;
    this->max = dimmer_logic ? PLUGIN_LED_MAX_PWM : PLUGIN_LED_MIN_PWM;

    this->onoff_pin = this->get_int_parameter(LED_PLUGIN_STR_ONOFFLOGIC);

    if (GPIO_IS_VALID_OUTPUT_GPIO(this->onoff_pin))
    {
        pinMode(this->onoff_pin, OUTPUT);
    }
    else
    {
        this->onoff_pin = -1;
    }

    this->onoff_pin_logic = this->get_int_parameter(LED_PLUGIN_STR_ONOFFLOGIC);

    this->topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
    this->topic_state = this->buildTopic(MQTT_COMMAND_STATE);

    this->subscribe_topic(this->topic_action.c_str());

    this->IsOn = false;
    this->brightness = PLUGIN_DEFAULT_BRIGHTNESS;
    this->red = 0;
    this->green = 0;
    this->blue = 0;

    this->_initialized = true;
    OZ_LOGI(this->name().c_str(), "LED Initialization ok!! ");
}

void led_plugin::_send_response(String Topic, String Message)
{
    if (this->topic_action.equals(Topic))
    {
        DynamicJsonDocument JsonDocument(PLUGIN_LED_JSON_DOCUMENT_SIZE);
        auto error = deserializeJson(JsonDocument, Message);

        if (error)
        {
            OZ_LOGW(this->name().c_str(), "Wrong json message");
            return;
        }

        if(JsonDocument.containsKey("state"))
        {
            String state = JsonDocument["state"];
            if(state.equals(MQTT_STATE_ON))
                this->IsOn = true;

            if(state.equals(MQTT_STATE_OFF))
                this->IsOn = false;
        }

        if(JsonDocument.containsKey("brightness"))
            this->brightness = JsonDocument["brightness"];

        /*
        if (this->wite_pin < 0)
        {
            if (JsonDocument.containsKey("color"))
            {
                if (JsonDocument["color"].containsKey("r"))
                    this->red = JsonDocument["color"]["r"];
                if (JsonDocument["color"].containsKey("g"))
                    this->green = JsonDocument["color"]["g"];
                if (JsonDocument["color"].containsKey("b"))
                    this->blue = JsonDocument["color"]["b"];
            }
        }
        */
        this->_update();
        this->_publish();
    }
}

void led_plugin::_update()
{
    if (this->IsOn)
    {
        if (this->onoff_pin > 0)
        {
            digitalWrite(this->onoff_pin, this->onoff_pin_logic);
        }

        this->_setPWM(this->brightness);
    }
    else
    {
        if (this->onoff_pin > 0)
        {
            digitalWrite(this->onoff_pin, !this->onoff_pin_logic);
        }

        this->_setPWM(0);
    }
}

void led_plugin::_setPWM(uint8_t Brightness)
{
    if (this->wite_pin < 0)
    {
        uint16_t p_bright = map(Brightness, 0, 255, this->min, this->max);
        GPIOZ.pwm_write(this->red_pin, map(this->red, 0, 255, this->min, p_bright));
        GPIOZ.pwm_write(this->green_pin, map(this->green, 0, 255, this->min, p_bright));
        GPIOZ.pwm_write(this->blue_pin, map(this->blue, 0, 255, this->min, p_bright));
    }
    else
    {
        GPIOZ.pwm_write(this->wite_pin, map(Brightness, 0, 255, this->min, this->max));
    }
}

void led_plugin::_force_update()
{
    this->_publish();
}

void led_plugin::_publish()
{
    DynamicJsonDocument JsonDocument(PLUGIN_LED_JSON_DOCUMENT_SIZE);

    JsonDocument["state"] = this->IsOn ? MQTT_STATE_ON : MQTT_STATE_OFF;
    JsonDocument["brightness"] = this->brightness;

    /*
    if (wite_pin < 0)
    {
        JsonDocument["color_mode"] = "rgb";
        JsonDocument["color"]["r"] = this->red;
        JsonDocument["color"]["g"] = this->green;
        JsonDocument["color"]["b"] = this->blue;
    }
    else
    {
        //JsonDocument["color_mode"] = "brightness";
        //JsonDocument["color"]["r"] = 0;
        //JsonDocument["color"]["g"] = 0;
        //JsonDocument["color"]["b"] = 0;
    }
    */

    String message;
    serializeJson(JsonDocument, message);
    OZMQTT.send(this->topic_state, message);
}
