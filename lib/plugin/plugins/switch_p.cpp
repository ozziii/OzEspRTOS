#include "switch_p.h"

// 0   1      2         3              4             5               6              7                         8                  9
//#/[Name]/[Pin]/[Delay (s)]=0/[Logic 1:0]=1/[Start Stat 1:0]=0/[Read Pin]=-1/[Interrupt ON/OFF 1:0]=1/[Read logic 1:0]=1/[Pull UP/Down 1:0]=0
switch_p::switch_p(params_t init) : plugin_base(init)
{

    this->_action_pin = this->get_int_parameter(PLUGIN_BASE_STR_PIN);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(this->_action_pin))
    {
        OZ_LOGW(this->name().c_str(), "Action pin is not valid GPIO");
        return;
    }
    pinMode(this->_action_pin, OUTPUT);

    this->_action_logic = this->get_int_parameter(PLUGIN_BASE_STR_LOGIC);

    bool status = this->get_int_parameter(PLUGIN_BASE_STR_DEFSTATE);
    digitalWrite(this->_action_pin, status);
    this->_state = status;

    //#################  READ PIN ###############

    this->_read_pin = this->get_int_parameter(SWITCH_STR_RADPIN);
    if (!GPIO_IS_VALID_GPIO(this->_read_pin))
    {
        this->_read_pin = -1;
    }
    else
    {
        this->_read_logic = this->get_int_parameter(SWITCH_STR_RADLOGIC);
        uint8_t mode = this->gpioInputMode(this->get_string_parameter(PLUGIN_BASE_STR_INTERRUPT).c_str());
        pinMode(this->_read_pin, mode);
        this->_enable_interrupt = this->get_int_parameter(PLUGIN_BASE_STR_INTERRUPT);
        if (this->_enable_interrupt)
            this->attach_interrupt(this->_read_pin, CHANGE);
    }

    this->topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
    this->topic_state = this->buildTopic(MQTT_COMMAND_STATE);

    this->subscribe_topic(this->topic_action);

    this->_initialized = true;
    OZ_LOGI(this->name().c_str(), "Initialized ok!");
}

void switch_p::_send_response(String Topic, String Message)
{
    if (Topic.equals(this->topic_action))
    {
        if (Message.equals(MQTT_STATE_OFF))
        {
            if (_read_state() == this->_read_logic)
            {
                this->_switch_state(!this->_action_logic);
            }
        }

        if (Message.equals(MQTT_STATE_ON))
        {
            if (_read_state() != this->_read_logic)
            {
                this->_switch_state(this->_action_logic);
            }
        }

        if (!this->_enable_interrupt && this->sensor_delay() == 0)
        {
            delay(200);
            this->_publish_state(false);
        }
    }
}

void switch_p::_interrupt(uint8_t Pin)
{
    if (Pin == this->_read_pin)
        this->_publish_state(false);
}

void switch_p::_execute_sensor()
{
    this->_publish_state(true);
}

void switch_p::_force_update()
{
    this->_publish_state(false);
}

void switch_p::_switch_state(uint8_t State)
{
    _io_action(State);

    if (this->_read_pin < 0)
    {
        this->_state = State == this->_action_logic;
    }
}

bool switch_p::_read_state()
{
    if (this->_read_pin < 0)
    {
        return this->_state;
    }
    else
    {
        return digitalRead(this->_read_pin);
    }
}

void switch_p::_publish_state(bool ifNotChange)
{
    if (this->_read_pin < 0)
    {
        const char *state = this->_state == this->_read_logic ? MQTT_STATE_ON : MQTT_STATE_OFF;
        OZMQTT.send(this->topic_state.c_str(), state);
    }
    else
    {
        if (ifNotChange)
        {
            bool current_state = digitalRead(this->_read_pin);
            if (this->_state != current_state)
            {
                this->_state = current_state;
                const char *state = current_state == this->_read_logic ? MQTT_STATE_ON : MQTT_STATE_OFF;
                OZMQTT.send(this->topic_state.c_str(), state);
            }
        }
        else
        {
            this->_state = digitalRead(this->_read_pin);
            const char *state = this->_state == this->_read_logic ? MQTT_STATE_ON : MQTT_STATE_OFF;
            OZMQTT.send(this->topic_state.c_str(), state);
        }
    }
}

void switch_p::_io_action(uint8_t state)
{
    digitalWrite(this->_action_pin, state);
}
