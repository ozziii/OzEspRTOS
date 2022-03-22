#ifndef I_MQTT_H
#define I_MQTT_H

#include <WString.h>
#include <functional>
#include <vector>


typedef std::function<void(String topic, String message)> OnMessageCallback;
typedef std::function<void()> OnConnectionCallback;

/**
 * wifi_o.h
 * 
 * 
 * 
 */
class i_mqtt
{
public:
    /**
     * 
     * 
     * 
     * 
     */
    virtual void begin(OnMessageCallback MessageCallback,OnConnectionCallback ConnectionCallback) = 0;

    /**
     * 
     * 
     * 
     * 
     */
    virtual void send(String Topic,String Message,uint8_t QoS = 0,bool Retain = false) = 0;

    /**
     * 
     * 
     * 
     * 
     */
    virtual void subscribe(String Topic,uint8_t QoS = 0) = 0;

    /**
     * 
     * 
     * 
     * 
     */
    virtual void subscribe(std::vector<String> Topics) = 0;
};
#endif