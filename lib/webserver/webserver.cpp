#include "webserver.h"
#include <ETH.h>

#include <ESPAsyncWebServer.h>
#include <Update.h>

#include "configuration.h"
#include "debug_oz.h"
#include "setting.h"
#include "static.h"
#include "plugins.h"

AsyncWebServer *_oz_server;

String make_plugin_json_param(uint16_t key, params_t params)
{
    DynamicJsonDocument jsonBuffer(512);

    jsonBuffer["key"] = key;
    for (auto item : params)
    {
        jsonBuffer[item.first] = item.second;
    }

    String ret;
    serializeJson(jsonBuffer, ret);
    jsonBuffer.clear();
    ret.replace("\"", "'");
    return ret;
}

String get_plugin_name(String key)
{
    uint i_key = atoi(key.c_str());

    if (i_key > 0)
    {
        auto item = plugin_list.find(i_key);
        if(item != plugin_list.end())
        {
            return plugin_list[i_key].name;
        }
    }

    return String();
}

String get_stringied_plugin(JsonObject item)
{
    String ret;
    serializeJson(item, ret);
    ret.replace("\"", "'");
    return ret;
}

#ifdef WEB_SECURE
static bool webAuthenticate(AsyncWebServerRequest *request)
{
    String UserName = SETTING(DB_SETTING_WEBUSER);
    String Password = SETTING(DB_SETTING_WEBPASS);
    return request->authenticate(UserName.c_str(), Password.c_str());
}
#endif

/**
 * @b  /index" ok
 */
void on_home(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_image, index_image_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("X-XSS-Protection", "1; mode=block");
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "deny");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

/**
 * @b  /state ok
 */
const size_t json_state_dimension = 512;

void on_state(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonDocument jsonBuffer(json_state_dimension);

    JsonObject config = jsonBuffer.createNestedObject("config");

#if OZDEBUG_WEB_SUPPORT
    config["log"] = "y";
#else
    config["log"] = "n";
#endif

#ifdef OTA
    config["ota"] = "y";
#else
    config["ota"] = "n";
#endif

    JsonArray params = jsonBuffer.createNestedArray("states");

    JsonObject param00 = params.createNestedObject();
    param00["key"] = "Name";
    param00["value"] = SETTING("espname");

    JsonObject param01 = params.createNestedObject();
    param01["key"] = "IP";

    IPAddress ip = WiFi.localIP();
    if(ip == IPAddress())
        ip = ETH.localIP();

    param01["value"] = ip.toString();

    JsonObject param02 = params.createNestedObject();
    param02["key"] = "Chip Model";
    param02["value"] = ESP.getChipModel();

    JsonObject param03 = params.createNestedObject();
    param03["key"] = "version";
    param03["value"] = SW_VERSION;

    JsonObject param04 = params.createNestedObject();
    param04["key"] = "freememory";
    param04["value"] = ESP.getFreeHeap();

    JsonObject param07 = params.createNestedObject();
    param07["key"] = "Chip temperature";
    param07["value"] = temperatureRead();

    JsonObject param08 = params.createNestedObject();
    param08["key"] = "System stability";
    int stablity_index = atoi(Database.get_internal_parameter(STABILITY_CHECK_PARAM_NAME).c_str());
    param08["value"] = stablity_index > 0 ? "Stable" : "Unstable";

    serializeJson(jsonBuffer, *response);
    jsonBuffer.clear();
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

/**
 * @b  /reboot
 */
void on_reboot(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->printf("{ \"result\": \"%s\" }",RESP_OK);
    request->send(response);
    
    esp_restart();
}

/**
 * @b  /unlock
 */
void on_unlock(AsyncWebServerRequest *request)
{
    bool valid = false;

    if(Database.set_internal_parameter(STABILITY_CHECK_PARAM_NAME, String(MAX_CRASH_COUNT)) == OZDB_ERR_OK)
        if(Database.commit() == OZDB_ERR_OK)
            valid = true;

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    const char *ret = valid ? RESP_OK : RESP_ERROE;
    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
}

/**
 * @b  /parameter  ok
 */
const size_t json_parameter_dimension = 2048;

void on_parameter(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonDocument jsonBuffer(json_parameter_dimension);

    JsonArray data = jsonBuffer.createNestedArray("parameter");

    for (auto item : Parameters)
    {
        JsonObject setting = data.createNestedObject();
        setting["key"] = item.first;
        setting["value"] = SETTING(item.first);
    }

    serializeJson(jsonBuffer, *response);
    jsonBuffer.clear();
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

/**
 * @b  /saveparameter
 */
String parameter_recived_string;

void on_save_parameter(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (index == 0)
        parameter_recived_string.clear();

    for (size_t i = 0; i < len; i++)
    {
        parameter_recived_string += char(*data);
        data++;
    }
}

void on_save_parameter_end(AsyncWebServerRequest *request)
{
    bool valid = false;

    DynamicJsonDocument jsonBuffer(json_parameter_dimension);

    auto error = deserializeJson(jsonBuffer, parameter_recived_string);

    if (!error)
    {
        JsonObject root = jsonBuffer.as<JsonObject>();
        for (JsonPair kv : root)
        {
            String str_value = kv.value();
            String str_key = kv.key().c_str();

            auto dberr = Database.set_base_parameter(str_key, str_value);
            if (dberr != OZDB_ERR_OK)
            {
                OZ_LOGE(WEBTAG, "Error in save parameter Code: %u", dberr);
            }
        }
        valid = true;
        Database.commit();
    }

    jsonBuffer.clear();

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    const char *ret = valid ? RESP_OK : RESP_ERROE;
    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
    parameter_recived_string.clear();
}

/**
 * @b  /plugindef ok
 */
const size_t json_plugin_dimension = 6144;

void on_plugin_definition(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonDocument jsonBuffer(json_plugin_dimension);

    JsonArray data = jsonBuffer.createNestedArray("plugin");

    for (auto item : plugin_list)
    {
        JsonObject setting = data.createNestedObject();
        setting["key"] = item.first;
        setting["name"] = item.second.name;
        setting["value"] = make_plugin_json_param(item.first, item.second.parameters);
    }

    serializeJson(jsonBuffer, *response);
    jsonBuffer.clear();
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

/**
 * @b  /plugin  ok
 */
void on_plugin_list(AsyncWebServerRequest *request)
{
    auto plugins = Database.get_plugin();

    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonDocument jsonBuffer(json_plugin_dimension);

    JsonArray data = jsonBuffer.createNestedArray("psetting");

    for (auto item : plugins)
    {
        JsonObject setting = data.createNestedObject();
        setting["name"] = get_plugin_name(item["key"]);
        setting["value"] = get_stringied_plugin(item);
    }

    serializeJson(jsonBuffer, *response);
    jsonBuffer.clear();
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

/**
 * @b  /saveplugin  ok
 */
String plugin_recived_string;

void on_plugin_save(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (index == 0)
        plugin_recived_string.clear();

    for (size_t i = 0; i < len; i++)
    {
        plugin_recived_string += char(*data);
        data++;
    }
}

void on_plugin_save_end(AsyncWebServerRequest *request)
{
    bool valid = false;

    if (plugin_recived_string.indexOf("NaN") < 0)
    {
        int err = Database.set_plugin(plugin_recived_string.c_str());
        if (err == OZDB_ERR_OK)
        {
            valid = true;

            Database.commit();
        }
        else
        {
            OZ_LOGE(WEBTAG, "Save Plugin: Database error!! code[%d]", err);
        }
    }
    else
    {
        OZ_LOGE(WEBTAG, "Save Plugin: Required field not present!!");
    }

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    const char *ret = valid ? RESP_OK : RESP_ERROE;
    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
    plugin_recived_string.clear();
}

/**
 * @b  /getdb  ok
 */
void on_get_db_file(AsyncWebServerRequest *request)
{
    String fileContent = Database.get_all_db();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", fileContent);

    String ContentDisposition = "attachment; filename=\"OzEsp_" + SETTING(DB_SETTING_ESPNAME) + "_db.json\"";
    response->addHeader("Content-Disposition", ContentDisposition);
    request->send(response);
}

/**
 * @b  /savedb
 */
String database_recived_string;

void on_save_db_file(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    OZ_LOGV(WEBTAG,"RECIVE %u ",len);

    if (index == 0)
        database_recived_string.clear();

    for (size_t i = 0; i < len; i++)
    {
        database_recived_string += char(*data);
        data++;
    }
}

void on_save_db_file_end(AsyncWebServerRequest *request)
{
    OZ_LOGV(WEBTAG,"new db %s ",database_recived_string.c_str());

    bool valid = false;

    int err = Database.set_all_db(database_recived_string);

    if (err == OZDB_ERR_OK)
    {
        valid = true;
    }
    else
    {
        OZ_LOGW(WEBTAG, "Set new db error code: %d",err);
    }

    AsyncWebServerResponse *response = request->beginResponse(  valid ? 200 : 500, "text/plain", valid ? RESP_OK : RESP_ERROE );
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    database_recived_string.clear();
}

/**
 * @b  /cleardb
 */
void on_reset_db(AsyncWebServerRequest *request)
{
    bool valid = false;

    int err = Database.clear_db();

    if (err == OZDB_ERR_OK)
    {
        valid = true;
    }

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    const char *ret = valid ? RESP_OK : RESP_ERROE;
    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
}

#if OZDEBUG_WEB_SUPPORT
/**
 * @b  /log
 */
size_t json_log_dimension = 4096;

void on_get_log(AsyncWebServerRequest *request)
{
    DynamicJsonDocument jsonBuffer(json_log_dimension);

    JsonArray data = jsonBuffer.createNestedArray("debug");

    for (auto item : debugList.get())
    {
        JsonObject setting = data.createNestedObject();
        setting["row"] = item;
    }

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    serializeJson(jsonBuffer, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}
#endif

#ifdef OTA
void _on_ota_end(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? RESP_ERROE : RESP_OK);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

    if (!Update.hasError())
    {
        OZ_LOGD(WEBTAG,"Update Complete");
        //ota_reboot_tiker.once_ms(500, ota_reboot_tiker_task);
    }
}

void _on_ota(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
        {
            //Update.printError(Serial);
        }
    }

    // Write chunked data to the free sketch space
    if (Update.write(data, len) != len)
    {
        //Update.printError(Serial);
    }

    if (final)
    { // if the final flag is set then this is the last frame of data
        if (Update.end(true))
        { //true to set the size to the current progress
        }
        //Update.printError(Serial);
    }
}
#endif

/**
 * @b
 */
void web_server_o_begin()
{
    // Create server
    uint16_t port = SETTING_I(DB_SETTING_PORT);

    _oz_server = new AsyncWebServer(port);

    // Rewrites
    _oz_server->rewrite(COMMAND_HOME, COMMAND_HOME_2);

    // 404
    _oz_server->onNotFound([](AsyncWebServerRequest *request)
                           { request->send(404); });

    _oz_server->on(COMMAND_HOME_2, HTTP_GET, on_home);

    _oz_server->on(COMMAND_STATE, HTTP_GET, on_state);

    _oz_server->on(COMMAND_REBOOT, HTTP_GET, on_reboot);

    _oz_server->on(COMMAND_UNLOCK, HTTP_GET, on_unlock);

    _oz_server->on(COMMAND_PARAMETER_GET, HTTP_GET, on_parameter);
    _oz_server->on(COMMAND_PARAMETER_SAVE, HTTP_POST, on_save_parameter_end, NULL, on_save_parameter);

    _oz_server->on(COMMAND_PLUGIN_DEFINITION, HTTP_GET, on_plugin_definition);
    _oz_server->on(COMMAND_PLUGIN_ITEM, HTTP_GET, on_plugin_list);
    _oz_server->on(COMMAND_PLUGIN_SAVE, HTTP_POST, on_plugin_save_end, NULL, on_plugin_save);

    _oz_server->on(COMMAND_DB_GET, HTTP_GET, on_get_db_file);
    _oz_server->on(COMMAND_DB_SAVE, HTTP_POST, on_save_db_file_end, on_save_db_file);
    _oz_server->on(COMMAND_DB_RESET, HTTP_GET, on_reset_db);

#if OZDEBUG_WEB_SUPPORT
    _oz_server->on(COMMAND_LOG, HTTP_GET, on_get_log);
#endif

#ifdef OTA // OTA UPDATE COMMAND
    _oz_server->on(COMMAND_OTA_UPDATYE, HTTP_POST, _on_ota_end, _on_ota);
#endif

    _oz_server->begin();

    OZ_LOGI(WEBTAG, "Webserver running on port %u", port);
}
