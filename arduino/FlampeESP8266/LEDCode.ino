#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    114
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup_led() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  setup_led_fire();
}

int dot = 0;
uint32_t next = 0;

void loop_led() {
  loop_led_fire();
//  uint32_t now = millis();
//  if (now >= next) {
//    leds[dot] = CRGB::White;
//    FastLED.show();
//    // clear this led for the next time around the loop
//    leds[dot] = CRGB::Black;
//    dot = (dot + 1) % NUM_LEDS;
//    next = now + 10;
//  }
}

