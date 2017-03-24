#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup_wifi() {
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
  WiFiManagerParameter custom_mqtt_user("mqtt_user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("mqtt_password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_wifi_ssid("wifi_ssid", "wifi ssid", wifi_ssid, 40);
  WiFiManagerParameter custom_wifi_password("wifi_password", "wifi password", wifi_password, 40);
  WiFiManagerParameter custom_hotspot_ssid("hotspot_ssid", "hotspot ssid", hotspot_ssid, 40);
  WiFiManagerParameter custom_hotspot_password("hotspot_password", "hotspot password", hotspot_password, 40);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.addParameter(&custom_wifi_ssid);
  wifiManager.addParameter(&custom_wifi_password);
  wifiManager.addParameter(&custom_hotspot_ssid);
  wifiManager.addParameter(&custom_hotspot_password);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "Flampe"
  //and goes into a blocking loop awaiting configuration
//  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
  if (!wifiManager.autoConnect("Flampe")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_password, custom_mqtt_password.getValue());
  strcpy(wifi_ssid, custom_wifi_ssid.getValue());
  strcpy(wifi_password, custom_wifi_password.getValue());
  strcpy(hotspot_ssid, custom_hotspot_ssid.getValue());
  strcpy(hotspot_password, custom_hotspot_password.getValue());

  if (shouldSaveConfig) {
    save_config();
  }
}

