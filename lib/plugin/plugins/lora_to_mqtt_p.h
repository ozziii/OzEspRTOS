/**
 *
 *      root_topic espnane/name/
 *
 *HASSIO ------------------------------------------------------------------>  SERVER  -------------------------------------------------------->    DEVICE
 *
 *   topic:    espnane/name/send/adddress/topic                                          address -> payload&topic                                    |
 *   payload:  payload                                                                                                                               |
 *                                                                                                                                                   |
 *                                                                                                                                                   V
 *
 *HASSIO <-----------------------------------------------------------------  SERVER  <-----------------------------------------------------------  DEVICE
 *
 *   topic:    topic                                                                     payload&topic
 *   payload:  payload                                                             
 *                                                                                                            
 */
#ifndef LORATOMQTT_H
#define LORATOMQTT_H

#include "plugin_base.h"

#include <e32cp.h>

#define LORATOMQTT_STR_AUX "Aux"
#define LORATOMQTT_STR_M0 "M0"
#define LORATOMQTT_STR_M1 "M1"
#define LORATOMQTT_STR_UART "Uart"
#define LORATOMQTT_STR_TX "Tx"
#define LORATOMQTT_STR_RX "Rx"

// 12/lora/0/3/14/13/21/2
/**
 *
 *     LORA TO MQTT
 *
 *     Topic espname/NAME/send/Address/Topic   =>
 *
 *     SUBSCRIBE espname/NAME/send/#
 *
 */
class lora_to_mqtt : public plugin_base
{

public:
    lora_to_mqtt(params_t init);

    static lora_to_mqtt *COTR(params_t init) { return new lora_to_mqtt(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {LORATOMQTT_STR_AUX, PLUGIN_STR_NAN},
            {LORATOMQTT_STR_M0, PLUGIN_STR_NAN},
            {LORATOMQTT_STR_M1, PLUGIN_STR_NAN},
            {LORATOMQTT_STR_UART, "-1"},
            {LORATOMQTT_STR_TX, "-1"},
            {LORATOMQTT_STR_RX, "-1"}};
    }


    void execute_lora(e32_receve_struct_t message);
    QueueHandle_t get_queue();

protected:
    void _send_response(String Topic, String Message) override;

    String _root_topic;
    String _receve_topic;
    String _espname;

    e32cp _protocoll;
    e32oz _lora;

    uint8_t
        _uart_number,
        _aux,
        _m1,
        _m0;
};

#endif