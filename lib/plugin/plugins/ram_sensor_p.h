#ifndef RAM_P_H
#define RAM_P_H

#include "plugins/sensor_p.h"

#define RAM_SENSOR_VALUE

/****************************************************************
*            
*     FREE RAM SENSOR FOR DEBUGGING
*
***************************************************************/
class freeram : public sensor
{
public:
    freeram(params_t init);

    static freeram *COTR(params_t init) { return new freeram(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {PLUGIN_BASE_STR_DELAY,"0"},
        };
    }

private:
    void _sensor_update() override;
};

#endif