#ifndef TEST_PLUGIN_H
#define TEST_PLUGIN_H

#include "plugin_base.h"

class test_plugin : public plugin_base
{
public:
    test_plugin(params_t init);
  
    void _execute_sensor() override;

    void _force_update() override;

    static test_plugin * COTR(params_t  init) { return new test_plugin(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_PIN,PLUGIN_STR_NAN},
        };
    }

};

#endif