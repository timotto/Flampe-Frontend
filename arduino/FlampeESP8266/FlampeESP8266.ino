
//const char* mqtt_server = "bude.spacenet.vpn42";

const char* www_username = "admin";
const char* www_password = "esp8266";

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";
char mqtt_user[40];
char mqtt_password[40];
char wifi_ssid[40];
char wifi_password[40];
char hotspot_ssid[40];
char hotspot_password[40];

void setup() {
  Serial.begin(115200);
  setup_config();
  setup_wifi();
  
  setup_http();
  setup_mqtt();
  setup_led();
  Serial.println("Started!");
}

void loop() {
  loop_mqtt();
  loop_http();
  loop_led();
}


