#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include <map>

#include <configuration.h>
#include <setting.h>
#include <mqtt_oz.h>
#include <debug_oz.h>
#include <gpio_oz.h>
#include <utils.h>


#define params_t std::map<String,String>

enum plugin_type
{
  TYPE_MQTT = 1,
  TYPE_SENSOR = 2,
  TYPE_INTERRUPT = 4,
};



#define PLUGIN_BASE_STR_NAME "Name"
#define PLUGIN_BASE_STR_DELAY "Delay"
#define PLUGIN_BASE_STR_PIN "Pin"
#define PLUGIN_BASE_STR_LOGIC "Logic"
#define PLUGIN_BASE_STR_PULL "Pull"
#define PLUGIN_BASE_STR_INTERRUPT "Interrupt"
#define PLUGIN_BASE_STR_DEFSTATE "Start_State"
#define PLUGIN_BASE_STR_SDA "sda"
#define PLUGIN_BASE_STR_SCL "scl"
#define PLUGIN_BASE_STR_CLASS "device_class"
#define PLUGIN_BASE_STR_UART "urat_number"


#define PLUGIN_BASE_VAL_PULLUP   "up"
#define PLUGIN_BASE_VAL_PULLDOWN "down"

/**
 * 
 * 
 * 
 * 
 * 
 * 
 */ 
class plugin_base
{
public:
    /**
     * 
     * 
     */
    plugin_base(params_t Parameters);

    /**
     * 
     * 
     */
    bool is_initialized();

    /**
     * 
     * 
     */
    void interrupt(uint8_t pin);

    /**
     * 
     * 
     */
    void send_response(String topic, String message);

    /**
     *  ISR function
     * 
     */
    void execute_sensor();

    /**
     * 
     * 
     */
    void force_update();

    /**
     * 
     * 
     */
    virtual unsigned long sensor_delay();


    /**
     * 
     * 
     */
    String name();


    /**
     * 
     * 
     */
    virtual uint8_t getType();

    /**
     * 
     * 
     */
    std::vector<String> get_mqtt_topic();

    /**
     * 
     * 
     */
    std::vector<uint8_t> get_interrupt_pins();


protected:

    /**
     * 
     * 
     */
    String  get_string_parameter(String parameter_name);

    /**
     * 
     * 
     */
    int  get_int_parameter(String parameter_name);


    /**
     * 
     * 
     */
    float  get_float_parameter(String parameter_name);

    /**
     * 
     * 
     */
    unsigned long get_long_parameter(String parameter_name);


    /**
     *  Virtual void function
     * 
     */
    virtual void _interrupt(uint8_t pin) {}

    /**
     * Virtual void function
     * 
     */
    virtual void _send_response(String action, String message) {}

    /**
     * Virtual void function
     * 
     */
    virtual void _execute_sensor() {}

    /**
     * Virtual void function
     * 
     */
    virtual void _force_update();

    /**
     *  Getate topic from espname ad plugin name 
     *  Like  espname/pluginName/action
     * 
     */
    String buildTopic(String action);

    /**
     * 
     * 
     */
    void subscribe_topic(String topic);

    /**
     * 
     * 
     */
    void attach_interrupt(uint8_t pin,uint8_t mode);


    /**
     * 
     * 
     */
    void attach_touch(uint8_t Pin, uint16_t Threshold);

    /**
     * 
     * 
     */
    static uint8_t gpioInputMode(const char * mode);



    bool _initialized = false;
    params_t _parameters;

private:
    SemaphoreHandle_t mutex;
    std::vector<String> _mqtt_topics;
    std::vector<uint8_t> _interrups_pin;
    
};



typedef std::function<plugin_base*(params_t init )> plugin_base_ctor;

#endif