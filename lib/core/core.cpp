#include "core.h"
#include <ETH.h>

#include <esp_task_wdt.h>

#include <WString.h>
#include <map>
#include <list>

#include <configuration.h>
#include <plugins.h>
#include <gpio_oz.h>
#include <mqtt_oz.h>
#include <wifi_oz.h>

#include <oz_task.h>

#include "queue"

static std::map<String, uint16_t> core_topic_plugin;
static std::map<uint8_t, uint16_t> core_interrup_plugin;
static std::vector<plugin_base *> core_plugins;

static String core_topic_general_reboot;
static String core_topic_general_update;

static String core_topic_personal_reboot;
static String core_topic_personal_update;

static String core_topic_request_ip;
static String core_topic_present_ip;

static String core_topic_available;

//###################### UTILITY ############################################

String CoreBuildTopic(const char *first, const char *second)
{
    String temp;
    temp = first;
    temp += char(MQTT_TOPIC_SEPARATOR_CHAR);
    temp += second;
    return temp;
}

String CoreBuildTopic(const char *first, const char *second, const char *tree)
{
    String temp;
    temp = first;
    temp += char(MQTT_TOPIC_SEPARATOR_CHAR);
    temp += second;
    temp += char(MQTT_TOPIC_SEPARATOR_CHAR);
    temp += tree;
    return temp;
}

//###################### SENSOR ############################################

static std::vector<TimerHandle_t> _tiker_sensors;
static QueueHandle_t _sensor_queue_handle;

void CoreSensorTask(void *Parameter)
{
    plugin_base *plugin = reinterpret_cast<plugin_base *>(Parameter);
    plugin->execute_sensor();
    vTaskDelete(NULL);
}

void CoreLoopSensorTask(void *Parameter)
{
    for (;;)
    {
        plugin_base *plugin;

        if (xQueueReceive(_sensor_queue_handle, &plugin, portMAX_DELAY) == pdPASS)
        {
            xTaskCreate_WFH(
                CoreSensorTask,            // Function that should be called
                "SensorTask",              // Name of the task (for debugging)
                CORE_PLUGIN_TASK_MEMORY,   // Stack size (bytes)
                plugin,                    // Parameter to pass
                CORE_PLUGIN_TASK_PRIORITY, // Task priority
                NULL,                      // Task handle
                portMAX_DELAY);
        }
    }
    vTaskDelete(NULL);
}

void ICACHE_RAM_ATTR CoreExecuteSensorISR(TimerHandle_t xTimer)
{
    plugin_base *plugin = (plugin_base *)pvTimerGetTimerID(xTimer);
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(_sensor_queue_handle, &plugin, &xHigherPriorityTaskWoken);
}

//###################### INTERRUPT ############################################

struct core_interrupt_parameter
{
    plugin_base *plugin;
    uint16_t pin;
};

void CoreExecuteInterrupt(void *Parameter)
{
    // if (esp_task_wdt_add(NULL) != ESP_OK)
    // if (esp_task_wdt_delete(NULL) != ESP_OK)
    delay(200); //delay stabilize input bouncing

    core_interrupt_parameter *parameter = reinterpret_cast<core_interrupt_parameter *>(Parameter);
    parameter->plugin->interrupt(parameter->pin);
    vTaskDelete(NULL);
}

void CoreInterruptTask(void *parameter)
{
    if (core_interrup_plugin.size() == 0)
        vTaskDelete(NULL);

    for (;;)
    {
        int8_t pin = GPIOZ.getInterrupt();

        if (!GPIO_IS_VALID_GPIO(pin))
            continue;

        auto it = core_interrup_plugin.find(pin);

        if (it != core_interrup_plugin.end() && it->second < core_plugins.size())
        {
            core_interrupt_parameter newParameter;
            newParameter.pin = pin;
            newParameter.plugin = core_plugins[it->second];


            xTaskCreate_WFH(
                CoreExecuteInterrupt,      // Function that should be called
                "InterruptTask",           // Name of the task (for debugging)
                CORE_PLUGIN_TASK_MEMORY,   // Stack size (bytes)
                &newParameter,             // Parameter to pass
                CORE_PLUGIN_TASK_PRIORITY, // Task priority
                NULL,                      // Task handle
                portMAX_DELAY);
        }
    }

    vTaskDelete(NULL);
}

//###################### MQTT ############################################
/*
void CoreUpdateTask(void *parameter)
{
    plugin_base * plugin = reinterpret_cast<plugin_base *>(parameter);
    plugin->force_update();
    vTaskDelete(NULL);
}
*/

void CoreRunPluginUpdate()
{
    OZMQTT.send(core_topic_available, MQTT_PAYLOAD_AVAILABLE);

    for (auto plugin : core_plugins)
    {
        plugin->force_update();
        /*
                    xTaskCreate_WFH(
                    CoreUpdateTask,            // Function that should be called
                    "CoreUpdateTask",          // Name of the task (for debugging)
                    CORE_PLUGIN_TASK_MEMORY,   // Stack size (bytes)
                    plugin,                    // Parameter to pass
                    CORE_PLUGIN_TASK_PRIORITY, // Task priority
                    NULL,                      // Task handle
                    portMAX_DELAY);
                */
    }
}

void CoreOnMessageCallback(String topic, String message)
{
    if (topic.equals(core_topic_request_ip))
    {

        String address = WIFIOZ.getAddress();
        if(address.length() == 0 || address == "0.0.0.0" )
            address = ETH.localIP().toString();
        OZMQTT.send(core_topic_present_ip, address.c_str());
        return;
    }

    if (topic.equals(core_topic_general_update) || topic.equals(core_topic_personal_update))
    {
        CoreRunPluginUpdate();
        return;
    }

    if (topic.equals(core_topic_general_reboot) || topic.equals(core_topic_personal_reboot))
    {
        esp_restart();
        vTaskDelay(pdMS_TO_TICKS(2000));
        return;
    }

    if (core_topic_plugin.size() > 0)
    {
        for (auto it = core_topic_plugin.begin(); it != core_topic_plugin.end(); it++)
        {
            String editTopic;

            if(it->first.indexOf(char(MQTT_TOPIC_CHAR_ANY)) > 0)
            {
                editTopic = it->first.substring(0,it->first.length()-1); 
            }
            else
            {
                editTopic = it->first;
            }

            if(topic.indexOf(editTopic) >= 0)
            {
                core_plugins[it->second]->send_response(topic, message);
                return;
            }
        }

        /*
        auto it = core_topic_plugin.find(topic);

        if (it != core_topic_plugin.end() && it->second < core_plugins.size())
        {
            core_plugins[it->second]->send_response(topic, message);
        }
        */
    }
}

void CoreOnConnectionCallback()
{
    if (core_topic_plugin.size() > 0)
    {
        for (auto it = core_topic_plugin.begin(); it != core_topic_plugin.end(); ++it)
        {
            OZMQTT.subscribe(it->first);
        }
    }

    OZMQTT.subscribe(core_topic_general_reboot);
    OZMQTT.subscribe(core_topic_general_update);
    OZMQTT.subscribe(core_topic_personal_reboot);
    OZMQTT.subscribe(core_topic_personal_update);
    OZMQTT.subscribe(core_topic_request_ip);

    CoreRunPluginUpdate();
}

//############################  CORE  #######################################

#ifdef STABILITY_CHECK_ENABLE
void StabiltyTask(void *Parameter)
{
    delay(STABILITY_CHECK_TIME_MS);
    Database.set_internal_parameter(STABILITY_CHECK_PARAM_NAME, String(MAX_CRASH_COUNT));
    Database.commit();
    OZ_LOGD(CORE_DEBUG_TAG, "Stability check passed!!");
    vTaskDelete(NULL);
}
#endif

void core_begin()
{
    bool isStable = true;

#ifdef STABILITY_CHECK_ENABLE
    String stab = Database.get_internal_parameter(STABILITY_CHECK_PARAM_NAME);

    if (stab.length() > 0)
    {
        int index = atoi(stab.c_str());
        if (index > 0)
        {
            index--;
            Database.set_internal_parameter(STABILITY_CHECK_PARAM_NAME, String(index));
            Database.commit();

            xTaskCreate(
                StabiltyTask,                     // Function that should be called
                "StabiltyTask",                   // Name of the task (for debugging)
                STABILITY_TASK_MEMORY_ALLOCATION, // Stack size (bytes)
                NULL,                             // Parameter to pass
                0,                                // Task priority
                NULL                              // Task handle
            );
        }
        else
        {
            isStable = false;
            OZ_LOGW(CORE_DEBUG_TAG, "System is unstable");
        }
    }
    else
    {
        Database.set_internal_parameter(STABILITY_CHECK_PARAM_NAME, String(MAX_CRASH_COUNT));
        Database.commit();
    }
#endif

    core_topic_general_reboot = CoreBuildTopic(MQTT_GENERAL_TOPIC, MQTT_COMMAND_REBOOT);
    core_topic_general_update = CoreBuildTopic(MQTT_GENERAL_TOPIC, MQTT_COMMAND_UPDATE);
    core_topic_personal_reboot = CoreBuildTopic(SETTING(DB_SETTING_ESPNAME).c_str(), MQTT_COMMAND_REBOOT);
    core_topic_personal_update = CoreBuildTopic(SETTING(DB_SETTING_ESPNAME).c_str(), MQTT_COMMAND_UPDATE);
    core_topic_request_ip = CoreBuildTopic(MQTT_GENERAL_TOPIC, MQTT_COMMAND_REQUEST, SETTING(DB_SETTING_ESPNAME).c_str());
    core_topic_present_ip = CoreBuildTopic(MQTT_GENERAL_TOPIC, MQTT_COMMAND_PRESENT, SETTING(DB_SETTING_ESPNAME).c_str());

    if (!isStable)
        return;

    core_topic_available = CoreBuildTopic(SETTING(DB_SETTING_ESPNAME).c_str(), MQTT_AVAILABLE_TOPIC);

    core_plugins = plugins_get_all();

    _sensor_queue_handle = xQueueCreate(5, sizeof(plugin_base *));

    for (uint8_t i = 0; i < core_plugins.size(); i++)
    {
        uint8_t type = core_plugins[i]->getType();

        if (type & TYPE_MQTT)
        {
            for (auto topic : core_plugins[i]->get_mqtt_topic())
            {
                core_topic_plugin.insert({topic, i});
            }
        }

        if (type & TYPE_SENSOR)
        {
            if (core_plugins[i]->sensor_delay() > MIN_SENSOR_DELAY)
            {
                auto timer = xTimerCreate(
                    "SensorTiker",
                    pdMS_TO_TICKS(core_plugins[i]->sensor_delay()),
                    pdTRUE,
                    core_plugins[i],
                    CoreExecuteSensorISR);

                xTimerStart(timer, 0);

                _tiker_sensors.push_back(timer);
            }
        }

        if (type & TYPE_INTERRUPT)
        {
            for (auto interrupt_pin : core_plugins[i]->get_interrupt_pins())
            {
                core_interrup_plugin.insert({interrupt_pin, i});
            }
        }
    }

    OZMQTT.begin(CoreOnMessageCallback, CoreOnConnectionCallback);

    if (core_interrup_plugin.size() > 0)
    {
        xTaskCreate(
            CoreInterruptTask,            // Function that should be called
            "InterruptTask",              // Name of the task (for debugging)
            CORE_INTERRUPT_TASK_MEMORY,   // Stack size (bytes)
            NULL,                         // Parameter to pass
            CORE_INTERRUPT_TASK_PRIORITY, // Task priority
            NULL                          // Task handle
        );
    }

    if (_tiker_sensors.size() > 0)
    {
        xTaskCreate(
            CoreLoopSensorTask,        // Function that should be called
            "LoopTaskSensor",          // Name of the task (for debugging)
            CORE_SENSOR_TASK_MEMORY,   // Stack size (bytes)
            NULL,                      // Parameter to pass
            CORE_SENSOR_TASK_PRIORITY, // Task priority
            NULL                       // Task handle
        );
    }
}