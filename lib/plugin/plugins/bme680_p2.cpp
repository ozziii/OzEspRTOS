#include "bme680_p2.h"

#include "Wire.h"

uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

void bme6802_update_sensor_task(void *Parameter)
{
    auto *self = reinterpret_cast<bme6802 *>(Parameter);
    while (true)
    {
        self->_fill_value();
        delay(3000);
    }

    vTaskDelete(NULL);
}

bme6802::bme6802(params_t init) : sensor(init)
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
        Wire.begin(sda, scl);
    }
    else
    {
        Wire.begin();
    }

    this->_sensor = new Bsec();
    this->_sensor->begin(BME680_I2C_ADDR_SECONDARY, Wire);

    this->_checkStatus();
    this->_sensor->setConfig(bsec_config_iaq);

    bsec_virtual_sensor_t sensorList1[3] = {
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ
    };

    this->_sensor->updateSubscription(sensorList1, 3, BSEC_SAMPLE_RATE_ULP);


    bsec_virtual_sensor_t sensorList2[5] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    this->_sensor->updateSubscription(sensorList2, 5, BSEC_SAMPLE_RATE_LP);


    if (!this->_checkStatus())
        return;

    OZ_LOGI(this->name().c_str(), " Initialization ok!  Delay: %u ms", this->sensor_delay());
    this->_initialized = true;

    xTaskCreate(
        bme6802_update_sensor_task,   // Function that should be called
        "bme6802_update_sensor_task", // Name of the task (for debugging)
        4096,                         // Stack size (bytes)
        this,                         // Parameter to pass
        0,                            // Task priority
        NULL                          // Task handle
    );
}

bme6802::~bme6802()
{
    delete this->_sensor;
}

void bme6802::_sensor_update()
{
}

bool bme6802::_checkStatus()
{
    if (this->_sensor->status != BSEC_OK)
    {
        if (this->_sensor->status < BSEC_OK)
        {
            OZ_LOGE(this->name().c_str(), "BSEC error code : %d", this->_sensor->status);
            return false;
        }
        else
        {
            OZ_LOGW(this->name().c_str(), "BSEC warning code : %d", this->_sensor->status);
        }
    }

    if (this->_sensor->bme680Status != BME680_OK)
    {
        if (this->_sensor->bme680Status < BME680_OK)
        {
            OZ_LOGE(this->name().c_str(), "BME680 error code : %d", this->_sensor->bme680Status);
            return false;
        }
        else
        {
            OZ_LOGW(this->name().c_str(), "BME680 warning code : %d", this->_sensor->bme680Status);
        }
    }

    return true;
}

void bme6802::_fill_value()
{
    if (this->_sensor->run())
    {
        this->_values[0].value = this->_sensor->temperature;
        this->_values[1].value = this->_sensor->humidity;
        this->_values[2].value = this->_sensor->pressure;
        this->_values[3].value = this->_sensor->staticIaq;
    }
}