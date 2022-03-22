#ifndef PZEM004T_P_H
#define PZEM004T_P_H

#include <PZEM004Tv30.h>
#include <PZEM004T.h>

#include "plugins/sensor_p.h"


#define PZEM004T_STR_ADDRESS "Address"
#define PZEM004T_STR_VERSION "Version"
#define PZEM004T_STR_SERIAL  "Uart"
#define PZEM004T_STR_TX "Tx"
#define PZEM004T_STR_RX "Rx"
#define PZEM004T_STR_SETADDRESS "Set_Address"

/**
 *
 *     PZEM-004T   Main voltage, current, power, energy, cosfi sensor
 *
 */
class pzem004t : public sensor
{
public:
    pzem004t(params_t init);

    ~pzem004t();

    static pzem004t *COTR(params_t init) { return new pzem004t(init); }

    static params_t PARAM()
    {
        return params_t{
            {PLUGIN_BASE_STR_NAME, PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY, "0"},
            {PZEM004T_STR_ADDRESS, "1"},
            {PZEM004T_STR_VERSION, "1"},
            {PZEM004T_STR_SERIAL , "2"},
            {PZEM004T_STR_TX , "-1"},
            {PZEM004T_STR_RX , "-1"},
            {PZEM004T_STR_SETADDRESS , "0"},
            
        };
    }

private:
    void _sensor_update() override;

    HardwareSerial *_serial = nullptr;
    PZEM004Tv30 *_sensor_v3 = nullptr;
    PZEM004T *_sensor = nullptr;
    IPAddress *ip = nullptr;
    uint8_t _version;
};

#endif