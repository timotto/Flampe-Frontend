# Flampe

Flampe / Vlamp started as an LED "core" within some random bottle or vase with a gesture sensore at the top. 
The FastLED demos made it look nice already, the gesture sensore switched palette and animation pattern. 
The first iteration was focused on BT LE connectivity. This is the WiFi evolution.

The device starts an Access Point with the SSID "Flampe" and the PSK "password" for initial setup and when the 
configured WiFi connection cannot connect successfully. The device listens on 192.168.4.1 but eagers to become a 
captive portal just like WiFiManager. 

The web interface allows to configure an MQTT server for status update and control.

## Hardware

The hardware consists of an ESP8266 chip connected to an LED strip. Optionally an APDS-9960 gesture and light sensor may 
be connected via I2C.

## Frontend 

This project is generated with [yo angular generator](https://github.com/yeoman/generator-angular)
version 0.16.0.

### Build & development

There are some changes in the Gruntfile.js to simplify the use of the web front end on an ESP8266 WiFi web server: 
- The filerev task removes the original part of the filename leaving only hash and extension. Because the SPIFFS of 
the ESP8266 OS has some limitations on the total filename length.
- The compress task creates a compressed version of every file in the compressed/ folder, adding a .gz suffix.
- The clean and copy tasks have an :android option managing the data/w directory within the Arduino sketch - as long as 
the sketch remains in the subdirectory of this project. There's also a rename:arduino task which renames the 
index.html.gz into index.htm.gz because the web server has htm as default extension for the index file.

Run `grunt` for building, `grunt serve` for preview, and `grunt arduino` to create the SPIFFS content.

### Testing

Running `grunt test` will run the unit tests with karma.

