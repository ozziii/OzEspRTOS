#include <gpio_oz.h>

#include <setting.h>

static uint32_t gpioz_debouncing_delay = 0;



volatile static unsigned long last_trigger_pin00 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_00()
{
    if (millis() > last_trigger_pin00 + gpioz_debouncing_delay)
    {
        last_trigger_pin00 = millis();
        GPIOZ.pushInterrupt(0);
    }
}
volatile static unsigned long last_trigger_pin01 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_01()
{
    if (millis() > last_trigger_pin01 + gpioz_debouncing_delay)
    {
        last_trigger_pin01 = millis();
        GPIOZ.pushInterrupt(1);
    }
}
volatile static unsigned long last_trigger_pin02 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_02()
{
    if (millis() > last_trigger_pin02 + gpioz_debouncing_delay)
    {
        last_trigger_pin02 = millis();
        GPIOZ.pushInterrupt(2);
    }
}
volatile static unsigned long last_trigger_pin03 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_03()
{
    if (millis() > last_trigger_pin03 + gpioz_debouncing_delay)
    {
        last_trigger_pin03 = millis();
        GPIOZ.pushInterrupt(3);
    }
}
volatile static unsigned long last_trigger_pin04 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_04()
{
    if (millis() > last_trigger_pin04 + gpioz_debouncing_delay)
    {
        last_trigger_pin04 = millis();
        GPIOZ.pushInterrupt(4);
    }
}
volatile static unsigned long last_trigger_pin05 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_05()
{
    if (millis() > last_trigger_pin05 + gpioz_debouncing_delay)
    {
        last_trigger_pin05 = millis();
        GPIOZ.pushInterrupt(5);
    }
}
volatile static unsigned long last_trigger_pin06 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_06()
{
    if (millis() > last_trigger_pin06 + gpioz_debouncing_delay)
    {
        last_trigger_pin06 = millis();
        GPIOZ.pushInterrupt(6);
    }
}
volatile static unsigned long last_trigger_pin07 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_07()
{
    if (millis() > last_trigger_pin07 + gpioz_debouncing_delay)
    {
        last_trigger_pin07 = millis();
        GPIOZ.pushInterrupt(7);
    }
}
volatile static unsigned long last_trigger_pin08 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_08()
{
    if (millis() > last_trigger_pin08 + gpioz_debouncing_delay)
    {
        last_trigger_pin08 = millis();
        GPIOZ.pushInterrupt(8);
    }
}
volatile static unsigned long last_trigger_pin09 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_09()
{
    if (millis() > last_trigger_pin09 + gpioz_debouncing_delay)
    {
        last_trigger_pin09 = millis();
        GPIOZ.pushInterrupt(9);
    }
}
volatile static unsigned long last_trigger_pin10 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_10()
{
    if (millis() > last_trigger_pin10 + gpioz_debouncing_delay)
    {
        last_trigger_pin10 = millis();
        GPIOZ.pushInterrupt(10);
    }
}
volatile static unsigned long last_trigger_pin11 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_11()
{
    if (millis() > last_trigger_pin11 + gpioz_debouncing_delay)
    {
        last_trigger_pin11 = millis();
        GPIOZ.pushInterrupt(11);
    }
}
volatile static unsigned long last_trigger_pin12 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_12()
{
    if (millis() > last_trigger_pin12 + gpioz_debouncing_delay)
    {
        last_trigger_pin12 = millis();
        GPIOZ.pushInterrupt(12);
    }
}
volatile static unsigned long last_trigger_pin13 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_13()
{
    if (millis() > last_trigger_pin13 + gpioz_debouncing_delay)
    {
        last_trigger_pin13 = millis();
        GPIOZ.pushInterrupt(13);
    }
}
volatile static unsigned long last_trigger_pin14 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_14()
{
    if (millis() > last_trigger_pin14 + gpioz_debouncing_delay)
    {
        last_trigger_pin14 = millis();
        GPIOZ.pushInterrupt(14);
    }
}
volatile static unsigned long last_trigger_pin15 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_15()
{
    if (millis() > last_trigger_pin15 + gpioz_debouncing_delay)
    {
        last_trigger_pin15 = millis();
        GPIOZ.pushInterrupt(15);
    }
}
volatile static unsigned long last_trigger_pin16 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_16()
{
    if (millis() > last_trigger_pin16 + gpioz_debouncing_delay)
    {
        last_trigger_pin16 = millis();
        GPIOZ.pushInterrupt(16);
    }
}
volatile static unsigned long last_trigger_pin17 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_17()
{
    if (millis() > last_trigger_pin17 + gpioz_debouncing_delay)
    {
        last_trigger_pin17 = millis();
        GPIOZ.pushInterrupt(17);
    }
}
volatile static unsigned long last_trigger_pin18 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_18()
{
    if (millis() > last_trigger_pin18 + gpioz_debouncing_delay)
    {
        last_trigger_pin18 = millis();
        GPIOZ.pushInterrupt(18);
    }
}
volatile static unsigned long last_trigger_pin19 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_19()
{
    if (millis() > last_trigger_pin19 + gpioz_debouncing_delay)
    {
        last_trigger_pin19 = millis();
        GPIOZ.pushInterrupt(19);
    }
}
volatile static unsigned long last_trigger_pin21 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_21()
{
    if (millis() > last_trigger_pin21 + gpioz_debouncing_delay)
    {
        last_trigger_pin21 = millis();
        GPIOZ.pushInterrupt(21);
    }
}
volatile static unsigned long last_trigger_pin22 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_22()
{
    if (millis() > last_trigger_pin22 + gpioz_debouncing_delay)
    {
        last_trigger_pin22 = millis();
        GPIOZ.pushInterrupt(22);
    }
}
volatile static unsigned long last_trigger_pin23 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_23()
{
    if (millis() > last_trigger_pin23 + gpioz_debouncing_delay)
    {
        last_trigger_pin23 = millis();
        GPIOZ.pushInterrupt(23);
    }
}
volatile static unsigned long last_trigger_pin25 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_25()
{
    if (millis() > last_trigger_pin25 + gpioz_debouncing_delay)
    {
        last_trigger_pin25 = millis();
        GPIOZ.pushInterrupt(25);
    }
}
volatile static unsigned long last_trigger_pin26 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_26()
{
    if (millis() > last_trigger_pin26 + gpioz_debouncing_delay)
    {
        last_trigger_pin26 = millis();
        GPIOZ.pushInterrupt(26);
    }
}
volatile static unsigned long last_trigger_pin27 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_27()
{
    if (millis() > last_trigger_pin27 + gpioz_debouncing_delay)
    {
        last_trigger_pin27 = millis();
        GPIOZ.pushInterrupt(27);
    }
}
volatile static unsigned long last_trigger_pin32 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_32()
{
    if (millis() > last_trigger_pin32 + gpioz_debouncing_delay)
    {
        last_trigger_pin32 = millis();
        GPIOZ.pushInterrupt(32);
    }
}
volatile static unsigned long last_trigger_pin33 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_33()
{
    if (millis() > last_trigger_pin33 + gpioz_debouncing_delay)
    {
        last_trigger_pin33 = millis();
        GPIOZ.pushInterrupt(33);
    }
}
volatile static unsigned long last_trigger_pin34 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_34()
{
    if (millis() > last_trigger_pin34 + gpioz_debouncing_delay)
    {
        last_trigger_pin34 = millis();
        GPIOZ.pushInterrupt(34);
    }
}
volatile static unsigned long last_trigger_pin35 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_35()
{
    if (millis() > last_trigger_pin35 + gpioz_debouncing_delay)
    {
        last_trigger_pin35 = millis();
        GPIOZ.pushInterrupt(35);
    }
}
volatile static unsigned long last_trigger_pin36 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_36()
{
    if (millis() > last_trigger_pin36 + gpioz_debouncing_delay)
    {
        last_trigger_pin36 = millis();
        GPIOZ.pushInterrupt(36);
    }
}
volatile static unsigned long last_trigger_pin37 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_37()
{
    if (millis() > last_trigger_pin37 + gpioz_debouncing_delay)
    {
        last_trigger_pin37 = millis();
        GPIOZ.pushInterrupt(37);
    }
}
volatile static unsigned long last_trigger_pin38 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_38()
{
    if (millis() > last_trigger_pin38 + gpioz_debouncing_delay)
    {
        last_trigger_pin38 = millis();
        GPIOZ.pushInterrupt(38);
    }
}
volatile static unsigned long last_trigger_pin39 = 0;
static void ICACHE_RAM_ATTR OnInterrupt_39()
{
    if (millis() > last_trigger_pin39 + gpioz_debouncing_delay)
    {
        last_trigger_pin39 = millis();
        GPIOZ.pushInterrupt(39);
    }
}

struct pump_reset_task_parameters
{
    uint8_t pin;
    unsigned long delay;
};

static void pump_reset_task(void *Parameter)
{
    auto parameter = reinterpret_cast<pump_reset_task_parameters *>(Parameter);
    vTaskDelay(pdMS_TO_TICKS(parameter->delay));
    GPIOZ.switch_Pin(parameter->pin);
}

gpio_oz::gpio_oz()
{
    queue = xQueueCreate(GPIOZ_MAX_INTERRUPT_SLOT, sizeof(uint8_t));
}

void gpio_oz::switch_Pin(uint8_t Pin)
{
    bool value = digitalRead(Pin);
    digitalWrite(Pin, !value);
}

void gpio_oz::pump_Pin(uint8_t Pin, unsigned long Delay)
{
    pump_reset_task_parameters newParameter = {Pin, Delay};

    this->switch_Pin(Pin);

    xTaskCreate(
        pump_reset_task,                   // Function that should be called
        "pumpTask",                        // Name of the task (for debugging)
        GPIOZ_PUMP_TASK_MEMORY_ALLOCATION, // Stack size (bytes)
        &newParameter,                     // Parameter to pass
        GPIOZ_PUMP_TASK_PRIORITY,          // Task priority
        NULL                               // Task handle
    );
}

void gpio_oz::attach_PWM(uint8_t Pin)
{
    auto it = pwm_pins.find(Pin);
    if (it != pwm_pins.end())
    {
        return;
    }

    int channel = -1;
    for (uint8_t i = 0; i < PWM_MAX_CHANNEL; i++)
    {
        bool cahnnel_used = false;

        if (pwm_pins.size() > 0)
        {
            for (auto it : pwm_pins)
            {
                if (it.second == i)
                {
                    cahnnel_used = true;
                    break;
                }
            }
        }

        if (!cahnnel_used)
        {
            channel = i;
            break;
        }
    }

    if (channel >= 0)
    {
        pwm_pins.insert({Pin, channel});
        ledcSetup(channel, PWM_FREQUENCY, PWM_RESOLUTION);
        ledcAttachPin(Pin, channel);
    }
    else
    {
        OZ_LOGE(IOTAG, "All pwm channels occupied");
    }
}

void gpio_oz::detach_PWM(uint8_t Pin)
{
    auto it = pwm_pins.find(Pin);
    if (it != pwm_pins.end())
    {
        pwm_pins.erase(it);
        ledcDetachPin(Pin);
    }
}

void gpio_oz::pwm_write(uint8_t Pin, uint value)
{
    auto it = pwm_pins.find(Pin);
    if (it != pwm_pins.end())
    {
        ledcWrite(it->second, value);
    }
}

void gpio_oz::pushInterrupt(uint8_t pin)
{
    portENTER_CRITICAL_ISR(&innterrupt_mutex);
    xQueueSendFromISR(queue, &pin, pdFALSE);
    portEXIT_CRITICAL_ISR(&innterrupt_mutex);
}

int8_t gpio_oz::getInterrupt()
{
    uint8_t pin;
    if (xQueueGenericReceive(queue, &pin, portMAX_DELAY, pdFALSE))
        return pin;
    else
        return -1;
}

void gpio_oz::setInterrups(uint8_t pin, int mode)
{
    if(gpioz_debouncing_delay==0)
        gpioz_debouncing_delay = SETTING_I(DB_SETTING_DEBOU);

    switch (pin)
    {
    case GPIO_NUM_0:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_00, mode);
        break;
    case GPIO_NUM_1:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_01, mode);
        break;
    case GPIO_NUM_2:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_02, mode);
        break;
    case GPIO_NUM_3:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_03, mode);
        break;
    case GPIO_NUM_4:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_04, mode);
        break;
    case GPIO_NUM_5:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_05, mode);
        break;
    case GPIO_NUM_6:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_06, mode);
        break;
    case GPIO_NUM_7:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_07, mode);
        break;
    case GPIO_NUM_8:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_08, mode);
        break;
    case GPIO_NUM_9:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_09, mode);
        break;
    case GPIO_NUM_10:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_10, mode);
        break;
    case GPIO_NUM_11:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_11, mode);
        break;
    case GPIO_NUM_12:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_12, mode);
        break;
    case GPIO_NUM_13:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_13, mode);
        break;
    case GPIO_NUM_14:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_14, mode);
        break;
    case GPIO_NUM_15:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_15, mode);
        break;
    case GPIO_NUM_16:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_16, mode);
        break;
    case GPIO_NUM_17:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_17, mode);
        break;
    case GPIO_NUM_18:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_18, mode);
        break;
    case GPIO_NUM_19:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_19, mode);
        break;
    case GPIO_NUM_21:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_21, mode);
        break;
    case GPIO_NUM_22:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_22, mode);
        break;
    case GPIO_NUM_23:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_23, mode);
        break;
    case GPIO_NUM_25:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_25, mode);
        break;
    case GPIO_NUM_26:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_26, mode);
        break;
    case GPIO_NUM_27:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_27, mode);
        break;
    case GPIO_NUM_32:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_32, mode);
        break;
    case GPIO_NUM_33:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_33, mode);
        break;
    case GPIO_NUM_34:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_34, mode);
        break;
    case GPIO_NUM_35:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_35, mode);
        break;
    case GPIO_NUM_36:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_36, mode);
        break;
    case GPIO_NUM_37:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_37, mode);
        break;
    case GPIO_NUM_38:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_38, mode);
        break;
    case GPIO_NUM_39:
        attachInterrupt(digitalPinToInterrupt(pin), OnInterrupt_39, mode);
        break;
    default:
        OZ_LOGW(IOTAG, "Pin %u is not valid fot interrupt", pin);
        break;
    }
}

void gpio_oz::setTouchInterrups(uint8_t pin, uint16_t threshold)
{
    switch (pin)
    {
    case T0:
        touchAttachInterrupt(pin, OnInterrupt_04, threshold);
        break;
    case T1:
        touchAttachInterrupt(pin, OnInterrupt_00, threshold);
        break;
    case T2:
        touchAttachInterrupt(pin, OnInterrupt_02, threshold);
        break;
    case T3:
        touchAttachInterrupt(pin, OnInterrupt_15, threshold);
        break;
    case T4:
        touchAttachInterrupt(pin, OnInterrupt_13, threshold);
        break;
    case T5:
        touchAttachInterrupt(pin, OnInterrupt_12, threshold);
        break;
    case T6:
        touchAttachInterrupt(pin, OnInterrupt_14, threshold);
        break;
    case T7:
        touchAttachInterrupt(pin, OnInterrupt_27, threshold);
        break;
    case T8:
        touchAttachInterrupt(pin, OnInterrupt_33, threshold);
        break;
    case T9:
        touchAttachInterrupt(pin, OnInterrupt_32, threshold);
        break;
    default:
        OZ_LOGW(IOTAG, "Pin %u is not valid fot touch", pin);
        break;
    }
}

gpio_oz GPIOZ;