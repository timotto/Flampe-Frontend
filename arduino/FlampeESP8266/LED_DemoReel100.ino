uint32_t led_pattern_nextTime = 0;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

bool led_pattern_thisTime() {
  const uint32_t now = millis();
  if (now < led_pattern_nextTime) {return false;}
  // 100 fps
  led_pattern_nextTime = now + 10;
  return true;
}

void led_pattern_solid(){
  if(!led_pattern_thisTime) { return; }
  
  for(int i=0;i<ledCount;i++) {
    leds[i] = primaryColor;
  }
}

void led_pattern_gradient(){}

int led_pattern_rainbowStep = 0;
void led_pattern_rainbow(){
  if(++led_pattern_rainbowStep == 10) {
    led_pattern_rainbowStep = 0;
    gHue--;
  }
  fill_rainbow( leds, ledCount, gHue, 1);
}

void rainbowWithGlitter(){};
void confetti(){};
void sinelon(){};
void juggle(){};
void bpm(){};


