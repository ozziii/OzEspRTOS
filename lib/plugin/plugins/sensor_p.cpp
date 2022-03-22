#include "sensor_p.h"

#include "ArduinoJson.h"

sensor::sensor(params_t init) : plugin_base(init)
{
    this->_topic_state = this->buildTopic(MQTT_COMMAND_STATE);
}

void sensor::_execute_sensor()
{
    _sensor_update();

    DynamicJsonDocument JsonDocument(PLUGIN_LED_JSON_DOCUMENT_SIZE);

    char *c_temp = new char[16];
    for (uint8_t i = 0; i < this->_values.size(); i++)
    {
        if (this->_values[i].value < this->_values[i].max &&  this->_values[i].value > this->_values[i].min)
        {
            dtostrf(this->_values[i].value, 8, this->_values[i].decimal, c_temp);
            JsonDocument[this->_values[i].topic] = c_temp;
        }
    }
    delete[] c_temp;
    String message;
    serializeJson(JsonDocument, message);
    OZMQTT.send(this->_topic_state, message);
}
