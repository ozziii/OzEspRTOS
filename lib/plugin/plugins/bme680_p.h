#ifndef BME680_P_H
#define BME680_P_H

#include "Zanshin_BME680.h"

#include "plugins/sensor_p.h"


/**
 *            
 *     BME680 TEMPERATURE, PRESSURE AND HUMIDITY SENSOR  USING Zanshin Lib
 */
class bme680 : public sensor
{
public:
    bme680(params_t init);

    ~bme680();

    static bme680 *COTR(params_t init) { return new bme680(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
            {PLUGIN_BASE_STR_SDA,"21"},
            {PLUGIN_BASE_STR_SCL,"22"},
        };
    }
private:

    bool _sensor_begin();

    void _sensor_update() override;

    BME680_Class *_sensor;
};

#endif