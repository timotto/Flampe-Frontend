# Flampe

## Frontend 

This project is generated with [yo angular generator](https://github.com/yeoman/generator-angular)
version 0.16.0.

I added a process function to the options of filerev. The function uses the hash and extension only to make the 
filenames shorter and fit better on the ESP8266 SPIFFS filesystem. 

### Build & development

Run `grunt` for building and `grunt serve` for preview.

### Testing

Running `grunt test` will run the unit tests with karma.

## Arduino

The content of the dist/ directory resulting from a grunt build should become the content of the 
arduino/FlampeESP8266/data/w/ folder. All the files may also be compressed using gzip.

The Sketch relies on FastLED to draw patterns. An optional APDS-9960 sensor connected via I2C allows hand gesture, and 
light intensity and color detection.

