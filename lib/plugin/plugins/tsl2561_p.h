
#ifndef TSL2561_P_H
#define TSL2561_P_H


#include "plugins/sensor_p.h"
#include <SparkFunTSL2561.h>

#define TSL2561_STR_EXPOSION_TIME "Expposion_time"
#define TSL2561_STR_ADDRESS "i2s_address"

/**
 *            
 *     
 */
class tsl2561 : public sensor
{
public:
    tsl2561(params_t init);

    ~tsl2561();

    static tsl2561 *COTR(params_t init) { return new tsl2561(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
            {PLUGIN_BASE_STR_SDA,"21"},
            {PLUGIN_BASE_STR_SCL,"22"},
            {TSL2561_STR_EXPOSION_TIME,"500"},
            {TSL2561_STR_ADDRESS,"0"}
        };
    }
private:

    bool _sensor_begin();

    void _sensor_update() override;

    unsigned int _shutter_time;

    SFE_TSL2561 *_sensor;
};

#endif