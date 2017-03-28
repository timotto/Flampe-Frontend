#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_mqtt() {
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);
  
}

void loop_mqtt() {
   if (!mqtt_client.connected()) {
    mqtt_reconnect();
  } else {
    mqtt_client.loop();
  }
}

int reconnectStep = 0;
uint32_t reconnectStepTime = 0;
void mqtt_reconnect() {
  if (strlen(mqtt_server) == 0 || mqtt_port < 1) {
    return;
  }
  switch(reconnectStep) {
    case 0:
      // entry & exit case
      if (mqtt_client.connected()) {
        return;
      }
      reconnectStep = 1;
      break;
    case 1:
      {
        // attempt connection
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect(mqtt_devicename)) {
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
}

void mqttResubscribe() {
  if(strnlen(mqtt_intopic, sizeof(mqtt_intopic)) > 0) {
    mqtt_client.subscribe(mqtt_intopic);
  }
}

void mqttSend(const char* text, int length) {
  if(strnlen(mqtt_outtopic, sizeof(mqtt_outtopic)) < 1) {
    return;
  }
  mqtt_client.publish(mqtt_outtopic, text, length);
}

void mqttSend(const char* text) {
  if(strnlen(mqtt_outtopic, sizeof(mqtt_outtopic)) < 1) {
    return;
  }
  mqtt_client.publish(mqtt_outtopic, text);
}

void callback(char* topic, byte* payload, unsigned int length) {
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
  const char* action = json["action"];
  if (mqtt_listen_commands && strncmp("push",action,4) == 0) {
    Serial.println("Received push!");
    JsonObject& pushData = json["data"];
    apply_json_status(pushData, true);
    status_cleanupJsonData(pushData, true, false);
    websocketSend(buffer, length);
    if (mqtt_publish_state) {
      mqttSend(buffer);
    }
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

