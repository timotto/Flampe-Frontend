#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include "textkeys.h"

bool status_writeEnabled = true;

void apply_json_status(JsonObject& root, bool applyAndSave = true);

void setup_status() {
  bool readStatusObject = false;
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/status.json")) {
      //file exists, reading and loading
      Serial.println("reading status file");
      File configFile = SPIFFS.open("/status.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer statusJsonBuffer;
        JsonObject& json = statusJsonBuffer.parseObject(buf.get());
        json.prettyPrintTo(Serial);
        Serial.println();
        if (json.success()) {
          // don't update hardware, reconnect mqtt. this step happens before setup of the other systems
          apply_json_status(json, false);
          readStatusObject = true;
          Serial.println("\nparsed json");
        } else {
          Serial.println("failed to load json config");
        }
      } else {
        Serial.println("Cannot open file");
      }
    } else {
      Serial.println("Config file does not exist");
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  if(!readStatusObject) {
    create_default_status();
  }
}

void create_default_status() {
  setup_ledPin = 12;
  setup_ledCount = 1;
  setup_stripCount = 1;
  setup_orientation = 0;
  setup_ledReverse = false;
  
  brightness = 0;
  primaryColor = CRGB(123,140,42);
  accentColor = CRGB(223,190,242);

  currentAnimation = 0;
  idleAnimation = 0;
  currentPalette = 0;
  idlePalette = 0;
  idleBrightness = 0;
  idleTimeout = 0;
  
  strncpy(hotspot_ssid, "Flampe", sizeof(hotspot_ssid));
  strncpy(hotspot_password, "password", sizeof(hotspot_password));
  strncpy(wifi_ssid, "", sizeof(hotspot_ssid));
  strncpy(wifi_password, "", sizeof(hotspot_password));
  
  strncpy(mqtt_server, "", sizeof(mqtt_server));
  mqtt_port = 1883;
  strncpy(mqtt_user, "", sizeof(mqtt_user));
  strncpy(mqtt_password, "", sizeof(mqtt_password));
  strncpy(mqtt_intopic, "", sizeof(mqtt_intopic));
  strncpy(mqtt_outtopic, "", sizeof(mqtt_outtopic));
  strncpy(mqtt_devicename, "flampe", sizeof(mqtt_devicename));
  mqtt_listen_commands = false;
  mqtt_publish_state = false;
  mqtt_publish_gesture = false;
}

void save_status() {
  if (!status_writeEnabled) {
    return;
  }
  Serial.println("saving status");
  File configFile = SPIFFS.open("/status.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  DynamicJsonBuffer statusJsonBuffer;
  JsonObject& jsonStatus = statusJsonBuffer.createObject();
  create_status_object(jsonStatus);
  status_cleanupJsonData(jsonStatus, false, true);
  jsonStatus.printTo(configFile);
  configFile.close();
  //end save
}

void dump_json_color(JsonObject& jsonColor, CRGB* color) {
  jsonColor[TK_colorRed] = color->red;
  jsonColor[TK_colorGreen] = color->green;
  jsonColor[TK_colorBlue] = color->blue;
}

void create_status_object(JsonObject& jsonStatus) {
  JsonObject& setup = jsonStatus.createNestedObject("setup");
  setup[JS_ledpin] = setup_ledCount;
  setup[JS_ledcount] = setup_ledCount;
  setup[JS_stripcount] = setup_stripCount;
  setup[JS_reverse] = setup_ledReverse;
  switch(setup_orientation) {
    case 0: setup[JS_orientation] = JS_orientationStrip; break;
    case 1: setup[JS_orientation] = JS_orientationZigzag; break;
    case 2: setup[JS_orientation] = JS_orientationSpiral; break;
  }
  
  jsonStatus["brightness"] = brightness;
  
  JsonObject& primary = jsonStatus.createNestedObject("primary");
  dump_json_color(primary, &primaryColor);

  JsonObject& accent = jsonStatus.createNestedObject("accent");
  dump_json_color(accent, &accentColor);

  JsonObject& wifi = jsonStatus.createNestedObject("wifi");
  wifi["ssid"] = wifi_ssid;
  wifi["password"] = wifi_password;
  
  JsonObject& hotspot = jsonStatus.createNestedObject("hotspot");
  hotspot["ssid"] = hotspot_ssid;
  hotspot["password"] = hotspot_password;
  
  JsonObject& mqtt = jsonStatus.createNestedObject("mqtt");
  mqtt["host"] = mqtt_server;
  mqtt["port"] = mqtt_port;
  mqtt["login"] = mqtt_user;
  mqtt["password"] = mqtt_password;
  mqtt["intopic"] = mqtt_intopic;
  mqtt["outtopic"] = mqtt_outtopic;
  mqtt["devicename"] = mqtt_devicename;
  JsonObject &mqttListen = mqtt.createNestedObject("listen");
  mqttListen["commands"] = mqtt_listen_commands;
  JsonObject &mqttPublish = mqtt.createNestedObject("publish");
  mqttPublish["state"] = mqtt_publish_state;
  mqttPublish["gesture"] = mqtt_publish_gesture;

  JsonObject& animation = jsonStatus.createNestedObject("animation");
  animation["currentAnimation"] = animationTextkeys[currentAnimation];
  animation["idleAnimation"] = animationTextkeys[idleAnimation];
  animation[JS_currentPalette] = paletteTextkeys[currentPalette];
  animation["idlePalette"] = paletteTextkeys[idlePalette];
  animation["idleBrightness"] = idleBrightness;
  animation["idleTimeout"] = idleTimeout;
  
  JsonObject& animations = animation.createNestedObject("animations");
  for(int i=0;i<ARRAY_SIZE(animationTextkeys);i++) {
    animations.createNestedObject(animationTextkeys[i]);
  }
  JsonObject& palettes = animation.createNestedObject("palettes");
  for(int i=0;i<ARRAY_SIZE(paletteTextkeys);i++) {
    palettes.createNestedObject(paletteTextkeys[i]);
  }
}

void MASK_PASSWORD_FIELD(JsonObject& a, const char* b) {
  if(a.containsKey(b) && strlen(a[b]) > 0) { 
    a[b] = "........"; 
  }
}
/**
 * @param removePasswords set this true before sending data to the web ui
 * @param removeTransient set this true before saving state to SPIFFS
 */
void status_cleanupJsonData(JsonObject& jsonStatus, bool removePasswords, bool removeTransient) {
  if (removePasswords) {
    MASK_PASSWORD_FIELD(jsonStatus["wifi"],"password");
    MASK_PASSWORD_FIELD(jsonStatus["hotspot"],"password");
    MASK_PASSWORD_FIELD(jsonStatus["mqtt"],"password");
  }
  if (removeTransient) {
    jsonStatus.remove("status");
    JsonObject& jsonAnimation = jsonStatus["animation"];
    jsonAnimation.remove("animations");
    jsonAnimation.remove("palettes");
  }
}

int indexInArray(const char *a[], int size, const char *txt) {
  for(int i=0;i<size;i++) {
    if (strcmp(a[i], txt) == 0) {
      return i;
    }
  }
  return 0;
//  return -1;
}

#define CONTAINS_AND_OTHER(j,a,b) (j.containsKey(a) && strncmp(b, j[a], sizeof(b)) != 0)
#define CONTAINS_AND_OTHERi(j,a,b) (j.containsKey(a) && b != j[a])
#define SET_WHEN_CONTAINS_AND_OTHERs(j,a,b) if(CONTAINS_AND_OTHER(j,a,b))strncpy(b, j[a], sizeof(b)
#define SET_WHEN_CONTAINS_AND_OTHERi(j,a,b) if(CONTAINS_AND_OTHERi(j,a,b))b=j[a]

void copyJsonColor(JsonObject& jsonColor, CRGB* color) {
  if(jsonColor.containsKey(TK_colorRed)) {
    color->red = jsonColor[TK_colorRed];
  }
  if(jsonColor.containsKey(TK_colorGreen)) {
    color->green = jsonColor[TK_colorGreen];
  }
  if(jsonColor.containsKey(TK_colorBlue)) {
    color->blue = jsonColor[TK_colorBlue];
  }
  setUserPalette();
}

void apply_json_status(JsonObject& root, bool applyAndSave) {
  if (root.containsKey(JS_setup)) {
    JsonObject& setup = root.get(JS_setup);
    SET_WHEN_CONTAINS_AND_OTHERi(setup,JS_ledpin,setup_ledPin);
    SET_WHEN_CONTAINS_AND_OTHERi(setup,JS_ledcount,setup_ledCount);
    SET_WHEN_CONTAINS_AND_OTHERi(setup,JS_stripcount,setup_stripCount);
    SET_WHEN_CONTAINS_AND_OTHERi(setup,JS_reverse,setup_ledReverse);
    if(setup.containsKey(JS_orientation)) {
      if(strcmp(setup[JS_orientation], JS_orientationStrip) == 0) {
        setup_orientation = 0;
      } else
      if(strcmp(setup[JS_orientation], JS_orientationZigzag) == 0) {
        setup_orientation = 1;
      } else
      if(strcmp(setup[JS_orientation], JS_orientationSpiral) == 0) {
        setup_orientation = 2;
      }
    }
  }
  if (root.containsKey(JS_brightness)) {
    setBrightness(root[JS_brightness], applyAndSave);
  }
  if (root.containsKey("primary")) {
    copyJsonColor(root["primary"], &primaryColor);
  }
  if (root.containsKey("accent")) {
    copyJsonColor(root["accent"], &accentColor);
  }
  if (root.containsKey("wifi")) {
    JsonObject& wifi = root.get("wifi");
    if (wifi.containsKey("ssid")) {
      setWifiSsid(wifi["ssid"]);
    }
    if (wifi.containsKey("password")) {
      Serial.print("Update contains wifi password [");
      const char* txt = wifi["password"];
      Serial.print(txt);
      Serial.println("]");
      setWifiPassword(wifi["password"]);
    }
    if(applyAndSave) {
      wifiReconnect();
    }
  }
  if (root.containsKey("hotspot")) {
    JsonObject& hotspot = root.get("hotspot");
    if (hotspot.containsKey("ssid")) {
      setHotspotSsid(hotspot["ssid"]);
    }
    if (hotspot.containsKey("password")) {
      setHotspotPassword(hotspot["password"]);
    }
    if(applyAndSave) {
      wifiReconnect();
    }
  }
  if (root.containsKey("mqtt")) {
    bool reconnect = false;
    bool resubscribe = false;
    JsonObject& mqtt = root.get("mqtt");
    if(CONTAINS_AND_OTHER(mqtt,"host",mqtt_server)) {
      setMqttHost(mqtt["host"]);
      reconnect = true;
    }
    if(CONTAINS_AND_OTHERi(mqtt,"port",mqtt_port)) {
      setMqttPort(mqtt["port"]);
      reconnect = true;
    }
    if(CONTAINS_AND_OTHER(mqtt,"login",mqtt_user)) {
      setMqttLogin(mqtt["login"]);
      reconnect = true;
    }
    if(CONTAINS_AND_OTHER(mqtt,"password",mqtt_password)) {
      setMqttPassword(mqtt["password"]);
      reconnect = true;
    }
    if(CONTAINS_AND_OTHER(mqtt,"name",mqtt_devicename)) {
      strncpy(mqtt_devicename, mqtt["name"], sizeof(mqtt_devicename));
      resubscribe = true;
    }
    if(CONTAINS_AND_OTHER(mqtt,"intopic",mqtt_intopic)) {
      strncpy(mqtt_intopic, mqtt["intopic"], sizeof(mqtt_intopic));
      resubscribe = true;
    }
    if(CONTAINS_AND_OTHER(mqtt,"outtopic",mqtt_outtopic)) {
      strncpy(mqtt_outtopic, mqtt["outtopic"], sizeof(mqtt_outtopic));
      resubscribe = true;
    }
    if(mqtt.containsKey("listen")) {
      JsonObject& mqttListen = mqtt["listen"];
      if(CONTAINS_AND_OTHERi(mqttListen,"commands",mqtt_listen_commands)) {
        mqtt_listen_commands = mqttListen["commands"];
      }
    }
    if(mqtt.containsKey("publish")) {
      JsonObject& mqttPublish = mqtt["publish"];
      if(CONTAINS_AND_OTHERi(mqttPublish,"state",mqtt_publish_state)) {
        mqtt_publish_state = mqttPublish["state"];
      }
      if(CONTAINS_AND_OTHERi(mqttPublish,"gesture",mqtt_publish_gesture)) {
        mqtt_publish_gesture = mqttPublish["gesture"];
      }
    }
    if(applyAndSave) {
      if (reconnect) {
        mqttReconnect();
      } else if (resubscribe) {
        mqttResubscribe();
      }
    }
  }
  if (root.containsKey("animation")) {
    JsonObject& anim = root.get("animation");
    if (anim.containsKey("idleTimeout")){
      idleTimeout = anim["idleTimeout"];
    }
    if (anim.containsKey("idleBrightness")){
      idleBrightness = anim["idleBrightness"];
    }
    if (anim.containsKey("currentAnimation")){
      setAnimation(indexInArray(animationTextkeys, animationTextkeysCount, anim["currentAnimation"]));
    }
    if (anim.containsKey("idleAnimation")){
      setIdleAnimation(indexInArray(animationTextkeys, animationTextkeysCount, anim["idleAnimation"]));
    }
    if (anim.containsKey(JS_currentPalette)){
      setPalette(indexInArray(paletteTextkeys, paletteTextkeysCount, anim[JS_currentPalette]));
    }
    if (anim.containsKey("idlePalette")){
      setIdlePalette(indexInArray(paletteTextkeys, paletteTextkeysCount, anim["idlePalette"]));
    }
  }
  if (applyAndSave) {
    save_status();
  }
}

#define MQTT_SEND_LONG_IF(topic, dataKey) if(data.containsKey(dataKey))mqtt_sendLong(topic, data[dataKey])

void _status_sendIndividually(const char* topicPrefix, JsonObject& data) {
  const int prefixLen = strlen(topicPrefix);
  for(JsonObject::iterator it=data.begin(); it!=data.end(); ++it) {
    const char* key = it->key;
    const int keyLen = strlen(key);
    
    char subPrefix[prefixLen + keyLen + 2];
    if (prefixLen == 0) {
      strcpy(subPrefix, key);
      subPrefix[keyLen] = 0;
    } else {
      strcpy(subPrefix, topicPrefix);
      strcpy(subPrefix + prefixLen + 1, key);
      subPrefix[prefixLen] = '/';
      subPrefix[prefixLen + keyLen + 1] = 0;
    }

    if (it->value.is<JsonObject>()) {
      JsonObject& sub = it->value;
      _status_sendIndividually(subPrefix, sub);
    } else {
      const char* value = it->value;
      mqtt_sendLong(subPrefix, (byte*)value, strlen(value));
    }
  }
}

/**
 * Publish a status update via Web Socket, MQTT
 * @param rootObject {"action":"push","data":{...}}
 */
void status_broadcastUpdate(JsonObject& rootObject) {
  JsonObject& data = rootObject[JS_data];
  // remove passwords AND static data, push is to be small
  status_cleanupJsonData(data, true, true);
  const unsigned int jsonLength = rootObject.measureLength();
  char buffer[jsonLength+1];
  buffer[jsonLength] = 0;
  rootObject.printTo(buffer, jsonLength+1);

  websocketSend(buffer, jsonLength);
  if (mqtt_publish_state) {
    mqttSend(buffer, jsonLength);
    _status_sendIndividually("", data);
  }
}

/**
 * Publish a single value status update via Web Socket, MQTT
 */
void status_broadcastUpdate(const char* key, const char* text, unsigned int length) {
  char textCopy[length+1];
  textCopy[length]=0;
  memcpy(textCopy, text, length);

  DynamicJsonBuffer statusJsonBuffer;
  JsonObject& jsonStatus = statusJsonBuffer.createObject();
  jsonStatus["action"] = "push";
  JsonObject& data = jsonStatus.createNestedObject("data");
  data[key] = textCopy;

  status_broadcastUpdate(jsonStatus);
}

/**
 * Publish a single value status update via Web Socket, MQTT
 */
void status_broadcastUpdate(const char* key, int value) {
  char textCopy[40];
  sprintf(textCopy, "%d", value);

  status_broadcastUpdate(key, textCopy, strlen(textCopy));
}


