#include "dc_motor_p.h"

#include "ArduinoJson.h"

static void dc_motor_stopISR(TimerHandle_t xTimer)
{
    auto self = (dc_motor *)pvTimerGetTimerID(xTimer);
    self->_stop();
}

// 0    1       2              3            4           5                   6                        7                    8                  9                   10
//#/[Name]/[Pin Forward]/[Pin Backward]/[Power]=100/[Button Forward]=-1/[Button Backward]=-1/[Button Logic 1:0]=1/[limit Forward]=-1/[limit Backward]=-1/[Limit Logic 1:0]=1
dc_motor::dc_motor(params_t init) : plugin_base(init)
{

    this->_forward_pin = this->get_int_parameter(DC_MOTOR_STR_PIN_FORWARD);
    if (!GPIO_IS_VALID_GPIO(this->_forward_pin))
    {
        OZ_LOGW(this->name().c_str(), "Forward Pin is not valid GPIO");
        return;
    }

    this->_backward_pin = this->get_int_parameter(DC_MOTOR_STR_PIN_BACKWARD);
    if (!GPIO_IS_VALID_GPIO(this->_backward_pin))
    {
        OZ_LOGW(this->name().c_str(), "Backward Pin is not valid GPIO");
        return;
    }

    GPIOZ.attach_PWM(this->_forward_pin);
    GPIOZ.attach_PWM(this->_backward_pin);

    this->_base_power = this->get_int_parameter(DC_MOTOR_STR_POWER);

    this->_forward_button = this->get_int_parameter(DC_MOTOR_STR_BUTTON_FORWARD);
    if (GPIO_IS_VALID_GPIO(this->_forward_button))
    {
        pinMode(this->_forward_button, INPUT);
        this->attach_interrupt(this->_forward_button, CHANGE);
    }
    else
    {
        this->_forward_button = -1;
    }

    this->_backward_button = this->get_int_parameter(DC_MOTOR_STR_BUTTON_BACKWARD);
    if (GPIO_IS_VALID_GPIO(this->_backward_button))
    {
        pinMode(this->_backward_button, INPUT);
        this->attach_interrupt(this->_backward_button, CHANGE);
    }
    else
    {
        this->_backward_button = -1;
    }

    this->_logic_button = this->get_int_parameter(DC_MOTOR_STR_BUTTON_LOGIC);

    this->_forward_limit = this->get_int_parameter(DC_MOTOR_STR_LIMIT_FORWARD);
    if (GPIO_IS_VALID_GPIO(this->_forward_limit))
    {
        pinMode(this->_forward_limit, INPUT);
        this->attach_interrupt(this->_forward_limit, CHANGE);
    }
    else
    {
        this->_forward_limit = -1;
    }

    this->_backward_limit = this->get_int_parameter(DC_MOTOR_STR_LIMIT_BACKWARD);
    if (GPIO_IS_VALID_GPIO(this->_backward_limit))
    {
        pinMode(this->_backward_limit, INPUT);
        this->attach_interrupt(this->_backward_limit, CHANGE);
    }
    else
    {
        this->_backward_limit = -1;
    }

    this->_logic_limit = this->get_int_parameter(DC_MOTOR_STR_LIMIT_LOGIC);

    this->topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
    this->subscribe_topic(this->topic_action);

    this->_stop_timer = xTimerCreate(
        "dc_motor_p_timer",
        pdMS_TO_TICKS(1),
        pdFALSE,
        (void *)this,
        dc_motor_stopISR);

    this->_initialized = true;

    OZ_LOGI(this->name().c_str(), "Initialized ok! ");
}

void dc_motor::_send_response(String Topic, String Message)
{
    if (this->topic_action.equals(Topic))
    {
        DynamicJsonDocument JsonDocument(PLUGIN_DC_MOTOR_JSON_DOCUMENT_SIZE);
        auto error = deserializeJson(JsonDocument, Message);

        if (error)
        {
            OZ_LOGE(this->name().c_str(), "Wrong json message");
            return;
        }

        int power = JsonDocument["power"];
        int time = JsonDocument["time"];
        JsonDocument.clear();

        OZ_LOGV(this->name().c_str(), "Recive Power [%d]  Time [%d] ms ", power, time);

        if (power > 0)
        {
            if (power > 100)
                power = 100;
#ifdef RINCORSA            
            this->_backward(power, 100);
            vTaskDelay(pdMS_TO_TICKS(120));
#endif
            this->_forward(power, time);
        }
        else if (power < 0)
        {
            if (power < -100)
                power = -100;

            this->_backward(abs(power), time);
        }
        else if (power == 0)
        {
            this->_stop();
        }
    }
}

void dc_motor::_interrupt(uint8_t Pin)
{
    if (Pin == this->_forward_button)
    {
        vTaskDelay(pdMS_TO_TICKS(150)); // DEBOUNCING 

        if (digitalRead(Pin) == this->_logic_button)
        {  
#ifdef RINCORSA            
            this->_backward(this->_base_power, 100); 
            vTaskDelay(pdMS_TO_TICKS(120)); 
#endif 
            this->_forward(this->_base_power, 0);
            // ANTI DEBOUNCING 
            vTaskDelay(pdMS_TO_TICKS(500));
            if(digitalRead(Pin) != this->_logic_button)
                this->_stop();
        }
        else
        {
            this->_stop();
        }
    }
    else if (Pin == this->_backward_button)
    {
        vTaskDelay(pdMS_TO_TICKS(150)); // DEBOUNCING 

        if (digitalRead(Pin) == this->_logic_button)
        {
            this->_backward(this->_base_power, 0);

            vTaskDelay(pdMS_TO_TICKS(500));
            if(digitalRead(Pin) != this->_logic_button)
                this->_stop();
        }
        else
        {
            this->_stop();
        }
    }
    else if (Pin == this->_forward_limit)
    {
        vTaskDelay(pdMS_TO_TICKS(150)); // DEBOUNCING 
        if (digitalRead(Pin) == this->_logic_limit)
        {
            this->_stop();
        }
        else
        {
        }
    }
    else if (Pin == this->_backward_limit)
    {
        vTaskDelay(pdMS_TO_TICKS(150)); // DEBOUNCING 
        if (digitalRead(Pin) == this->_logic_limit)
        {
            this->_stop();
        }
        else
        {
        }
    }
}

void dc_motor::_forward(uint8_t power, unsigned long time)
{
    if (this->_forward_limit < 0 || digitalRead(this->_forward_limit) == this->_logic_limit)
    {
        uint pvalue = map(power, 0, 100, 0, 1024);

        GPIOZ.pwm_write(this->_forward_pin, pvalue);


        GPIOZ.pwm_write(this->_backward_pin, 0);

        if (time > 0)
        {
            if (xTimerIsTimerActive(this->_stop_timer))
            {
                xTimerDelete(this->_stop_timer, 0);
            }

            xTimerChangePeriod(this->_stop_timer, pdMS_TO_TICKS(time), 0);
        }
    }
}

void dc_motor::_backward(uint8_t power, unsigned long time)
{
    if (this->_backward_limit < 0 || digitalRead(this->_backward_limit) == this->_logic_limit)
    {
        GPIOZ.pwm_write(this->_forward_pin, 0);
        uint pvalue = map(power, 0, 100, 0, 1024);
        GPIOZ.pwm_write(this->_backward_pin, pvalue);

        if (time > 0)
        {
            if (xTimerIsTimerActive(this->_stop_timer))
            {
                xTimerDelete(this->_stop_timer, 0);
            }

            xTimerChangePeriod(this->_stop_timer, pdMS_TO_TICKS(time), 0);
        }
    }
}

void dc_motor::_stop()
{
    GPIOZ.pwm_write(this->_forward_pin, 0);
    GPIOZ.pwm_write(this->_backward_pin, 0);
}
