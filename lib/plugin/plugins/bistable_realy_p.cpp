#include "bistable_realy_p.h"

bistable_relay::bistable_relay(params_t init) : switch_p(init)
{

}

void bistable_relay::_io_action(uint8_t state) 
{
    GPIOZ.pump_Pin(this->_action_pin,BISTABLE_RELAY_PLUGIN_SIGNAL_DURATION);
}
