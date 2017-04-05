#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "WifiClientPrint.h"

ESP8266WebServer server(80);

void setup_http() {
//  server.on("/", [](){
//    if(!server.authenticate(www_username, www_password))
//      return server.requestAuthentication();
//    server.send(200, "text/plain", "Login OK");
//  });

  // serveStatis is picky: /w wouldn't work, had to be /w/ as directory
  server.serveStatic("/",   SPIFFS, "/w/"  ,"max-age=604800"); 

  server.onNotFound([](){
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();

  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop_http() {
  server.handleClient();
}

