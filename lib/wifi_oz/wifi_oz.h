#ifndef WIFI_OZ_H
#define WIFI_OZ_H

#define WIFITAG "WIFI"

#include <map>
#include <WString.h>

#define WIFIOZ_RECONNECTION_DELAY  2000   // Set in ms
#define WIFIOZ_WATCHDOG_DELAY      60000  // Set in ms

#define WIFIOZ_OZDEBUG


class wifi_oz
{
    public:
        void begin(String espname);
        void begin(String espname, String SSIDAccessPoint,String PasswordAccessPoint);
        void begin(String espname, String SSIDAccessPoint,String PasswordAccessPoint,std::map<String,String> NetworkList);
        void setSSIDAccessPoint(String SSID);
        void setPasswordAccessPoint(String Password);
        void addNetwork(String SSID, String Password);
        String getAddress();

};

extern wifi_oz WIFIOZ;

#endif