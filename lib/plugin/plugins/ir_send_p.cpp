#include "ir_send_p.h"

ir_send_plugin::ir_send_plugin(params_t init) : plugin_base(init)
{
    this->_pin = this->get_int_parameter(PLUGIN_BASE_STR_PIN);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(this->_pin))
    {
        OZ_LOGW(this->name().c_str(), "Pin is not valid output GPIO");
        return;
    }

    this->_frequency = this->get_int_parameter(IR_SEND_STR_FREQUENCY);
    this->_dutycicle = this->get_int_parameter(IR_SEND_STR_DUTY_CYCLE);
    this->_reverse = this->get_int_parameter(IR_SEND_STR_REVERSE);

    this->_topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
    this->subscribe_topic(this->_topic_action);

    pinMode(this->_pin, OUTPUT_OPEN_DRAIN);
    this->_channel = GPIOZ.attach_PWM(this->_pin, this->_frequency * 1000, 8);

    if (this->_reverse)
    {
        this->_on_state = ((100 - this->_dutycicle) * 256) / 100;
        this->_off_state = 256;
        ledcWrite(this->_channel, 256); //  * 256 since we have 8 bit resolution
        OZ_LOGI(this->name().c_str(), "IR sender set in inverse mode ");
    }
    else
    {
        this->_on_state = (this->_dutycicle * 256) / 100;
        this->_off_state = 0;
        OZ_LOGI(this->name().c_str(), "IR sender set in normal mode");
    }

    this->_initialized = true;
    OZ_LOGI(this->name().c_str(),
            "IR sender Initializate pin[%u] freq[%u]kh  duty cicle[%u]% ",
            this->_pin,
            this->_frequency,
            this->_dutycicle);
}

void ir_send_plugin::_send_response(String Topic, String Message)
{
    if (!Topic.equals(this->_topic_action))
        return;

    size_t command_len = Message.length();

    if (command_len == 0)
        return;

    uint8_t *buffer = (uint8_t *)malloc(command_len + 1);

    if (buffer == nullptr)
    {
        OZ_LOGW(this->name().c_str(), "Send Ir Command Error to allocate buffer memory. ");
        return;
    }

    size_t err = this->_str_to_buffer(Message, buffer, command_len);

    if (err > 0)
    {
        OZ_LOGD(this->name().c_str(), "Get buffer of %u element", err);
        this->_sendRaw(buffer, err);
    }
    else
    {
        OZ_LOGW(this->name().c_str(), "Send Ir Command Error command conversion. ");
    }

    free(buffer);
}

uint8_t ir_send_plugin::_str_to_buffer(String command, uint8_t *buffer, size_t length)
{
    if (command.length() > length)
        return 0;

    char c = char(IR_SEND_MESSAGE_SEPARATOR_CHAR);

    uint16_t index = 0;
    int i2 = 0;
    int i = command.indexOf(c);

    while (i > 0)
    {
        String item = command.substring(i2, i);
        if (item.length() != 0 && item.indexOf(c) < 0)
        {
            buffer[index] = atoi(item.c_str());
            index++;
        }
        i2 = i + 1;
        i = command.indexOf(c, i2);
    }

    String item = command.substring(i2, command.length());
    if (item.length() != 0 && item.indexOf(c) < 0)
    {
        buffer[index] = atoi(item.c_str());
        index++;
    }

    return index;
}

void ir_send_plugin::_sendRaw(uint8_t *buffer, size_t length)
{
    for (uint_fast8_t i = 0; i < length; i++)
    {
        if (i & 1)
        {
            // Odd
            ets_delay_us(buffer[i] * IR_SEND_MICROS_PER_TICK);
        }
        else
        {
            this->_turn_on(); // Enable timer or ledcWrite() generated PWM output
            ets_delay_us(buffer[i] * IR_SEND_MICROS_PER_TICK);
            this->_turn_off();
        }
    }
    this->_turn_off();
}

void ir_send_plugin::_turn_on()
{

    ledcWrite(this->_channel, this->_on_state); //  * 256 since we have 8 bit resolution
}

void ir_send_plugin::_turn_off()
{

    ledcWrite(this->_channel, this->_off_state);
}