#include "bme680_p.h"

#include "Wire.h"

bme680::bme680(params_t init) : sensor(init)
{
    this->_values.push_back(
        {MQTT_SENSOR_TEMPERATURE,
         2,
         50,
         -20,
         0});

    this->_values.push_back(
        {MQTT_SENSOR_HUMIDITY,
         2,
         99,
         1,
         0});

    this->_values.push_back(
        {MQTT_SENSOR_PRESSURE,
         2,
         5000,
         100,
         0});

    this->_values.push_back(
        {MQTT_SENSOR_IAQ,
         2,
         500,
         10,
         0});

    int sda = this->get_int_parameter(PLUGIN_BASE_STR_SDA);
    int scl = this->get_int_parameter(PLUGIN_BASE_STR_SCL);

    if (GPIO_IS_VALID_GPIO(sda) && GPIO_IS_VALID_GPIO(scl))
    {
        Wire.setPins(sda,scl);
    }

    this->_sensor = new BME680_Class();

    auto end_time = BME680_PLUGIN_SENSOR_TIMEOUT + millis();

    while (!this->_sensor->begin(I2C_STANDARD_MODE))
    {
        delay(2000);
        if (millis() > end_time)
        {
            OZ_LOGW(this->name().c_str(), "No sensor found");
            this->_initialized =false;
            return ;
        }
    }

    this->_sensor->setOversampling(TemperatureSensor, Oversample16); // Use enumerated type values
    this->_sensor->setOversampling(HumiditySensor, Oversample16);    // Use enumerated type values
    this->_sensor->setOversampling(PressureSensor, Oversample16);    // Use enumerated type values
    this->_sensor->setIIRFilter(IIR4);                               // Use enumerated type values
    this->_sensor->setGas(320, 150);                                 // 320°C for 150 milliseconds

    OZ_LOGI(this->name().c_str(), " Initialization ok!  Delay: %u ms",this->sensor_delay());
    this->_initialized = true;

}

bme680::~bme680()
{
    delete this->_sensor;
}

void bme680::_sensor_update()
{
    int32_t tempeature, humidity, pressure, gas;

    this->_sensor->getSensorData(tempeature, humidity, pressure, gas);

    this->_values[0].value = ((float)tempeature) / 100;
    this->_values[1].value = ((float)humidity) / 1000;
    this->_values[2].value = ((float)pressure) / 100;
    this->_values[3].value = ((float)gas) / 100;
}