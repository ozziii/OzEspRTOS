#include "vmc_p.h"

static void vmc_timer_direction_changeISR(TimerHandle_t xTimer)
{
    auto pin = (uint8_t *)pvTimerGetTimerID(xTimer);
    GPIOZ.switch_Pin(*pin);
}

vmc_plugin::vmc_plugin(params_t init) : plugin_base(init)
{

    this->_enable_pin = this->get_int_parameter(VMC_STR_EN);
    this->_pwm_pin = this->get_int_parameter(VMC_STR_POWER);
    this->_direction_pin = this->get_int_parameter(VMC_STR_DIR);
    this->_enable_logic_on = this->get_int_parameter(VMC_STR_ENLOGIC);
    this->_direction_logic_in = this->get_int_parameter(VMC_STR_DIRLOGIC);

    pinMode(this->_enable_pin, OUTPUT);
    pinMode(this->_direction_pin, OUTPUT);

    digitalWrite(this->_enable_pin, !this->_enable_logic_on);

    GPIOZ.attach_PWM(this->_pwm_pin);

    this->topic_state = buildTopic(MQTT_COMMAND_STATE);
    this->topic_action = buildTopic(MQTT_COMMAND_ACTION);
    this->subscribe_topic(this->topic_action);

    this->_switch_timer = xTimerCreate(
        "vmc_delay_timer",
        pdMS_TO_TICKS(1),
        pdTRUE,
        (void *)(&this->_direction_pin),
        vmc_timer_direction_changeISR);

    this->_mode = VMC_STATE_OFF;

    this->_initialized = true;

    OZ_LOGI(this->name().c_str(), "Initialized ok! Pin [EN:%u] [PMW:%u] [DIR:%u]", this->_enable_pin, this->_pwm_pin, this->_direction_pin);
}

void vmc_plugin::_send_response(String Topic, String Message)
{
    if (Topic.equals(this->topic_action))
    {
        std::vector<String> payload = oz_utils::splitString(Message.c_str(), VMC_PAYLOAD_SEPARATOR_CHAR);

        if (payload.size() != 3)
            return;

        if (payload[0].equals(VMC_STATE_AUTO))
        {
            if(!this->_mode.equals(VMC_STATE_AUTO))
            {
                digitalWrite(this->_direction_pin, !this->_direction_logic_in); // TODO
                digitalWrite(this->_enable_pin, this->_enable_logic_on);
                this->_mode = VMC_STATE_AUTO;
            }
            
            uint32_t delay = atol(payload[2].c_str()) * 1000;

            xTimerChangePeriod(this->_switch_timer, pdMS_TO_TICKS(delay), 0);

            GPIOZ.pwm_write(this->_pwm_pin, this->map_power(payload[1]));
        }

        if (payload[0].equals(VMC_STATE_IN))
        {
            this->_mode = VMC_STATE_IN;

            xTimerDelete(this->_switch_timer, 0);

            digitalWrite(this->_direction_pin, this->_direction_logic_in); // TODO

            GPIOZ.pwm_write(this->_pwm_pin, this->map_power(payload[1]));

            digitalWrite(this->_enable_pin, this->_enable_logic_on);
        }

        if (payload[0].equals(VMC_STATE_OUT))
        {
            this->_mode = VMC_STATE_OUT;

            xTimerDelete(this->_switch_timer, 0);

            digitalWrite(this->_direction_pin, !this->_direction_logic_in); // TODO

            GPIOZ.pwm_write(this->_pwm_pin, this->map_power(payload[1]));

            digitalWrite(this->_enable_pin, this->_enable_logic_on);
        }

        if (payload[0].equals(VMC_STATE_OFF))
        {
            this->_mode = VMC_STATE_OFF;

            xTimerDelete(this->_switch_timer, 0);

            digitalWrite(this->_enable_pin, !this->_enable_logic_on);
            GPIOZ.pwm_write(_pwm_pin, 0);
        }
    }
}

uint16_t vmc_plugin::map_power(String power)
{
    uint16_t pow = atoi(power.c_str());

    pow = pow > 100 ? 100 : pow;

    return map(pow, 0, 100, 0, 1024);
}
