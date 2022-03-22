#include <Arduino.h>

#include <setting.h>
#include <debug_oz.h>
#include <wifi_oz.h>
#include <core.h>
#include <webserver.h>

#include <stdexcept>

void setup()
{

  /**
   * Initialize DEBUG
   * 
   */
  OZ_LOG_BEGIN();


  /**
   * No loop user remove WDT
   */
  disableLoopWDT();


  /**
   * WIFI initialization 
   * 
   * Feed network
   * Start WiFi 
   * 
   */
  std::map<String, String> Networks;
  Networks.insert({SETTING(DB_SETTING_WIFISSID1), SETTING(DB_SETTING_WIFIPASS1)});
  Networks.insert({SETTING(DB_SETTING_WIFISSID2), SETTING(DB_SETTING_WIFIPASS2)});
  Networks.insert({SETTING(DB_SETTING_WIFISSID3), SETTING(DB_SETTING_WIFIPASS3)});
  WIFIOZ.begin(SETTING(DB_SETTING_ESPNAME), WIFI_AP_SSID, WIFI_AP_PASS, Networks);

  /**
   * Main programm initialization 
   * 
   * Init Plug-in
   * 
   * Start MQTT
   * 
   * Init sensor tasks
   * Init interrupt task
   * Set mqtt massage calback
   * 
   */
  core_begin();


  /**
   * Initialize web server 
   * 
   */
  web_server_o_begin();


  //vTaskStartScheduler();

  /**
   *  Delete loopTaskHandle 
   * 
   *  Stop loop(){}
   * 
   */
  vTaskDelete(NULL);

}

void loop()
{
  
}