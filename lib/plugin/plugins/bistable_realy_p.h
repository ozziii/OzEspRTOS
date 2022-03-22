#ifndef BISTABLE_RELAY_P_H
#define BISTABLE_RELAY_P_H

#include "plugins/switch_p.h"

/**
*            
*     Bistable realy; If set, read status by external Pin (READ PIN)
*
*/
class bistable_relay : public switch_p
{
public:
    bistable_relay(params_t init);

    static bistable_relay * COTR(params_t init) { return new bistable_relay(init); }

    static params_t PARAM()
    {
        return switch_p::PARAM();
    }

protected:
    void _io_action(uint8_t state) override;


};

#endif