#include <mqtt_oz.h>

#include <mqtt_client.h>

#include <debug_oz.h>
#include <setting.h>


#define MQTTTAG "MQTT"


static bool mqtt_is_connected = false;
OnMessageCallback mqttoz_onMessageCallback;
OnConnectionCallback mqttoz_onConnectionCallback;

static esp_mqtt_client_handle_t mqtt_client;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        OZ_LOGI(MQTTTAG,"MQTT CONNECTED");
        mqttoz_onConnectionCallback();
        mqtt_is_connected = true;
        break;
    case MQTT_EVENT_DATA:
    {
        std::string stdTopic(event->topic, event->topic_len);
        std::string stdMessage(event->data, event->data_len);

        String Topic(stdTopic.c_str());
        String Message(stdMessage.c_str());

        OZ_LOGV(MQTTTAG,"Rec: [%s] -> [%s] ",stdMessage.c_str(),stdTopic.c_str());

        mqttoz_onMessageCallback(Topic, Message);
        break;
    }
    case MQTT_EVENT_DISCONNECTED:
    {   
        mqtt_is_connected = false;
        break;
    }
    default:
        break;
    };
}

void mqtt_idf::begin(OnMessageCallback MessageCallback, OnConnectionCallback ConnectionCallback)
{
    mqtt_is_connected = false;

    mqttoz_onMessageCallback = MessageCallback;
    mqttoz_onConnectionCallback = ConnectionCallback;

    OZ_LOGV(MQTTTAG,"MQTT BEGIN ");

    this->_address = SETTING(DB_SETTING_MQTTIP).c_str();
    this->_port = SETTING_I(DB_SETTING_MQTTPORT);
    this->_client_id = SETTING(DB_SETTING_ESPNAME).c_str();
    this->_username = SETTING(DB_SETTING_MQTTUSER).c_str();
    this->_password = SETTING(DB_SETTING_MQTTPASS).c_str();
    this->_lwt_topic = this->lastWillTopic().c_str();

    const esp_mqtt_client_config_t mqtt_cfg = {
        .event_handle = nullptr,
        .event_loop_handle = nullptr,
        .host = nullptr,
        .uri = this->_address.c_str(),
        .port = this->_port,
        .client_id = this->_client_id.c_str(),
        .username = this->_username.c_str(),
        .password = this->_password.c_str(),
        .lwt_topic = this->_lwt_topic.c_str(),
        .lwt_msg = MQTT_PAYLOAD_NOT_AVAILABLE,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if(mqtt_client == NULL)
    {
        OZ_LOGE(MQTTTAG,"Create client error");
        return;
    }

    esp_err_t error = esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);

    if(error != ESP_OK)
    {
        OZ_LOGE(MQTTTAG,"Error on register event : %u",error);
        return;
    }

    error = esp_mqtt_client_start(mqtt_client);

    if(error != ESP_OK)
    {
        OZ_LOGE(MQTTTAG,"Error start client : %u",error);
        return;
    }
}

void mqtt_idf::send(String Topic, String Message, uint8_t QoS, bool Retain)
{
    OZ_LOGV(MQTTTAG,"Send [%s] -> [%s]",Message.c_str(),Topic.c_str());
    esp_mqtt_client_publish(mqtt_client, Topic.c_str(), Message.c_str(), Message.length(), QoS, Retain);
}

void mqtt_idf::subscribe(String Topic, uint8_t QoS)
{
    OZ_LOGI(MQTTTAG,"Sub -> [%s]",Topic.c_str());
    esp_mqtt_client_subscribe(mqtt_client, Topic.c_str(), QoS);
}

void mqtt_idf::subscribe(std::vector<String> Topics)
{
    for (uint16_t i = 0; i < Topics.size(); i++)
    {
        this->subscribe(Topics[i], MQTT_QoS);
    }
}

bool mqtt_idf::is_connected()
{
    return mqtt_is_connected;
}

String mqtt_idf::lastWillTopic()
{
    String lastWill;
    lastWill = SETTING(DB_SETTING_ESPNAME);
    lastWill += char(MQTT_TOPIC_SEPARATOR_CHAR);
    lastWill += MQTT_AVAILABLE_TOPIC;
    return lastWill;
}

mqtt_idf OZMQTT;