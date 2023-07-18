#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <plugin_base.h>
#include <oz_multiroom.h>

#define AUDIO_SERVER_STR_EN "Enable"
#define AUDIO_SERVER_STR_BCK "BCK"
#define AUDIO_SERVER_STR_RLC "RLC"
#define AUDIO_SERVER_STR_DATA "Data"


/**     
 *     Audio Server Plugin
 */
class audio_server : public plugin_base
{
public:
    audio_server(params_t init);

    static audio_server *COTR(params_t init) { return new audio_server(init); }

    static params_t PARAM()
    {
        return params_t
        {
            {PLUGIN_BASE_STR_NAME,PLUGIN_STR_NAN},
            {AUDIO_SERVER_STR_EN,PLUGIN_STR_NAN},
            {AUDIO_SERVER_STR_BCK,PLUGIN_STR_NAN},
            {AUDIO_SERVER_STR_RLC,PLUGIN_STR_NAN},
            {AUDIO_SERVER_STR_DATA,PLUGIN_STR_NAN},
        };
    }

protected:
    void _interrupt(uint8_t pin) override;
    void _send_response(String Topic, String Message) override;
    void _force_update() override;

    bool _start_server();
    bool _stop_server();

    bool _is_on;

    String _topic_action, _topic_state;

    int8_t _enable_pin;

    oz_multiroom _mr;
};
#endif