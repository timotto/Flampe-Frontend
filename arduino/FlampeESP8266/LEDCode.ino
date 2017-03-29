#include "FastLED.h"

void setBrightness(int value, bool updateHardware = true);

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
//#define NUM_LEDS    24
//#define LED_STRIPS      3

//CRGB leds[NUM_LEDS];
//CRGB ledsShadow[NUM_LEDS];
CRGB *leds;
CRGB *ledsShadow;

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

#define BRIGHTNESS_MIN  0
#define BRIGHTNESS_MAX  240

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define flmax(a,b) (a>b?a:b)
#define flmin(a,b) (a<b?a:b)

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { loop_led_fire, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
void rainbow(){};
void rainbowWithGlitter(){};
void confetti(){};
void sinelon(){};
void juggle(){};
void bpm(){};

CRGBPalette16 gPal, userPalette, userPalette2, userPalette3;
CRGBPalette16 pallettes[] = {userPalette,userPalette2,userPalette3,HeatColors_p,OceanColors_p,CloudColors_p,ForestColors_p,LavaColors_p,RainbowColors_p,PartyColors_p};

// set during setup_led() which runs after setup_status()
int ledCount;
int stripCount;
int ledOrientation;
int ledsPerStrip;

void setup_led() {
  // set setup values read by setup_status()
  ledOrientation = setup_orientation;
  if (stripCount < 1) { stripCount = 1; }
  ledCount = setup_ledCount;
  if (ledCount < 1) { ledCount = 1; }
  stripCount = setup_stripCount;
  if (stripCount < 1) { stripCount = 1; }
  // calculate constants based on that
  ledsPerStrip = ledCount /  stripCount;
  // initialize ram stuff
  leds = (CRGB*)malloc(ledCount * sizeof(CRGB));
  ledsShadow = (CRGB*)malloc(ledCount * sizeof(CRGB));
  
  delay(1000); // 1 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(ledsShadow, ledCount).setCorrection(TypicalLEDStrip);

  setUserPalette();
  setBrightness(brightness);
  setPalette(currentPalette);
  setAnimation(currentAnimation);
  
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
  remap_leds();
  FastLED.show();
}

inline void remap_leds() {
  switch(ledOrientation) {
    case 0:
      // strip
      memcpy(ledsShadow, leds, ledCount * sizeof(CRGB));
      break;
    case 1: {
      // zigzag
      for(int led=0;led<ledCount;led++) {
        // x axis
        const int strip = led % stripCount;
        // position on the y axis
        const int y = led / stripCount;
        // from or towards origin
        const int offset = strip%2==0
          // from origin for even
          ?y
          // odd strips go towards origin, and back to index 0 based
          :ledsPerStrip-y-1;
        const int destIndex = (strip * ledsPerStrip) + offset;
        ledsShadow[destIndex] = leds[led];
      }
      break; }
    case 2:
      // spiral
      memcpy(ledsShadow, leds, ledCount * sizeof(CRGB));
      break;
  }
}

void adjustBrightness(int direction) {
  setBrightness(brightness + direction);
}

void setBrightness(int value, bool updateHardware) {
  brightness = flmin(BRIGHTNESS_MAX, flmax(BRIGHTNESS_MIN, value));
  if (updateHardware) {
    FastLED.setBrightness( brightness );
    Serial.print("Brightness: ");
    Serial.println(brightness);
  }
}

void adjustPalette(int direction) {
  setPalette(currentPalette + direction);
}

void setIdlePalette(int index) {
  idlePalette = flmin(ARRAY_SIZE( pallettes), flmax(0, index));
}

void setPalette(int index) {
  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  
  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  
  // Third, here's a simpler, three-step gradient, from black to red to white
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  currentPalette = flmin(ARRAY_SIZE( pallettes), flmax(0, index));
  gPal = pallettes[currentPalette];
  Serial.print("Palette #: ");
  Serial.println(currentPalette);
}

void setPrimaryColor(uint8_t red, uint8_t green, uint8_t blue) {
  primaryColor.red = red;
  primaryColor.green = green;
  primaryColor.blue = blue;
  setUserPalette();
}

void setAccentColor(uint8_t red, uint8_t green, uint8_t blue) {
  accentColor.red = red;
  accentColor.green = green;
  accentColor.blue = blue;
  setUserPalette();
}

void setUserPalette() {
  userPalette = CRGBPalette16( CRGB::Black, primaryColor, accentColor);
  userPalette2 = CRGBPalette16( CRGB::Black, primaryColor, accentColor, CRGB::White);
  userPalette3 = CRGBPalette16( accentColor, CRGB::Black, CRGB::White,  primaryColor);
  pallettes[0] = userPalette;
  pallettes[1] = userPalette2;
  pallettes[2] = userPalette3;
  if (currentPalette >= 0 && currentPalette <= 2) {
    setPalette(currentPalette);
  }
}

void setAnimation(int index) {
  currentAnimation = index;
}
void setIdleAnimation(int index) {
  idleAnimation = index;
}

