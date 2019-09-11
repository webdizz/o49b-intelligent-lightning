#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

struct WiFiConnection
{
    const char *ssid;
    const char *password;
};

bool loadConfig(WiFiConnection &wiFiConnection);