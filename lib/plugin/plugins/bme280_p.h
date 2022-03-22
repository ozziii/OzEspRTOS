#ifndef BME280_P_H
#define BME280_P_H

#include "plugins/sensor_p.h"

#include "Wire.h"

#include <BME280_t.h>




/****************************************************************
*            
*     BME280 TEMPERATURE, PRESSURE AND HUMIDITY SENSOR 
*
***************************************************************/
class bme280 : public sensor
{
public:
    bme280(params_t init);

    ~bme280();

    static bme280 *COTR(params_t init) { return new bme280(init); }


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
    void _sensor_update() override;

    BME280<> *_sensor;
};

#endif