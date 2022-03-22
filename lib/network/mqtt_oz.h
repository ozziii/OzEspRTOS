#ifndef MQTT_OZ_H
#define MQTT_OZ_H

#include <i_mqtt.h>

/**
 *
 *
 *
 */
class mqtt_idf : public i_mqtt
{
public:
    /**
     *
     *
     *
     *
     */
    void begin(OnMessageCallback MessageCallback, OnConnectionCallback ConnectionCallback);

    /**
     *
     *
     *
     *
     */
    void send(String Topic, String Message, uint8_t QoS = 0, bool Retain = false);

    /**
     *
     *
     *
     *
     */
    void subscribe(String Topic, uint8_t QoS = 0);

    /**
     * @brief 
     *
     * 
     * @param Topics 
     */
    void subscribe(std::vector<String> Topics);

    /**
     * @brief 
     * 
     * 
     * 
     * @return true 
     * @return false 
     */
    bool is_connected();

private:
    String lastWillTopic();

    String _address,_client_id,_username,_password,_lwt_topic;

    uint32_t _port;
};


extern mqtt_idf OZMQTT;

#endif