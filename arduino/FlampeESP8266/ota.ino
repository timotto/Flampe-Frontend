#include <ArduinoOTA.h>

int ota_brightnessBefore;

void setup_ota(){
  ArduinoOTA.begin();
}

void loop_ota(){
  ArduinoOTA.setRebootOnSuccess(false);
  
  ArduinoOTA.onStart([]() {
    status_writeEnabled = false;
    ota_brightnessBefore = led_getBrightness();
    setBrightness(10);
    led_showMessage(CRGB::Blue);
    delay(100);
    if(ArduinoOTA.getCommand() == U_SPIFFS) {
      led_showMessage(CRGB::Yellow);
      SPIFFS.end();
    }
  });

  ArduinoOTA.onEnd([]() {
    led_showMessage(CRGB::Green);
    if(ArduinoOTA.getCommand() == U_SPIFFS) {
      status_writeEnabled = true;
      SPIFFS.begin();
      setBrightness(ota_brightnessBefore);
      led_hideMessage();
      save_status();
      Serial.println("SPIFFS update complete - resuming");
    } else {
      Serial.println("runtime update complete - restarting");
      delay(100);
      ESP.restart();
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    led_showMessage(CRGB::Red);
    ESP.restart();
  });

  ArduinoOTA.handle();
}

