#ifndef SETTING_H
#define SETTING_H

#include "oz_db.h"

#define SETTING(key)  Database.get_base_parameter(key)
#define SETTING_I(key) atoi(Database.get_base_parameter(key).c_str())
#define SETTING_F(key) atof(Database.get_base_parameter(key).c_str())


#define SETTINGF(...)  Database.get_base_parameter_f(##__VA_ARGS__)
#define SETTINGF_I(...)  atoi(SETTINGF(##__VA_ARGS__).c_str())
#define SETTINGF_F(...)  atoi(SETTINGF(##__VA_ARGS__).c_str())



#define DB_SETTING_INIT "init"
#define DB_SETTING_ESPNAME "espname"
#define DB_SETTING_WEBUSER "WEBUSER"
#define DB_SETTING_WEBPASS "WEBPASS"
#define DB_SETTING_MQTTIP  "MQTTIP"
#define DB_SETTING_MQTTPORT "MQTTPORT"
#define DB_SETTING_MQTTUSER "MQTTUSER"
#define DB_SETTING_MQTTPASS "MQTTPASS"

#define DB_SETTING_WIFISSID1 "WIFI1SSID"
#define DB_SETTING_WIFIPASS1 "WIFI1PASS"
#define DB_SETTING_WIFISSID2 "WIFI2SSID"
#define DB_SETTING_WIFIPASS2 "WIFI2PASS"
#define DB_SETTING_WIFISSID3 "WIFI3SSID"
#define DB_SETTING_WIFIPASS3 "WIFI3PASS"

#define DB_SETTING_IP     "LAN_IP"
#define DB_SETTING_GATEW  "LAN_GATEWAY"
#define DB_SETTING_SUBNET "LAN_SUBNETMASK"
#define DB_SETTING_DNS    "LAN_DNS"
#define DB_SETTING_PORT   "SERVER_PORT"

#define DB_SETTING_DEBOU   "DebouncingInterrupt"

/**
 * 
 * { 
 * "base"   : {"espname":"Pari","ip":"1"} , 
 * "internal"   : {"espname":"Pari","ip":"1"} , 
 * "plugin" : [
 *              {"key":2,"NAME":"LEND","pin":3},
 *              {"key":2,"NAME":"LEND","pin":3},
 *              {"key":2,"NAME":"LEND","pin":3},
 *              {"key":2,"NAME":"LEND","pin":3}
 *            ]
 * }   
 * 
 */



const std::map<String, String> Parameters= {
    {DB_SETTING_ESPNAME,"NoName"},
    {DB_SETTING_WEBUSER,"admin"},
    {DB_SETTING_WEBPASS,"admin"},
    {DB_SETTING_MQTTIP, "mqtt://ozziii.duckdns.org"},
    {DB_SETTING_MQTTPORT, "1883"},
    {DB_SETTING_MQTTUSER, "mqtt"},
    {DB_SETTING_MQTTPASS , "abbcd0ac-7fe8"},
    {DB_SETTING_WIFISSID1, "lukegeorge"},
    {DB_SETTING_WIFIPASS1, "microelettronica"},
    {DB_SETTING_WIFISSID2, "Alice-62361025"},
    {DB_SETTING_WIFIPASS2, "camillis1991de2000manuelamarialu"},
    {DB_SETTING_WIFISSID3, ""},
    {DB_SETTING_WIFIPASS3, ""},
    {DB_SETTING_IP, ""},     //IP ADDRESS
    {DB_SETTING_GATEW, ""},  //GATEWAY
    {DB_SETTING_SUBNET, ""}, //SUBNET MASK
    {DB_SETTING_DNS, ""},    //DNS
    {DB_SETTING_PORT, "80" },  //Server PORT
    {DB_SETTING_DEBOU, "200"}
};

extern oz_db Database;

#endif