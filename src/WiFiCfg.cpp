#include <WiFiCfg.h>

#include "FS.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

void populateWiFiConnection(WiFiConnection &wiFiConnection, JsonObject &json)
{
    // Copy values from the JSON document
    wiFiConnection.ssid = json["wifi"]["ssid"].as<char *>();
    wiFiConnection.password = json["wifi"]["password"].as<char *>();
}

bool loadConfig(WiFiConnection &wiFiConnection)
{
    const char *filename = "/config.json";
    File configFile = SPIFFS.open(filename, "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file");
        return false;
    }

    // Deserialize the JSON document
    StaticJsonBuffer<512> jb;
    JsonObject &json = jb.parseObject(configFile);

    if (!json.success())
    {
        Serial.println(F("Failed to read configuration file, using default configuration"));
    }

    populateWiFiConnection(wiFiConnection, json);

    return true;
}