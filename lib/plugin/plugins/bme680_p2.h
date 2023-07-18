
#ifndef BME680_P2_H
#define BME680_P2_H

#include "bsec.h"
#include "plugins/sensor_p.h"

#define BME680_STR_TEMP_OFFSET "Temp_Offset"

/**
 *            
 *     BME680 TEMPERATURE, PRESSURE AND HUMIDITY SENSOR USIN BSEC LIB
 *
 */
class bme6802 : public sensor
{
public:
    bme6802(params_t init);

    ~bme6802();

    static bme6802 *COTR(params_t init) { return new bme6802(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
            {PLUGIN_BASE_STR_SDA,"21"},
            {PLUGIN_BASE_STR_SCL,"22"},
            {BME680_STR_TEMP_OFFSET,"0"},
        };
    }

    void _fill_value();

private:

    bool _checkStatus();

    void _sensor_update() override;

    Bsec  *_sensor;
};

#endif
