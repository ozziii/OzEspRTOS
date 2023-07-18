#include "tsl2561_p.h"

#include "Wire.h"


tsl2561::tsl2561(params_t init) : sensor(init)
{
    this->_initialized = false;

    this->_values.push_back(
        {MQTT_SENSOR_LUMINOSITY,
         1,
         40000,
         -1,
         0});

    if (!i2cIsInit(0))
    {
        int sda = this->get_int_parameter(PLUGIN_BASE_STR_SDA);
        int scl = this->get_int_parameter(PLUGIN_BASE_STR_SCL);

        if (GPIO_IS_VALID_GPIO(sda) && GPIO_IS_VALID_GPIO(scl))
            Wire.setPins(sda, scl);

        if (Wire.begin() == false)
        {
            OZ_LOGE(this->name().c_str(), "Wire not begin");
            return;
        }
    }


    char address = this->get_int_parameter(TSL2561_STR_ADDRESS);
    
    this->_sensor = new SFE_TSL2561();

    if(address == 0)
        this->_sensor->begin();
    else
        this->_sensor->begin(address);

    delay(100);

    unsigned char ID;

    if (!this->_sensor->getID(ID))
    {
        byte error = this->_sensor->getError();
        OZ_LOGE(this->name().c_str(), " Initialization Error: [%u] ", error);
        delete this->_sensor;
        return;
    }

    unsigned int ms;

    if (!this->_sensor->setTiming(0, 3, ms))
    {
        byte error = this->_sensor->getError();
        OZ_LOGE(this->name().c_str(), " Initialization Error: [%u] ", error);
        delete this->_sensor;
        return;
    }

    if (!this->_sensor->setPowerUp())
    {
        byte error = this->_sensor->getError();
        OZ_LOGE(this->name().c_str(), " Initialization Error: [%u] ", error);
        delete this->_sensor;
        return;
    }


    this->_shutter_time = this->get_int_parameter(TSL2561_STR_EXPOSION_TIME);
    if(this->_shutter_time >= 0 )
        this->_shutter_time = 500;

    OZ_LOGI(this->name().c_str(), " Initialization ok!  Delay: %u ms , ID:%u", this->sensor_delay(), ID);
    this->_initialized = true;
}

tsl2561::~tsl2561()
{
    delete this->_sensor;
}

void tsl2561::_sensor_update()
{

    this->_sensor->manualStart();
    delay(this->_shutter_time);
    this->_sensor->manualStop();

    unsigned int data0, data1;

    if (!this->_sensor->getData(data0, data1))
    {
        byte error = this->_sensor->getError();
        OZ_LOGW(this->name().c_str(), " Read Error [%u] ", error);
        this->_values[0].value = -1;
        return;
    }

    double lux;

    if (!this->_sensor->getLux(0, this->_shutter_time, data0, data1, lux))
    {
        byte error = this->_sensor->getError();
        OZ_LOGW(this->name().c_str(), " Get LUX Error [%u] ", error);
        this->_values[0].value = -1;
        return;
    }

    this->_values[0].value = ((float)lux);
}
