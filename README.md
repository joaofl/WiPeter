**WiPeter** is a WiFi low-cost power meter.

This project was created using Platform.OI in order to monitor the power consumption of my entire home. It connected to the main power line of my home.

Its based on the ESP-8266 chip, more specifically on the 12-E dev module.

![Alt text](https://github.com/joaofl/WiPeter/blob/master/docs/pinout.jpeg?raw=true "ESP-12E pinout")

I use mainly two libs, one for the PZEM-004 powe meter, and another one for the publish subscriber MQTT connections with the Home Assitant sever. Find the links below:

Arduino communication library for Peacefair PZEM-004T Energy monitor
https://github.com/olehs/PZEM004T

esphomelib is a framework for using your ESP8266/ESP32 devices with Home Assistant http://esphomelib.com
https://github.com/OttoWinter/esphomelib

There is a nice tutorial on how to assembly the hardware here:
http://pdacontrolen.com/meter-pzem-004t-with-arduino-esp32-esp8266-python-raspberry-pi/

It lacks documentetation, but its fully functional by the current date. In case you want to use this project and have doubts, feel free to contact, open and issue or contribute to the wiki.