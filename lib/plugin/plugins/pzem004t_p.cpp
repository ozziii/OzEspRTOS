#include "pzem004t_p.h"

// 0   1       2       3       4            5             6             7
//#/[NAME]/[delay]/[TX-RX]/[Address]=1/[Serial]=2/[Version 1:3]=1/[Set address]=0
pzem004t::pzem004t(params_t init) : sensor(init)
{
    this->_values.push_back( // 0
        {MQTT_SENSOR_VOLTAGE,
         2,
         1000,
         0,
         0});

    this->_values.push_back( // 1
        {MQTT_SENSOR_CURRENT,
         2,
         200,
         0,
         0});

    this->_values.push_back( // 2
        {MQTT_SENSOR_POWER,
         2,
         10000,
         0,
         0});

    this->_values.push_back( // 3
        {MQTT_SENSOR_ENERGY,
         2,
         10.0e+020,
         0,
         0});

    uint8_t tx_pin = this->get_int_parameter(PZEM004T_STR_TX);
    uint8_t rx_pin = this->get_int_parameter(PZEM004T_STR_RX);
    

    uint8_t sensor_address = this->get_int_parameter(PZEM004T_STR_ADDRESS);
    if (sensor_address == 0)
        sensor_address = 1;

    uint8_t serial_address = this->get_int_parameter(PZEM004T_STR_SERIAL);
    if (serial_address >= 3)
    {
        serial_address = 2;
    }

    this->_serial = new HardwareSerial(serial_address);

    this->_version = this->get_int_parameter(PZEM004T_STR_VERSION);

    if (this->_version == 3)
    {
        this->_values.push_back( // 4
            {MQTT_SENSOR_POWER_FACTOR,
             2,
             1,
             0,
             0});
    }

    bool setAddress = this->get_int_parameter(PZEM004T_STR_SETADDRESS);


    switch (this->_version)
    {
    case 1:
    {
        this->_sensor = new PZEM004T(this->_serial, rx_pin, tx_pin);
        this->ip = new IPAddress(192, 168, 1, sensor_address);

        uint8_t try_time = 0;

        while (!this->_sensor->setAddress(*ip) && try_time < 10)
        {
            try_time++;
            delay(500);
        }

        if (try_time == 10)
        {
            OZ_LOGW(this->name().c_str(), "Device not found");
            return;
        }
    }
    break;

    case 3:
    {
        if (setAddress)
        {
            this->_sensor_v3 = new PZEM004Tv30(*this->_serial, rx_pin, tx_pin);

            if (this->_sensor_v3->setAddress(sensor_address))
            {
                OZ_LOGI(this->name().c_str(), "Ok address set %u", sensor_address);
            }
            else
            {
                OZ_LOGW(this->name().c_str(), "Can't set address %u", sensor_address);
            }
        }
        else
        {
            this->_sensor_v3 = new PZEM004Tv30(*this->_serial, rx_pin, tx_pin, sensor_address);
        }
    }
    break;

    default:
        OZ_LOGW(this->name().c_str(), "No version [%u] exist", this->_version);
        return;
        break;
    }

    this->_initialized = true;
}

pzem004t::~pzem004t()
{
    delete this->_serial;
    delete this->_sensor_v3;
    delete this->_sensor;
    delete this->ip;
}

void pzem004t::_sensor_update()
{
    switch (this->_version)
    {
    case 1:
    {
        this->_values[0].value = this->_sensor->voltage(*ip);
        this->_values[1].value = this->_sensor->current(*ip);
        this->_values[2].value = this->_sensor->power(*ip);
        this->_values[3].value = this->_sensor->energy(*ip);
    }
    break;

    case 3:
    {
        this->_values[0].value = this->_sensor_v3->voltage();
        this->_values[1].value = this->_sensor_v3->current();
        this->_values[2].value = this->_sensor_v3->power();
        this->_values[3].value = this->_sensor_v3->energy();
        this->_values[4].value = this->_sensor_v3->pf();
    }
    break;

    default:
        break;
    }
}