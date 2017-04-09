#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "WifiClientPrint.h"
#include "detail/RequestHandlersImpl.h"

class TimingStaticRequestHandler : public StaticRequestHandler {
public:
  TimingStaticRequestHandler(FS& fs, const char* path, const char* uri, const char* cache_header) 
  : StaticRequestHandler(fs, path, uri, cache_header) 
  {}
  
  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
    const uint32_t t0 = millis();
    const bool result = StaticRequestHandler::handle(server, requestMethod, requestUri);
    const uint32_t dt = millis() - t0;
    Serial.printf("%s %s in %d ms\n",result?"Served":"Failed to serve",requestUri.c_str(),dt);
    return result;
  }
};

const char * ETAG_SERVER_HEADER = "ETag";
const char * ETAG_CLIENT_HEADER = "If-None-Match";

class EtagStaticRequestHandler : public TimingStaticRequestHandler {
public:
  EtagStaticRequestHandler(FS& fs, const char* path, const char* uri, const char* cache_header, String etag_value) 
  : TimingStaticRequestHandler(fs, path, uri, cache_header), _etag_value(etag_value)
  {}
  
  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
    String request_etag = server.header(ETAG_CLIENT_HEADER);
    if (_etag_value == request_etag) {
      server.send(304);
      server.client().stop();
      return true;
    }
    server.sendHeader(ETAG_SERVER_HEADER, _etag_value);
    return TimingStaticRequestHandler::handle(server, requestMethod, requestUri);
  }
protected:
  String _etag_value;
};

ESP8266WebServer server(80);

void setup_http() {
//  server.on("/", [](){
//    if(!server.authenticate(www_username, www_password))
//      return server.requestAuthentication();
//    server.send(200, "text/plain", "Login OK");
//  });

  // serveStatis is picky: /w wouldn't work, had to be /w/ as directory
  // void ESP8266WebServer::serveStatic(const char* uri, FS& fs, const char* path, const char* cache_header) {
//  server.serveStatic("/",   SPIFFS, "/w/"  ,"max-age=604800"); 


  SPIFFS.begin();
  
  // calculate current etag value based on the filenames
  char etagStr[16];
  _http_calculate_etag(etagStr, 16);
  String etagValue = etagStr;
  Serial.printf("ETag value: %s\n", etagStr);
  
  // configure WebServer class to collect ETag request header value
  server.collectHeaders(&ETAG_CLIENT_HEADER, 1);
  server.addHandler(new EtagStaticRequestHandler(SPIFFS, "/w/", "/", NULL, etagValue));

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

void _http_calculate_etag(char* etagValue, size_t size) {
  uint32_t value = 0;
  Dir dir = SPIFFS.openDir("/w/");
  while (dir.next()) {
    String pathName = dir.fileName();
    size_t fileSize = dir.fileSize();
    
    int i = pathName.lastIndexOf('/');
    String fileName = (i == -1)?pathName:pathName.substring(i+1);
    if (fileName.length() < 10) {
      // too short, at least fedcba98.x
//      Serial.printf("File ignored, name too short: %s\n", fileName.c_str());
      continue;
    }
    bool allHex = true;
    for(i=0;i<8;i++) {
      char c = fileName.charAt(i);
      if (!( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )) {
        allHex = false;
      }
    }
    if (!allHex) {
//      Serial.printf("File ignored, not all hex: %s\n", fileName.c_str());
      continue;
    }
    char hexBytes[9];
    memcpy(hexBytes, fileName.c_str(), 8);
    hexBytes[8] = 0;
    uint32_t fileValue = strtol(hexBytes, NULL, 16);
    uint32_t oldValue = value;
    value ^= fileSize;
    value ^= fileValue;
//    Serial.printf("oldValue=%d value=%d fileSize=%d fileValue=%d hexBytes=[%s] fileName=%s\n", oldValue, value, fileSize, fileValue, hexBytes, fileName.c_str());
  }
  snprintf(etagValue, size, "%08x", value);
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

