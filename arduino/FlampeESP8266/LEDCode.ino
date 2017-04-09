#include <Ticker.h>
#include "FastLED.h"
#include "textkeys.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define MS_PER_BRIGHTNESS 10
#define MS_PER_COLOR 5
#define DATA_PIN    7
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

Ticker ledTicker;
CRGB *leds;
CRGB *ledsShadow;

#define FRAMES_PER_SECOND  120

#define BRIGHTNESS_MIN  0
#define BRIGHTNESS_MAX  240

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define flmax(a,b) (a>b?a:b)
#define flmin(a,b) (a<b?a:b)

// actual values used with FastLED
int led_actual_brightness = 0;
CRGB led_actual_primaryColor = CRGB::Black;
CRGB led_actual_accentColor = CRGB::Black;
// time stamps when fading brightness, color
uint32_t led_actual_brightness_last = 0;
uint32_t led_actual_color_last = 0;

int led_currentPattern = 3;
int led_patternBeforeMessage = 3;
CRGB led_messageColor;

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { led_message, led_pattern_solid, led_pattern_gradient, loop_led_fire, bpm, juggle, led_pattern_rainbow};

CRGBPalette16 gPal, userPalette, userPalette2, userPalette3;
CRGBPalette16 pallettes[] = {userPalette,userPalette2,userPalette3,HeatColors_p,OceanColors_p,CloudColors_p,ForestColors_p,LavaColors_p,RainbowColors_p,PartyColors_p};

// set during setup_led() which runs after setup_status()
int ledPin;

int ledCount;
int stripCount;
int ledOrientation;
int ledsPerStrip;
bool ledsReverse;

void setup_led() {
  ledPin = setup_ledPin;
  if (ledPin < 1) ledPin = 12;
  
  // set setup values read by setup_status()
  ledOrientation = setup_orientation;
  if (stripCount < 1) { stripCount = 1; }
  ledCount = setup_ledCount;
  if (ledCount < 1) { ledCount = 1; }
  stripCount = setup_stripCount;
  if (stripCount < 1) { stripCount = 1; }
  ledsReverse = setup_ledReverse;
  // calculate constants based on that
  ledsPerStrip = ledCount /  stripCount;
  // initialize ram stuff
  leds = (CRGB*)malloc(ledCount * sizeof(CRGB));
  ledsShadow = (CRGB*)malloc(ledCount * sizeof(CRGB));
  
  delay(1000); // 1 second delay for recovery
  
  // TODO how to configure pin via web ui!?
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(ledsShadow, ledCount).setCorrection(TypicalPixelString);
//  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(ledsShadow, ledCount).setCorrection(TypicalLEDStrip);

  setUserPalette();
  setBrightness(brightness);
  setPalette(currentPalette);
  setAnimation(currentAnimation);
  
  setup_led_fire();

  ledTicker.attach_ms(10, tick_led);
}

int dot = 0;
uint32_t next = 0;

void tick_led() {
  _led_workBrightness();
  _led_workColors();
  gPatterns[led_currentPattern]();
  led_show();
}

inline void led_show() {
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
  if (ledsReverse) {
    CRGB tmp;
    int j;
    for(int i=0;i<ledCount/2;i++) {
      j = ledCount - i - 1;
      tmp = ledsShadow[i];
      ledsShadow[i] = ledsShadow[j];
      ledsShadow[j] = tmp;
    }
  }
}

void _led_workBrightness() {
  if (brightness == led_actual_brightness) {return;}
  uint32_t now = millis();
  if (now - led_actual_brightness_last < MS_PER_BRIGHTNESS ) {return;}
  if (brightness>led_actual_brightness) {
    led_actual_brightness++;
  } else {
    led_actual_brightness--;
  }
  FastLED.setBrightness( led_actual_brightness );
  led_actual_brightness_last = now;
}

void _led_workColors() {
  if (_led_isSameColor(led_actual_primaryColor, primaryColor) &&
      _led_isSameColor(led_actual_accentColor, accentColor)) {return;}
      
  uint32_t now = millis();
  if (now - led_actual_color_last < MS_PER_COLOR ) {return;}
  led_actual_color_last = now;

  _led_adjustColor(&led_actual_primaryColor, &primaryColor);
  _led_adjustColor(&led_actual_accentColor, &accentColor);
  setUserPalette();
}

bool _led_isSameColor(CRGB a, CRGB b) {
  return a.red==b.red && a.green == b.green && a.blue == b.blue;
}

void _led_adjustColor(CRGB* adjust, CRGB* wanted) {
  if (adjust->red < wanted->red) adjust->red++;
  else if (adjust->red > wanted->red) adjust->red--;
  if (adjust->green < wanted->green) adjust->green++;
  else if (adjust->green > wanted->green) adjust->green--;
  if (adjust->blue < wanted->blue) adjust->blue++;
  else if (adjust->blue > wanted->blue) adjust->blue--;
}

void led_message(){

};

void led_showMessage(CRGB color) {
  if (led_patternBeforeMessage != 0) {
    led_patternBeforeMessage = led_currentPattern;
  }
  led_messageColor = color;
  led_currentPattern = 0;
  int i;
  for(i=0;i<ledCount;i++) {
    leds[i] = color;
  }
  led_show();
}

void led_hideMessage() {
  led_currentPattern = led_patternBeforeMessage;
}

void adjustBrightness(int direction) {
  setBrightness(brightness + direction);

  JsonPush push;
  push.data()[JS_brightness] = brightness;
  push.broadcast();
}

void setBrightness(int value) {
  brightness = flmin(BRIGHTNESS_MAX, flmax(BRIGHTNESS_MIN, value));
}

int led_getBrightness() {
  return brightness;
}

void adjustPalette(int direction) {
  setPalette(currentPalette + direction);

  JsonPush push;
  push.data().createNestedObject(JS_animation)[JS_currentPalette] = paletteTextkeys[currentPalette];
  push.broadcast();
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
  currentPalette = (index + ARRAY_SIZE( pallettes)) % ARRAY_SIZE( pallettes);
  gPal = pallettes[currentPalette];
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
  userPalette = CRGBPalette16( CRGB::Black, led_actual_primaryColor, led_actual_accentColor);
  userPalette2 = CRGBPalette16( CRGB::Black, led_actual_primaryColor, led_actual_accentColor, CRGB::White);
  userPalette3 = CRGBPalette16( led_actual_accentColor, CRGB::Black, CRGB::White,  led_actual_primaryColor);
  pallettes[0] = userPalette;
  pallettes[1] = userPalette2;
  pallettes[2] = userPalette3;
  if (currentPalette >= 0 && currentPalette <= 2) {
    setPalette(currentPalette);
  }
}

void adjustAnimation(int direction) {
  // reduced by 1 for the message animation
  const int len = ARRAY_SIZE( gPatterns) - 1;
  
  int nindex = (len + direction + currentAnimation) % len;
  setAnimation(nindex);
  JsonPush push;
  push.data().createNestedObject(JS_animation)[JS_currentAnimation] = animationTextkeys[currentAnimation];
  push.broadcast();
}

void setAnimation(int index) {
  currentAnimation = index;
  // 0 is message display
  led_currentPattern = index + 1;
}

void setIdleAnimation(int index) {
  idleAnimation = index;
}

