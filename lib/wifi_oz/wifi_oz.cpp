#include <wifi_oz.h>

#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#ifdef WIFIOZ_OZDEBUG
#include <debug_oz.h>
#endif

TimerHandle_t _wifioz_connection_timer;
TimerHandle_t _wifioz_watchdog;

std::map<String, String> _wifioz_networkList;
String _wifioz_ssid_ap, _wifioz_pass_ap;
static String espname;

void wifioz_connect_wifi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_STA);
#ifdef WIFIOZ_OZDEBUG
    OZ_LOGD(WIFITAG, "Scanning network....");
#endif
    WiFi.scanNetworks(true, true);
}

void wifioz_connect_wifiISR(TimerHandle_t xTimer)
{
    wifioz_connect_wifi();
}

void wifioz_make_AP()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(_wifioz_ssid_ap.c_str(), _wifioz_pass_ap.c_str());
}

void wifioz_make_STA()
{
    uint16_t networkCount = WiFi.scanComplete();
    if (networkCount == WIFI_SCAN_FAILED)
    {
        xTimerStart(_wifioz_connection_timer, 0);
        return;
    }

    if (networkCount == WIFI_SCAN_RUNNING)
    {
        xTimerStart(_wifioz_connection_timer, 0);
        return;
    }

    if (networkCount == 0)
    {
#ifdef WIFIOZ_OZDEBUG
        OZ_LOGW(WIFITAG, "No network found!! run AP!!");
#endif
        wifioz_make_AP();
        return;
    }

    String ssid_scan;
    int32_t rssi_scan;
    uint8_t sec_scan;
    uint8_t *BSSID_scan;
    int32_t chan_scan;

    String ConnectingSSID = "";

    for (uint8_t i = 0; i < networkCount; ++i)
    {

        WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);
#ifdef WIFIOZ_OZDEBUG
        bool known = false;
#endif

        if (ssid_scan.length() != 0)
        {
            // TODO REMOVE EMPTY SSID CHOSE BEST RSSI
            if (_wifioz_networkList.find(ssid_scan) != _wifioz_networkList.end())
            {

#ifdef WIFIOZ_OZDEBUG
                known = true;
#endif
                ConnectingSSID = ssid_scan;
            }
        }

#ifdef WIFIOZ_OZDEBUG
        OZ_LOGI(WIFITAG, "%s BSSID: %02X:%02X:%02X:%02X:%02X:%02X CH: %2d RSSI: %3d SSID: %s",
                (known ? "-->" : "   "),
                BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5],
                chan_scan,
                rssi_scan,
                ssid_scan.c_str());
#endif
    }

    WiFi.scanDelete();

    if (ConnectingSSID.length() > 0)
    {
        auto it = _wifioz_networkList.find(ConnectingSSID);
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        WiFi.setHostname(espname.c_str());
        WiFi.begin(it->first.c_str(), it->second.c_str());
    }
    else
    {
#ifdef WIFIOZ_OZDEBUG
        OZ_LOGW(WIFITAG, "No konow network!! run AP!!");
#endif
        wifioz_make_AP();
    }
}

void wifioz_WatchdogISR(TimerHandle_t xTimer)
{
#ifdef WIFIOZ_OZDEBUG
    OZ_LOGV(WIFITAG, "Watchdog triggered!!");
#endif
    wifi_mode_t mode = WiFi.getMode();

    switch (mode)
    {
    case WIFI_MODE_NULL:
        wifioz_connect_wifi();
        break;
    case WIFI_MODE_STA:
    {
        if (!WiFi.isConnected())
            wifioz_connect_wifi();
    }
    break;
    case WIFI_MODE_AP:
    {
        if (WiFi.softAPgetStationNum() <= 0)
            wifioz_connect_wifi();
    }
    break;
    case WIFI_MODE_APSTA:
        wifioz_connect_wifi();
        break;
    case WIFI_MODE_MAX:
        wifioz_connect_wifi();
        break;
    }
}

void wifioz_event_handler(WiFiEvent_t event)
{
    switch (event)
    {

    case SYSTEM_EVENT_STA_GOT_IP:
#ifdef WIFIOZ_OZDEBUG
        OZ_LOGI(WIFITAG, "Connected my IP: %s", WiFi.localIP().toString().c_str());
#endif
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
#ifdef WIFIOZ_OZDEBUG
        OZ_LOGW(WIFITAG, "Disconnected!!");
#endif
        xTimerStart(_wifioz_connection_timer, 0);
        break;

    case SYSTEM_EVENT_SCAN_DONE:
        wifioz_make_STA();
        break;

    case SYSTEM_EVENT_AP_START:

#ifdef WIFIOZ_OZDEBUG
        OZ_LOGI(WIFITAG, "SoftAp on!!  IP: %s", WiFi.softAPIP().toString().c_str());
#endif
        break;

    default:
        break;
    }
}

void wifi_oz::begin(String Espname)
{
    _wifioz_connection_timer = xTimerCreate(
        "wifioz_connection",
        pdMS_TO_TICKS(WIFIOZ_RECONNECTION_DELAY),
        pdFALSE,
        (void *)0,
        wifioz_connect_wifiISR);

    _wifioz_watchdog = xTimerCreate(
        "wifioz_watchdog",
        pdMS_TO_TICKS(WIFIOZ_WATCHDOG_DELAY),
        pdTRUE,
        (void *)0,
        wifioz_WatchdogISR);

    xTimerStart(_wifioz_watchdog, 0);

    espname = Espname;

    WiFi.onEvent(wifioz_event_handler);
    wifioz_connect_wifi();
}

void wifi_oz::begin(String Espname, String SSIDAccessPoint, String PasswordAccessPoint)
{
    _wifioz_ssid_ap = SSIDAccessPoint;
    _wifioz_pass_ap = PasswordAccessPoint;
    this->begin(Espname);
}

void wifi_oz::begin(String Espname, String SSIDAccessPoint, String PasswordAccessPoint, std::map<String, String> NetworkList)
{
    _wifioz_networkList = NetworkList;
    this->begin(Espname, SSIDAccessPoint, PasswordAccessPoint);
}

void wifi_oz::setSSIDAccessPoint(String SSID)
{
    _wifioz_ssid_ap = SSID;
}

void wifi_oz::setPasswordAccessPoint(String Password)
{
    _wifioz_pass_ap = Password;
}

void wifi_oz::addNetwork(String SSID, String Password)
{
    _wifioz_networkList.insert({SSID, Password});
}

String wifi_oz::getAddress()
{
    if (WiFi.isConnected())
        return WiFi.localIP().toString();
    else
        return String();
}

wifi_oz WIFIOZ;