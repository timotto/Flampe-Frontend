uint32_t led_pattern_nextTime = 0;

int led_hue_advanceStep= 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

bool led_pattern_thisTime() {
  const uint32_t now = millis();
  if (now < led_pattern_nextTime) {return false;}
  // 100 fps
  led_pattern_nextTime = now + 10;
  return true;
}

void led_advance_hue(int x) {
  if(++led_hue_advanceStep >= x) {
    led_hue_advanceStep = 0;
    gHue--;
  }
}

void led_pattern_solid(){
  if(!led_pattern_thisTime) { return; }
  
  for(int i=0;i<ledCount;i++) {
    leds[i] = led_actual_primaryColor;
  }
}

void led_pattern_gradient(){
  if(!led_pattern_thisTime) { return; }

  CHSV primaryHsv = rgb2hsv_approximate(led_actual_primaryColor);
  CHSV accentHsv = rgb2hsv_approximate(led_actual_accentColor);
  fill_gradient(leds, ledCount, primaryHsv, accentHsv);
}

void led_pattern_rainbow(){
  if(!led_pattern_thisTime) { return; }

  led_advance_hue(10);
  
  fill_rainbow( leds, ledCount, gHue, 1);
}

void juggle(){
  if(!led_pattern_thisTime) { return; }

  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, ledCount, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,ledCount)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void bpm(){
  if(!led_pattern_thisTime) { return; }

  led_advance_hue(10);

  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  uint8_t beat = beatsin8( BeatsPerMinute, 0, 255);
  for( int i = 0; i < ledCount; i++) { //9948
    leds[i] = ColorFromPalette(gPal, gHue+(i*2), beat-gHue+(i*10));
  }
}


