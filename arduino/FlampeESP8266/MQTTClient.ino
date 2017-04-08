#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "textkeys.h"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void setup_mqtt() {
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);
}

void loop_mqtt() {
  if (strlen(mqtt_server) == 0 || mqtt_port < 1) {
    return;
  }
  if(mqtt_isConnectedOrReconnect()) {
    mqtt_client.loop();
  }
}

int reconnectStep = 0;
uint32_t reconnectStepTime = 0;

bool mqtt_isConnectedOrReconnect() {
  switch(reconnectStep) {
    case 0:
      // entry & exit case
      if (mqtt_client.connected()) {
        return true;
      }
      reconnectStepTime = millis() + 2000;
      reconnectStep = 1;
      break;
    case 1:
      {
        if (millis() < reconnectStepTime) {
          break;
        }
        // attempt connection
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect(FLAMPE_ID)) {
          Serial.println("connected");
          // Once connected, publish an announcement...
          mqttSend("{\"status\":\"online\"}");
          mqttResubscribe();
          Serial.println("MQTT connected, message sent");
          reconnectStep = 0;
        } else {
          Serial.print("failed, rc=");
          Serial.print(mqtt_client.state());
          Serial.println(" try again in 5 seconds");
          reconnectStep = 2;
        }
      }
      break;
    case 2:
      reconnectStep = 3;
      reconnectStepTime = millis() + 5000;
      break;
    case 3:
      // connect failed
      if (millis() >= reconnectStepTime) {
        // Wait 5 seconds before retrying
        reconnectStep = 0;
      }
      break;
  }
  return false;
}

void mqttResubscribe() {
  if(strnlen(mqtt_intopic, sizeof(mqtt_intopic)) > 0) {
    const int intopicLen = strlen(mqtt_intopic);
    char subtopic[intopicLen+3];
    sprintf(subtopic, "%s/#", mqtt_intopic);
    mqtt_client.subscribe(mqtt_intopic);
    mqtt_client.subscribe(subtopic);
  }
}

void mqttSend(const char* text, int length) {
  if(strnlen(mqtt_outtopic, sizeof(mqtt_outtopic)) < 1) {
    return;
  }
  mqtt_client.publish(mqtt_outtopic, text, length);
}

void mqtt_sendLong(const char* topic, int payload) {
  char tmp[40];
  sprintf(tmp, "%d", payload);
  mqtt_sendLong(topic, (byte*)tmp, strlen(tmp));
}

void mqtt_sendLong(const char* topic, byte* payload, unsigned int length) {
  if(strnlen(mqtt_outtopic, sizeof(mqtt_outtopic)) < 1) {
    return;
  }
  const int outtopicLen = strlen(mqtt_outtopic);
  const int topicLen = strlen(topic);
  // both path components plus a slash plus 0
  char subtopic[outtopicLen+topicLen+2];
  sprintf(subtopic, "%s/%s", mqtt_outtopic, topic);
  mqtt_client.publish(subtopic, payload, length);
}

void mqttSend(const char* text) {
  if(strnlen(mqtt_outtopic, sizeof(mqtt_outtopic)) < 1) {
    return;
  }
  mqtt_client.publish(mqtt_outtopic, text);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (!mqtt_listen_commands) {
    return;
  }
  
  // complete match
  if(strcmp(mqtt_intopic, topic) == 0) {
    mqtt_onPlainTopic(payload, length);
  } else {
    const int inTopicLength = strlen(mqtt_intopic);
    // begins with intopic, is longer and continues with a slash (a sub-directory!)
    if(strncmp(mqtt_intopic, topic, inTopicLength) == 0 && strlen(topic) > inTopicLength && topic[inTopicLength] == '/') {
      // continue with topic after the / behin the known intopic
      mqtt_onLongTopic(&topic[inTopicLength+1], payload, length);
    }
  }
}

void mqtt_onLongTopic(char* topic, byte* payload, unsigned int length) {
  DynamicJsonBuffer statusJsonBuffer;
  JsonObject& jsonStatus = statusJsonBuffer.createObject();
  jsonStatus["action"] = "push";
  JsonObject& data = jsonStatus.createNestedObject("data");
  JsonObject *top = &data;

  char textValue[length+1];
  textValue[length] = 0;
  memcpy(textValue, payload, length);
  
  char* topicRest = topic;
  int slashIndex;
  while((slashIndex = strIndexOf(topicRest, '/')) != -1) {
    char name[slashIndex+1];
    name[slashIndex] = 0;
    memcpy(name, topicRest, slashIndex);
    topicRest = topicRest+slashIndex+1;
    top = &(top->createNestedObject(statusJsonBuffer.strdup(name)));
  }
  (*top)[topicRest] = textValue;
  apply_json_status(data, true);
  status_broadcastUpdate(jsonStatus);
}

int strIndexOf(const char* s, char x) {
  int i;
  const int n = strlen(s);
  for(i=0;i<n;i++) {
    if(s[i] == x) {
      return i;
    }
  }
  return -1;
}

void mqtt_onPlainTopic(byte* payload, unsigned int length) {
  char buffer[length+1];
  memcpy(buffer, payload, length);
  buffer[length] = 0;
  DynamicJsonBuffer jsonBuffer;

  JsonObject& json = jsonBuffer.parseObject((char*)payload);

  if (!json.success()) {
    Serial.println("Unable to parse JSON");
    return;
  }
  if (!(json.containsKey("action"))) {
    Serial.println("no action in JSON - ignored");
    return;
  }
  if (!(json.containsKey("data"))) {
    Serial.println("no data in JSON - ignored");
    return;
  }
  if (strcmp(JS_action,json[JS_action]) == 0) {
    Serial.println("Received push!");
    JsonObject& pushData = json[JS_data];
    apply_json_status(pushData, true);
    status_broadcastUpdate(json);
  }
}

void setMqttHost(const char* host) {
  strncpy(mqtt_server, host, sizeof(mqtt_server));
}

void setMqttPort(int port) {
  mqtt_port = port;
}

void setMqttLogin(const char* login) {
  strncpy(mqtt_user, login, sizeof(mqtt_user));
}

void setMqttPassword(const char* password) {
  strncpy(mqtt_password, password, sizeof(mqtt_password));
}

void mqttReconnect() {
  mqtt_client.disconnect();
}

