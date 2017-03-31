#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT    D6
#define APDS9960_SDA    D4
#define APDS9960_SCL    D5

SparkFun_APDS9960 apds = SparkFun_APDS9960();

volatile bool isr_flag = 0;

void setup_sensor(){

  //Start I2C with pins defined above
  Wire.begin(APDS9960_SDA,APDS9960_SCL);

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);
  
  // Initialize interrupt service routine
  attachInterrupt(APDS9960_INT, sensor_interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

void loop_sensor(){
  if( isr_flag == 1 ) {
    detachInterrupt(APDS9960_INT);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(APDS9960_INT, sensor_interruptRoutine, FALLING);
  }
}

void sensor_interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        adjustPalette(1);
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        adjustPalette(-1);
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        adjustPalette(-1);
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        adjustPalette(1);
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        break;
      default:
        Serial.println("NONE");
    }
  }
}
