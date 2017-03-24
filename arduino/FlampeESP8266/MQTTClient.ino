#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_mqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop_mqtt() {
   if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }
}

int reconnectStep = 0;
uint32_t reconnectStepTime = 0;
void reconnect() {
  switch(reconnectStep) {
    case 0:
      // entry & exit case
      if (client.connected()) {
        return;
      }
      reconnectStep = 1;
      break;
    case 1:
      {
        // attempt connection
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
          Serial.println("connected");
          reconnectStep = 3;
        } else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
          reconnectStep = 2;
          reconnectStepTime = millis() + 5000;
        }
      }
      break;
    case 2:
      // connect failed
      if (millis() >= reconnectStepTime) {
        // Wait 5 seconds before retrying
        reconnectStep = 0;
      }
      break;
    case 3:
      // connected
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      reconnectStep = 0;
      Serial.println("MQTT connected, message sent");
      break;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

