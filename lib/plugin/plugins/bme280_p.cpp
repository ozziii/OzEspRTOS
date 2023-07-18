
#include "bme280_p.h"

#include "Wire.h"

bme280::bme280(params_t init) : sensor(init)
{
    this->_values.push_back(
        {MQTT_SENSOR_TEMPERATURE, // Topic
         2,                       // decimal
         50,                      // max
         -20,                     // min
         0});                     // value

    this->_values.push_back(
        {MQTT_SENSOR_HUMIDITY,
         2,
         120,
         1,
         0});

    this->_values.push_back(
        {MQTT_SENSOR_PRESSURE,
         2,
         1200,
         10,
         0});

    this->_sensor = new BME280<>();

    if (!i2cIsInit(0))
    {
        uint16_t sda = this->get_int_parameter(PLUGIN_BASE_STR_SDA);
        uint16_t scl = this->get_int_parameter(PLUGIN_BASE_STR_SCL);

        if (GPIO_IS_VALID_GPIO(sda) && GPIO_IS_VALID_GPIO(scl))
        {
            Wire.begin(sda, scl);
        }
        else
        {
            Wire.begin();
        }
    }
    
    if (!this->_sensor->begin())
    {
        OZ_LOGW(this->name().c_str(), "No sensor found");
        this->_initialized = false;
        return;
    }

    OZ_LOGI(this->name().c_str(), "Initialization ok!");
    this->_initialized = true;
}

bme280::~bme280()
{
    delete this->_sensor;
}

void bme280::_sensor_update()
{
    this->_sensor->refresh();
    this->_values[0].value = this->_sensor->temperature;
    this->_values[1].value = this->_sensor->humidity;
    this->_values[2].value = this->_sensor->pressure / 100.0F;
}
