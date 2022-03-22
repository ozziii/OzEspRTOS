
#include "test_plugin.h"
test_plugin::test_plugin(params_t init) : plugin_base(init)
{
    OZ_LOGI(this->name().c_str(), "Configuration complete");

    this->_initialized = true;
}

void test_plugin::_execute_sensor() 
{
    OZ_LOGI(this->name().c_str(), "Begin Sensor");
    delay(1000);
    OZ_LOGI(this->name().c_str(), "End Sensor");
}

void test_plugin::_force_update() 
{
    OZ_LOGI(this->name().c_str(), "Begin Update");
    delay(1000);
    OZ_LOGI(this->name().c_str(), "End Update");
}
