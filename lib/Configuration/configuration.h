#ifndef CONFIGURATION_h
#define CONFIGURATION_h

#include <Arduino.h>

/*===========================================================================
 * ============================= GENERAL CONFIGURATION =========================
 * ===========================================================================*/

#define SW_VERSION "0.10.2.0 alfa"

/*===========================================================================
 * ============================= DEBUG CONFIGURATION =========================
 * ===========================================================================*/
#define RELESE_VERSION 1

#define OZDEBUG_LOG_LEVEL_NONE (0)
#define OZDEBUG_LOG_LEVEL_ERROR (1)
#define OZDEBUG_LOG_LEVEL_WARN (2)
#define OZDEBUG_LOG_LEVEL_INFO (3)
#define OZDEBUG_LOG_LEVEL_DEBUG (4)
#define OZDEBUG_LOG_LEVEL_VERBOSE (5)

#if RELESE_VERSION
#define OZDEBUG_ARDUINO_ESP_LOG_LEVEL CONFIG_ARDUHAL_LOG_DEFAULT_LEVEL
#define OZDEBUG_ESPIDF_DEBUG_LEVEL 0             // ENABLE INTERNAL SERIAL DEBUG
#define OZDEBUG_LOG_LEVEL OZDEBUG_LOG_LEVEL_INFO// LOG LEVEL SETTING
#else
#define OZDEBUG_ARDUINO_ESP_LOG_LEVEL CONFIG_ARDUHAL_LOG_DEFAULT_LEVEL
#define OZDEBUG_ESPIDF_DEBUG_LEVEL 5                // ENABLE INTERNAL SERIAL DEBUG
#define OZDEBUG_LOG_LEVEL OZDEBUG_LOG_LEVEL_VERBOSE // LOG LEVEL SETTING
#endif

#if OZDEBUG_LOG_LEVEL

#if RELESE_VERSION
#define OZDEBUG_WEB_SUPPORT 1    // ENABLE DEBUG GUI WEB
#define OZDEBUG_SERIAL_SUPPORT 0 // ENABLE SERIAL DEBUG
#else
#define OZDEBUG_WEB_SUPPORT 0    // ENABLE DEBUG GUI WEB
#define OZDEBUG_SERIAL_SUPPORT 1 // ENABLE SERIAL DEBUG
#endif

#define OZDEBUG_WEB_COLOR 1    // ENABLE WEB COLOR
#define OZDEBUG_SERIAL_COLOR 0 // ENABLE SERIAL COLOR

#if OZDEBUG_WEB_SUPPORT
#define OZDEBUG_WEB_MAX_LIST 30
#endif

#if OZDEBUG_SERIAL_SUPPORT
#define OZDEBUG_SERIAL_PORT Serial // Default debugging port
#define OZDEBUG_SERIAL_PORT_BLAUD 115200
#endif

#endif

/*===========================================================================
 * ============================= SYSTEM CONFIGURATION ========================
 * ===========================================================================*/

//// SATBILITY
#define STABILITY_CHECK_ENABLE

#ifdef STABILITY_CHECK_ENABLE

#define STABILITY_TASK_MEMORY_ALLOCATION 8192
#define STABILITY_CHECK_PARAM_NAME "satb"
#define STABILITY_CHECK_TIME_MS 50000
#define MAX_CRASH_COUNT 5
#endif

// Enable OTA update
#define OTA

#define CORE_INTERRUPT_TASK_MEMORY 1024
#define CORE_INTERRUPT_TASK_PRIORITY 3

#define CORE_SENSOR_TASK_MEMORY 1024
#define CORE_SENSOR_TASK_PRIORITY 3

#define CORE_PLUGIN_TASK_MEMORY 8192
#define CORE_PLUGIN_TASK_PRIORITY 2

/*===========================================================================
 * ============================= WIFI CONFIGURATION ==========================
 * ===========================================================================*/

#define ETH_TYPE_WIFI 0
#define ETH_TYPE_LAN8720 1




#define WIFI_AP_SSID "ESPWIFI"
#define WIFI_AP_PASS "ESPWIFIESPWIFIESPWIFI"

/*===========================================================================
 * ============================= MQTT CONFIGURATION ==========================
 * ===========================================================================*/

#define MQTT_RECONNECTION_DELAY 2000
#define MQTT_WATCHDOG_DELAY 60000

#define MQTT_QoS 1
#define MQTT_RETAIN false

#define MQTT_TOPIC_SEPARATOR_CHAR 47
#define MQTT_TOPIC_CHAR_ANY 35

#define MQTT_GENERAL_TOPIC "OzEsp"

#define MQTT_COMMAND_UPDATE "update"
#define MQTT_COMMAND_REBOOT "reboot"
#define MQTT_COMMAND_DISCOVERY "discovery"
#define MQTT_COMMAND_REQUEST "request"
#define MQTT_COMMAND_PRESENT "present"
#define MQTT_COMMAND_ACTION "action"
#define MQTT_COMMAND_STATE "state"
#define MQTT_COMMAND_VOLUME "volume"
#define MQTT_COMMAND_VOLUME_STATE "volume_state"
#define MQTT_COMMAND_FORWARD "forward"
#define MQTT_COMMAND_REVERSE "reverse"
#define MQTT_COMMAND_STOP "stop"

#define MQTT_SENSOR_LUMINOSITY "luminosity"
#define MQTT_SENSOR_TEMPERATURE "temperature"
#define MQTT_SENSOR_HUMIDITY "humidity"
#define MQTT_SENSOR_PRESSURE "pressure"
#define MQTT_SENSOR_DEWPOINT "dewpoint"
#define MQTT_SENSOR_IAQ "iaq"
#define MQTT_SENSOR_CO "carbonmox"
#define MQTT_SENSOR_CURRENT "current"
#define MQTT_SENSOR_VOLTAGE "voltage"
#define MQTT_SENSOR_POWER "power"
#define MQTT_SENSOR_POWER_FACTOR "powerfactor"
#define MQTT_SENSOR_ENERGY "energy"
#define MQTT_SENSOR_VALUE "value"
#define MQTT_SENSOR_ALIVE "alive"
#define MQTT_SENSOR_BYTE "byte"

#define TOPIC_BRIGHTNESS "brightness"
#define TOPIC_RGB "rgb"

// Availability ( LAST WILL )
#define MQTT_AVAILABLE_TOPIC "available"
#define MQTT_PAYLOAD_AVAILABLE "online"
#define MQTT_PAYLOAD_NOT_AVAILABLE "offline"

// STATE
#define MQTT_STATE_ON "ON"
#define MQTT_STATE_OFF "OFF"

/*===========================================================================
 * ============================= HARDWARE CONFIGURATION ======================
 * ===========================================================================*/

// PIN PUMP DELAY

// PWM SETTING
#define PWM_FREQUENCY 5000            // NOT CHANHGE !!!
#define PWM_RESOLUTION 10             // NOT CHANHGE !!!
#define PWM_MAX_CHANNEL 16            // NOT CHANHGE !!!
#define MAX_INTERRUPT_TASK_PRIORITY 3 // NOT CHANHGE !!!

/*===========================================================================
 * ============================= WEB SERVER CONFIGURATION ====================
 * ===========================================================================
 *
 *   WEB COMMAND LIST  IMPORTATN / BEFORE COMMAND!!!
 */

// MAX JSON DOC SIZE
#define JSON_DOCUMENT_SIZE 4096

//#define WEB_SECURE

#define COMMAND_HOME "/"
#define COMMAND_HOME_2 "/index"
#define COMMAND_STATE "/state"
#define COMMAND_REBOOT "/reboot"
#define COMMAND_UNLOCK "/unlock"
#define COMMAND_LOG "/log"
#define COMMAND_PARAMETER_GET "/parameter"
#define COMMAND_PARAMETER_SAVE "/saveparameter"
#define COMMAND_PLUGIN_DEFINITION "/plugindef"
#define COMMAND_PLUGIN_ITEM "/plugin"
#define COMMAND_PLUGIN_SAVE "/saveplugin"
#define COMMAND_DB_RESET "/cleardb"
#define COMMAND_DB_GET "/getdb"
#define COMMAND_DB_SAVE "/savedb"
#define COMMAND_OTA_UPDATYE "/update"

/*
 *   WEB PARAMETER LIST
 */
#define PARAM_NAME "name"
#define PARAM_VALUE "val"
#define PARAM_TOPIC "topic"
#define PARAM_MESSAGE "message"

/*
 *   WEB RESPOSE
 */
#define RESP_OK "O"
#define RESP_ERROE "E"
#define RESP_WRONG_PARAMETER "W"

/*===========================================================================
 * ============================= PLUG-IN       =============================
 * ===========================================================================
 */
#define PLUGIN_INIT_SEPARATOR_CHAR 47
#define PLUGIN_PIN_SEPARATOR_CHAR '-'

#define MIN_SENSOR_DELAY 100 // millisecond

#define PLUGIN_STR_NAN "NaN"

/*===========================================================================
 * =================  PLUG-IN  ANALOG READ   =================================
 * ===========================================================================*/

#define ANALOG_READ_PLUGIN_ATTENUATION ADC_0db
#define ANALOG_READ_PLUGIN_RESOLUTION 12

/*===========================================================================
 * =================  PLUG-IN  BISTABLE RELAY  =================================
 * ===========================================================================*/

#define BISTABLE_RELAY_PLUGIN_SIGNAL_DURATION 200 // [ms]

/*===========================================================================
 * =================  PLUG-IN  LED            =================================
 * ===========================================================================*/

#define PLUGIN_LED_JSON_DOCUMENT_SIZE 512 // NOT CHANGE !!
#define PLUGIN_LED_MIN_PWM 0              // NOT CHANGE !!
#define PLUGIN_LED_MAX_PWM 1024           // NOT CHANGE !!
#define PLUGIN_DEFAULT_BRIGHTNESS 125

/*===========================================================================
 * =================  BME680             =================================
 * ===========================================================================*/

#define BME680_PLUGIN_SENSOR_TIMEOUT 30000


/*===========================================================================
 * =================  TSL2561             =================================
 * ===========================================================================*/

#define TSL2561_PLUGIN_SENSOR_TIMEOUT 30000

/*===========================================================================
 * =================  PLUG-IN  MULTIROOM SPEAKER    ====================================
 * ===========================================================================*/

#define MULTICAST_PLUGIN_SERVER_STATE_TOPIC "multiroom/server2/state"

#define MULTICAST_PLUGIN_CLIENT_REQEST_TOPIC "multiroom/client2/request"
#define MULTICAST_PLUGIN_CLIENT_READY_TOPIC "multiroom/client2/ready"
#define MULTICAST_PLUGIN_CLIENT_STOP_TOPIC "multiroom/client2/stop"

#define MULTICAST_PLUGIN_PLAY_MESSAGE "play"
#define MULTICAST_PLUGIN_STOP_MESSAGE "stop"

#define SPEAKER_PLUGIN_MULTICAST_PORT 4321
#define SPEAKER_PLUGIN_LOW_GAIN 0
#define SPEAKER_PLUGIN_TOP_GAIN 255
#define SPEAKER_PLUGIN_DEFAULT_GAIN 60

/*===========================================================================
 * =================================== PLUG-IN VMC ====================================
 * ===========================================================================*/

#define VMC_STATE_AUTO "auto"
#define VMC_STATE_IN "in"
#define VMC_STATE_OUT "out"
#define VMC_STATE_OFF "off"

#define VMC_PAYLOAD_SEPARATOR_CHAR 44

/*===========================================================================================
 * =================================== BLE TO UDP PLUGIN ====================================
 * ==========================================================================================*/


#define A2DPSINK_ACTION_PLAY "play"
#define A2DPSINK_ACTION_PAUSE "pause"
#define A2DPSINK_ACTION_NEXT "next"
#define A2DPSINK_ACTION_PREV "prev"
#define A2DPSINK_ACTION_VOLUP "volup"
#define A2DPSINK_ACTION_VOLDOWN "voldown"


/*======================================================================================
 * =================================== LORA TO MQTT  ====================================
 * ======================================================================================*/

#define LORA_MAX_TRY_ON_ERROR 3

#define LORA_TASK_LOOP_DELAY 1000
#define LORA_TOPIC_SEPARATOR_CHAR 47
#define LORA_MESSAGE_SEPARATOR_CHAR 38
#define LORA_TOPIC_SEND_STRING "send"
#define LORA_TOPIC_RECIVE_STRING "recive"
#define LORA_KEY_LENGTH 32
const uint8_t LORA_PRESHARED_KEY[LORA_KEY_LENGTH] =
    {
        0x32, 0x35, 0xA3, 0xB1, 0x32, 0x35, 0xA3, 0xB1,
        0x32, 0x35, 0xA3, 0xB1, 0x32, 0x35, 0xA3, 0xB1,
        0x12, 0x35, 0xA3, 0xB1, 0x32, 0x1C, 0xB3, 0xB1,
        0x52, 0x85, 0xA4, 0xB1, 0x62, 0x85, 0xA3, 0xB1};


#define LORA_KEY = {  0x32, 0x35, 0xA3, 0xB1, 0x32, 0x35, 0xA3, 0xB1, 0x32, 0x35, 0xA3, 0xB1, 0x32, 0x35, 0xA3, 0xB1,0x12, 0x35, 0xA3, 0xB1, 0x32, 0x1C, 0xB3, 0xB1,0x52, 0x85, 0xA4, 0xB1, 0x62, 0x85, 0xA3, 0xB1}


#endif