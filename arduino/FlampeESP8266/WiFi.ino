#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266mDNS.h>

int wifiConnectionFails = 0;

int wifi_apClientsConnected = 0;
bool wifi_apClientsConnectedChanged = false;

void setup_wifi() {
  WiFi.onEvent(WiFiEvent);
  reconfigureWifi();

  char tmp[15];
  sprintf(tmp, "flampe-%06x", ESP.getChipId());
    
  if(MDNS.begin(tmp)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
  }
}

void loop_wifi() {
  if (wifi_apClientsConnectedChanged) {
    wifi_apClientsConnectedChanged = false;
    // TODO send status update push
  }
}

void WiFiEvent(WiFiEvent_t event) {
  
  switch(event) {
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      wifi_apClientsConnected++;
      wifi_apClientsConnectedChanged = true;
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      wifi_apClientsConnected--;
      wifi_apClientsConnectedChanged = true;
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      break;
    case WIFI_EVENT_STAMODE_CONNECTED:
      // almost there
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:
      wifiConnectionFails = 0;
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      // TODO turn off AP with gracetime
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      if (wifiConnectionFails++ > 0) {
        // know already
        if (wifiConnectionFails == 3) {
          // cancel with third attempt
          WiFi.disconnect();
          // TODO retry later
        }
        return;
      }
      Serial.println("WiFi lost connection");
      // TODO turn on AP with gracetime
      break;
    default:
      Serial.printf("[WiFi-event] event: %d\n", event);
      break;
  }
}

void setWifiSsid(const char* value) {
  strncpy(wifi_ssid, value, sizeof(wifi_ssid));
}

void setWifiPassword(const char* value) {
  strncpy(wifi_password, value, sizeof(wifi_password));
}

void setHotspotSsid(const char* value) {
  strncpy(hotspot_ssid, value, sizeof(hotspot_ssid));
}

void setHotspotPassword(const char* value) {
  strncpy(hotspot_password, value, sizeof(hotspot_password));
}

void wifiReconnect(){
  reconfigureWifi();
}

void hotspotReconnect(){
  reconfigureWifi();
}

void reconfigureWifi() {
  bool enableSta = false;
  bool enableAp = false;
  if(strlen(wifi_ssid) > 0) {
    enableSta = true;
  } else {
    if (WiFi.isConnected()) {
      WiFi.disconnect();
    Serial.println("WiFi: Sta: disabled");
    }
  }
  
  if(strlen(hotspot_ssid) > 0) {
    enableAp = true;
  } else {
    WiFi.softAPdisconnect();
    Serial.println("WiFi: Ap: disabled");
  }
  
  if(enableSta && enableAp) {
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WiFi: AP & STA");
  } else if (enableSta) {
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi: STA");
  } else if (enableAp) {
    WiFi.mode(WIFI_AP);
    Serial.println("WiFi: AP");
  } else {
    WiFi.disconnect(true);
    WiFi.softAPdisconnect(true);
    Serial.println("WiFi: disabled");
    return;
  }

  if (enableSta) {
    String ssStr = wifi_ssid;
    if (strlen(wifi_password) > 0) {
      String pwStr = wifi_password;
      if(!(pwStr == WiFi.psk() && ssStr == WiFi.SSID())) {
        WiFi.begin(wifi_ssid, wifi_password);
        Serial.print("WiFi: Sta: SSID or PSK changed");
        Serial.print(" SSID [");
        Serial.print(wifi_ssid);
        Serial.print("] and PSK [");
        Serial.print(wifi_password);
        Serial.println("]");
      } else {
        WiFi.begin();
        Serial.print("WiFi: Sta: SSID and PSK are unchanged: ");
        Serial.print(" SSID [");
        Serial.print(wifi_ssid);
        Serial.print("] and PSK [");
        Serial.print(wifi_password);
        Serial.println("]");
      }
    } else {
      if(ssStr != WiFi.SSID()) {
        WiFi.begin(wifi_ssid);
        Serial.print("WiFi: Sta: SSID changed");
        Serial.print(" SSID [");
        Serial.print(wifi_ssid);
        Serial.println("]");
      } else {
        WiFi.begin();
        Serial.print("WiFi: Sta: SSID is unchanged");
        Serial.print(" SSID [");
        Serial.print(wifi_ssid);
        Serial.println("]");
      }
    }
  }
  if (enableAp) {
    if (strlen(hotspot_password) > 0) {
      WiFi.softAP(hotspot_ssid, hotspot_password);
      Serial.print("WiFi: Ap: SSID [");
      Serial.print(hotspot_ssid);
      Serial.print("] and PSK [");
      Serial.print(hotspot_password);
      Serial.println("]");
    } else {
      WiFi.softAP(hotspot_ssid);
      Serial.print("WiFi: Ap: SSID [");
      Serial.print(hotspot_ssid);
      Serial.println("]");
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
}

