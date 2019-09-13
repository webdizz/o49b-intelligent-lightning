#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include "WiFiCfg.h"
#include "FS.h"

struct WiFiConnection wiFiConnection;

const int PIN_BACK_WALL = 14;     //D5
const int PIN_FRONT_DOOR = 12;    //D6
const int PIN_YARD_FENCING_R = 5; //D1
const int PIN_YARD_FENCING_L = 4; //D2

#if defined(LED_BUILTIN)
const int ledPin = LED_BUILTIN;
#else
const int ledPin = 13; // manually configure LED pin
#endif

WebThingAdapter *adapter;

const char *ledTypes[] = {"OnOffSwitch", "Light", nullptr};
ThingDevice led("outdoorLightningController", "Built-in LED Outdoor Lightning", ledTypes);
ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");

ThingDevice backWall("backWall", "Back Wall Light", ledTypes);
ThingProperty backWallOn("on", "", BOOLEAN, "OnOffProperty");

ThingDevice frontDoor("frontDoor", "Front Door Light", ledTypes);
ThingProperty frontDoorOn("on", "", BOOLEAN, "OnOffProperty");

ThingDevice yardFencingR("yardFencingR", "Yard Fencing Light Right side", ledTypes);
ThingProperty yardFencingROn("on", "", BOOLEAN, "OnOffProperty");
ThingDevice yardFencingL("yardFencingL", "Yard Fencing Light Left side", ledTypes);
ThingProperty yardFencingLOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void setup(void)
{
  Serial.begin(115200);

  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }
  if (!loadConfig(wiFiConnection))
  {
    Serial.println("Failed to load config");
  }
  else
  {
    Serial.println("Config loaded");
  }
  delay(500);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  pinMode(PIN_BACK_WALL, OUTPUT);
  digitalWrite(PIN_BACK_WALL, HIGH);

  pinMode(PIN_FRONT_DOOR, OUTPUT);
  digitalWrite(PIN_FRONT_DOOR, HIGH);

  pinMode(PIN_YARD_FENCING_R, OUTPUT);
  digitalWrite(PIN_YARD_FENCING_R, HIGH);
  pinMode(PIN_YARD_FENCING_L, OUTPUT);
  digitalWrite(PIN_YARD_FENCING_L, HIGH);

  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(wiFiConnection.ssid);
  Serial.println("\"");

#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(wiFiConnection.ssid, wiFiConnection.password);

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, blink ? LOW : HIGH);
    Serial.println("Is about to connect to WiFi");
    blink = !blink;
  }
  digitalWrite(ledPin, HIGH);

  // setup lightning controlling
  adapter = new WebThingAdapter("OutdoorLightning", WiFi.localIP());

  led.addProperty(&ledOn);
  adapter->addDevice(&led);

  backWall.addProperty(&backWallOn);
  adapter->addDevice(&backWall);

  frontDoor.addProperty(&frontDoorOn);
  adapter->addDevice(&frontDoor);

  yardFencingR.addProperty(&yardFencingROn);
  adapter->addDevice(&yardFencingR);
  yardFencingL.addProperty(&yardFencingLOn);
  adapter->addDevice(&yardFencingL);

  adapter->begin();

  // http://$IP/things/$THING_ID
}

void loop(void)
{
  adapter->update();

  bool on = ledOn.getValue().boolean;
  digitalWrite(ledPin, on ? LOW : HIGH);

  bool isBackWallOn = backWallOn.getValue().boolean;
  digitalWrite(PIN_BACK_WALL, isBackWallOn ? LOW : HIGH);

  bool isFrontDoorOn = frontDoorOn.getValue().boolean;
  digitalWrite(PIN_FRONT_DOOR, isFrontDoorOn ? LOW : HIGH);

  bool isYardFencingROn = yardFencingROn.getValue().boolean;
  digitalWrite(PIN_YARD_FENCING_R, isYardFencingROn ? LOW : HIGH);
  bool isYardFencingLOn = yardFencingLOn.getValue().boolean;
  digitalWrite(PIN_YARD_FENCING_L, isYardFencingLOn ? LOW : HIGH);
}