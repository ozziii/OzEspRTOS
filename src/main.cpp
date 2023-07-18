#include <Arduino.h>
#include <ETH.h>

#include <setting.h>
#include <debug_oz.h>
#include <wifi_oz.h>
#include <core.h>
#include <webserver.h>

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
   * Network initialization
   *
   *
   */
  switch (SETTING_I(DB_SETTING_ETH))
  {
  case ETH_TYPE_LAN8720:  // Ethernet on LAN8720
    if(!ETH.begin(1, 17, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN))
    {
      std::map<String, String> Networks;
      Networks.insert({SETTING(DB_SETTING_WIFISSID1), SETTING(DB_SETTING_WIFIPASS1)});
      Networks.insert({SETTING(DB_SETTING_WIFISSID2), SETTING(DB_SETTING_WIFIPASS2)});
      Networks.insert({SETTING(DB_SETTING_WIFISSID3), SETTING(DB_SETTING_WIFIPASS3)});
      WIFIOZ.begin(SETTING(DB_SETTING_ESPNAME), WIFI_AP_SSID, WIFI_AP_PASS, Networks);
    }
    break;
  default:   // Default use wifi
    std::map<String, String> Networks;
    Networks.insert({SETTING(DB_SETTING_WIFISSID1), SETTING(DB_SETTING_WIFIPASS1)});
    Networks.insert({SETTING(DB_SETTING_WIFISSID2), SETTING(DB_SETTING_WIFIPASS2)});
    Networks.insert({SETTING(DB_SETTING_WIFISSID3), SETTING(DB_SETTING_WIFIPASS3)});
    WIFIOZ.begin(SETTING(DB_SETTING_ESPNAME), WIFI_AP_SSID, WIFI_AP_PASS, Networks);
    break;
  }

  //

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