#ifndef PLUGINS_o_H
#define PLUGINS_o_H

#include <plugin_base.h>
#include <map>

#include "plugins/analog_read_p.h"
#include "plugins/buzzer_p.h"
#include "plugins/binary_sensor_p.h"
#include "plugins/bme680_p2.h"
#include "plugins/bme280_p.h"
#include "plugins/led_p.h"
#include "plugins/switch_p.h"
#include "plugins/bistable_realy_p.h"
#include "plugins/pzem004t_p.h"
#include "plugins/dc_motor_p.h"
#include "plugins/speaker_p.h"
#include "plugins/audio_server_p.h"
#include "plugins/vmc_p.h"
#include "plugins/lora_to_mqtt_p.h"
#include "plugins/test_plugin.h"
#include "plugins/ir_send_p.h"
#include "plugins/ram_sensor_p.h"
#include "plugins/pwm_switch_p.h"
#include "plugins/tsl2561_p.h"

struct plugin_base_parameter
{
    const char *Name;
    const char *Default;
};

struct plugin_list_item
{
    const char *name;
    params_t parameters;
    plugin_base_ctor ctor;
};

static std::map<uint16_t, plugin_list_item> plugin_list =
    {
        {1, {"Ram", freeram::PARAM(), freeram::COTR}},
        {4, {"Bistable relay", bistable_relay::PARAM(), bistable_relay::COTR}},
        {5, {"Switch or Relay", switch_p::PARAM(), switch_p::COTR}},
        {6, {"LED pwm control", led_plugin::PARAM(), led_plugin::COTR}},
        {7, {"VMC", vmc_plugin::PARAM(), vmc_plugin::COTR}},
        {8, {"Binary Sensor", binary_sensor::PARAM(), binary_sensor::COTR}},
        {9, {"LoRa", lora_to_mqtt::PARAM(), lora_to_mqtt::COTR}},  
        {10, {"Buzzer", buzzer::PARAM(), buzzer::COTR}},
        {11, {"DC MOTOR", dc_motor::PARAM(), dc_motor::COTR}},
        {12, {"IR Send", ir_send_plugin::PARAM(), ir_send_plugin::COTR}},
        {13, {"Analog Read", analog_read::PARAM(), analog_read::COTR}},
        {14, {"PWM Switch", pwm_switch::PARAM(), pwm_switch::COTR}},

        {20, {"Speaker", speaker::PARAM(), speaker::COTR}},
        {21, {"Audio Server", audio_server::PARAM(), audio_server::COTR}},
        {22, {"PZEM-004T", pzem004t::PARAM(), pzem004t::COTR}},
        
        {24, {"BME280", bme280::PARAM(), bme280::COTR}},
        {25, {"BME680", bme6802::PARAM(), bme6802::COTR}}, 
        {26, {"TLS2561", tsl2561::PARAM(), tsl2561::COTR}}, 

        {99, {"Test", test_plugin::PARAM(), test_plugin::COTR}}
    };

std::vector<plugin_base *> plugins_get_all();

#endif