#include <WebSocketsServer.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
// because auf color types
#include <FastLED.h>

char FLAMPE_ID[15];

const char* www_username = "admin";
const char* www_password = "esp8266";

int setup_ledPin;
int setup_ledCount;
bool setup_ledReverse;
int setup_orientation;
int setup_stripCount;

int brightness = 0;
CRGB primaryColor = CRGB(0,0,0);
CRGB accentColor = CRGB(0,0,0);
int currentAnimation;
int currentPalette;
int idleAnimation;
int idlePalette;
int idleBrightness = 0;
int idleTimeout = 0;

char mqtt_server[40];
int mqtt_port = 1883;
char mqtt_user[40];
char mqtt_password[40];
char mqtt_intopic[40];
char mqtt_outtopic[40];
char mqtt_devicename[40];
bool mqtt_listen_commands;
bool mqtt_publish_state;
bool mqtt_publish_gesture;
char wifi_ssid[40];
char wifi_password[40];
char hotspot_ssid[40];
char hotspot_password[40];

void setup() {
  sprintf(FLAMPE_ID, "flampe-%06x", ESP.getChipId());

  Serial.begin(115200);
  SPIFFS.begin();
  setup_status();
  setup_wifi();
  setup_http();
  setup_ota();
  setup_websocket();
  setup_mqtt();
  setup_led();
  setup_sensor();
  Serial.println("Started!");
}

void loop() {
  loop_wifi();
  loop_mqtt();
  loop_websocket();
  loop_http();
  loop_ota();
  loop_led();
  loop_sensor();
}


