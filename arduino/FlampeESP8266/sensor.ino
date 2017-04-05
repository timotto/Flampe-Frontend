#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT    D6
#define APDS9960_SDA    D4
#define APDS9960_SCL    D5

// Constants
#define PROXIMITY_LCHECK_TIME  800 // Wait period (ms) between proximity reads
#define PROXIMITY_HCHECK_TIME  100 // Wait period (ms) between proximity reads
#define PROXIMITY_THROUT       10
#define PROXIMITY_THRIN        20
#define PROXIMITY_THRMAX      255
#define SENSOR_MODE_GESTURE     0
#define SENSOR_MODE_PROXIMITY   1

SparkFun_APDS9960 apds = SparkFun_APDS9960();

// set by isr
volatile bool isr_flag = 0;
// set when processing the even and the result is DIR_AGAIN because of inner state machine timeout
bool again_flag = false;
uint8_t proximity_data = 0;
uint8_t sensorMode = 0; // 0 gesture, 1 proximity

uint32_t nextProximityTime = 0;
uint32_t lastProximityUseTime = 0;
uint32_t sensor_proximityHighFreqStartTime = 0;
int16_t sensor_proximityDeltaValueDelayed = 0;

uint8_t sensor_proximityEntryValue = 0;
bool sensor_proximityHighFrequency = false;
uint8_t sensor_proximityAveraged;
bool sensor_proximityDeltaValueBase = false;

int8_t sensor_onoff_lastBrightnessValue = 50;

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
  
  // Start running the APDS-9960 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
    Serial.println(F("Proximity sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }

  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

void loop_sensor(){
  
  uint32_t now = millis();
  switch(sensorMode) {
    case SENSOR_MODE_GESTURE:
      if((isr_flag || again_flag)) {
        sensor_handleGesture();
        isr_flag = 0;
      } else {
        // enters proximity mode from method itself
        sensor_handleProximity();
      }
      break;
    case SENSOR_MODE_PROXIMITY:
      // exits proximity mode from method itself
      sensor_handleProximity();
      break;
  }
}

void sensor_interruptRoutine() {
  isr_flag = 1;
}

void sensor_handleGesture() {
  // only true on UDLR
  bool stopSensor_proximityHighFrequency = true;
  if ( apds.isGestureAvailable() ) {
    again_flag = false;
    const int16_t gesture = apds.readGesture();
    t_sensor_gesture_function f = 0;
    int d = 0;
    switch (gesture) {
      case DIR_UP:
        Serial.println("UP");
        f = sensor_gesture_y;
        d = sensor_gesture_y_direction;
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        f = sensor_gesture_y;
        d = -1*sensor_gesture_y_direction;
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        f = sensor_gesture_x;
        d = -1*sensor_gesture_x_direction;
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        f = sensor_gesture_x;
        d = sensor_gesture_x_direction;
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        break;
      case DIR_AGAIN:
        again_flag = true;
        stopSensor_proximityHighFrequency = false;
        break;
      case DIR_TIMEOUT:
        stopSensor_proximityHighFrequency = false;
        break;
      default:
        Serial.println("NONE");
        break;
    }

    if (f != 0) {
      Serial.printf("Running f() %d\n", f);
      f(d);
    }

    if (stopSensor_proximityHighFrequency) {
      sensor_proximityHighFrequency = false;
    }
  }
}

/**
 * one of the possible values next to adjustBrightness and adjustPalette for sensor_gesture_{x,y}
 */
void sensor_toggleOnOff(int directionForSignature) {
  if (led_actual_brightness == 0 && brightness == 0) {
    // on
    setBrightness(sensor_onoff_lastBrightnessValue);
  } else if (led_actual_brightness > 0 && brightness > 0) {
    // off
    sensor_onoff_lastBrightnessValue = brightness;
    setBrightness(0);
  }
}

void sensor_handleProximity() {
  if (!sensor_enabled_proximity) {
    return;
  }
  uint32_t now = millis();
  uint8_t proximity_data;
  if (now < nextProximityTime) {
    return;
  }
  // decide if high or low frequency measurement
  nextProximityTime = now + sensor_proximityHighFrequency
    ?PROXIMITY_HCHECK_TIME
    :PROXIMITY_LCHECK_TIME;
  
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println("Error reading proximity value");
    return;
  }
  
  // sensor read is valid
  
  if (proximity_data < PROXIMITY_THROUT) {
    // value out of range, at the highest distance

    sensor_proximityHighFrequency = false;
    if (sensorMode == SENSOR_MODE_PROXIMITY) {
      sensor_switchProximityToGesture();
    }
  } else if (proximity_data > PROXIMITY_THRIN) {
    // value above threashold is valid

    // proximity measurement is filtered
    if (sensor_proximityHighFrequency) {
      // measurement has high frequency, average
      sensor_proximityAveraged = 0.9 * sensor_proximityAveraged + 0.1 * proximity_data;
    } else {
      // overwrite averaged value with most recent data to be prepared when high frequency measurement is on
      sensor_proximityAveraged = proximity_data;
    }
    
    if (sensorMode == SENSOR_MODE_GESTURE) {
      if (!sensor_proximityHighFrequency) {
        // step 1/3: not in high frequency mode yet, switch on
        sensor_proximityHighFreqStartTime = now;
        sensor_proximityHighFrequency = true;
      } else if ((now - sensor_proximityHighFreqStartTime) > 500) {
        // step 3/3: mode switch to proximity, drop gesture detection
        sensor_switchGestureToProximity();
        // set to now, so a delay is achieved for the first action
        lastProximityUseTime = now;
        // no delta yet
        sensor_proximityDeltaValueDelayed = 0;
        // save averaged value as delta base as next delayed action
        sensor_proximityDeltaValueBase = false;
//          } else {
          // step 2/3: deadband: value is already averaged as long as high frequency is on, but gesture isn't turned off yet. nothing to do.
      }
    } else {
      if (now - lastProximityUseTime > 200) {
        lastProximityUseTime  = now;
        if (!sensor_proximityDeltaValueBase) {
          sensor_proximityDeltaValueBase = true;
          sensor_proximityEntryValue = sensor_proximityAveraged;
        }
        Serial.printf("Proximity: %d delta: %d\n",sensor_proximityAveraged,sensor_proximityDeltaValueDelayed);
        sensor_proximityDeltaValueDelayed = sensor_proximityEntryValue - sensor_proximityAveraged;
        adjustBrightness(sensor_proximityDeltaValueDelayed / 16);
      }
    }
  //} else {
    // dead band
  }
}

void sensor_switchGestureToProximity() {

  detachInterrupt(APDS9960_INT);

  if (apds.disableGestureSensor()) {
    Serial.println(F("Gesture sensor is now stopped"));
  } else {
    Serial.println(F("Something went wrong during sensor shutdown!"));
  }
  
  sensorMode = 1;
  
  attachInterrupt(APDS9960_INT, sensor_interruptRoutine, FALLING);
}

void sensor_switchProximityToGesture() {

  detachInterrupt(APDS9960_INT);

  // Start running the APDS-9960 gesture sensor engine (interrupts)
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  sensorMode = 0;

  attachInterrupt(APDS9960_INT, sensor_interruptRoutine, FALLING);
}


