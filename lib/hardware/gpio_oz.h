#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define IOTAG "IO"

#include "map"

#include "debug_oz.h"

#define GPIOZ_MAX_INTERRUPT_SLOT 50           // NOT CHANHGE !!!
#define GPIOZ_DFAULT_PUMP_DELAY  300
#define GPIOZ_DEBOUNCING_DELAY   500


#define GPIOZ_PUMP_TASK_MEMORY_ALLOCATION     256
#define GPIOZ_PUMP_TASK_PRIORITY              3




class gpio_oz
{
  public:
    gpio_oz();

    /**
     * 
     * 
     * 
     * 
     */
    void switch_Pin(uint8_t Pin);

    /**
     * 
     * 
     * 
     * 
     */
    void pump_Pin(uint8_t Pin, unsigned long Delay);



    //#####################################   PWM ###################################################

    /**
     * 
     * 
     * 
     * 
     */
    void attach_PWM(uint8_t Pin);

    /**
     * 
     * 
     * 
     * 
     */
    void detach_PWM(uint8_t Pin);


    /**
     * 
     * 
     * 
     * 
     */
    void pwm_write(uint8_t Pin,uint value);



    //######################################   INTERRUPT  ##########################################


    /**
     * 
     * 
     * 
     * 
     */
    void pushInterrupt(uint8_t pin);


    /**
     * 
     * 
     * 
     * @return interrupt pin if ERROR occurred or Timeout  return -1 
     */
    int8_t getInterrupt();

    /**
     * 
     * 
     * 
     * 
     */
    void setInterrups(uint8_t pin, int mode);

    /**
     * 
     * 
     * 
     * 
     */
    void setTouchInterrups(uint8_t pin, uint16_t threshold);

  protected:
    QueueHandle_t queue;
    std::map<uint8_t,uint8_t> pwm_pins;
    uint8_t channel = 0;
    portMUX_TYPE innterrupt_mutex = portMUX_INITIALIZER_UNLOCKED;
};

extern gpio_oz GPIOZ;

#endif

