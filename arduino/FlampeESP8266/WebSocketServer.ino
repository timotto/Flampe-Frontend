#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // construct response
        DynamicJsonBuffer statusJsonBuffer;
        JsonObject& jsonStatus = statusJsonBuffer.createObject();
        jsonStatus[JS_action] = JS_push;
        JsonObject& data = jsonStatus.createNestedObject(JS_data);
        create_status_object(data);
        status_cleanupJsonData(data, true, false);
        data[JS_status] = JS_connected;
        data[JS_connection] = wifi_isApClient(ip)?JS_ap:JS_sta;

        // send message to client
        const int jsonLength = jsonStatus.measureLength();
        char buffer2[jsonLength+1];
        buffer2[jsonLength] = 0;
        jsonStatus.printTo(buffer2, jsonLength+1);
        webSocket.sendTXT(num, buffer2, jsonLength);
        Serial.println("Sent connected response!");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      char buffer[length+1];
      buffer[length] = 0;
      memcpy(buffer, payload, length);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject((char*)payload);

      if (strcmp(JS_push,json[JS_action]) == 0) {
        Serial.println("Received push!");
        JsonObject& pushData = json[JS_data];
        apply_json_status(pushData);
        status_broadcastUpdate(json);
      }
      break;
  }
}

void websocketSend(const char* text, int length) {
  webSocket.broadcastTXT(text, length);
}

void setup_websocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop_websocket() {
  webSocket.loop();
}

