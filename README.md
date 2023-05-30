# ESP32-GSM-GPS-Tracker
IOT - ESP32 based GSM - GPS Tracker Kit

![Ima![Image28](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/6d365abb-b42f-4ca5-a85d-7d404164cbf8)
ge6](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/8420cfc1-f8a4-4f51-b17a-c2ec5cddba75)

![PinOut](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/ddfefb31-1a75-4d62-b05c-81a11787898c)


## What is it?
This module is an autonomous GPS tracker based on an ESP32 processor.

It support regular SIM800 shield to manage 2G GSM communication and it is compatible with most 3.3V based GPS modules. With a dedicated software, the GPS localization of the module can be easily tracked using SMS exchanges or GPS localisation push on a dedicated web server.

The entire module (with Li-Ion battery) it small and light enough (70g) to be attached to an animal. The autonomy may vary based on the software and installed battery.

As an example, the module using SMS exchanges and UDP upload (each 3 minutes) runs during 14 hours with a 1000mAh battery.

Note : ONLY the PCB with NO components is provided, the mounter version will be proposed later

## Why did you make it?
This module is a spin-off of a horse tracker system than is used to monitor and alert during equestrian riding. This light version can be attached to any animal in order to follow him or retrieve him during rambles

## What makes it special?
The main features are: 
- ESP32 Wrover CPU 
- Li-Ion battery charger 
- USB connector 
- I2C connector 
- Serial programming connector 
- Buzzer 
- SIM800 PGM module pinOut 
- TOPGNSS GPS module pinout 
- Small form factor 
- Compatible with most 3.3V GPS Modules

It works with there modules SIM800 : https://fr.aliexpress.com/item/32370694678.html
![original](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/30c19919-67d3-4129-95b3-62782ec621f0)
![Image5](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/f70ea80f-a49c-4be6-ab83-522295902ebe)


GPS : 
https://fr.aliexpress.com/item/1005001634086877.html 
https://fr.aliexpress.com/item/32812641442.html 
https://fr.aliexpress.com/item/32949232692.html
![HTB1meiUilsmBKNjSZFsq6yXSVXaX](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/f28806d4-0698-421b-ada3-f9b64e4561ee)

![Image1](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/c57fd541-502a-4c9f-b4d9-ce1a26d08960)
![Image14](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/6e121b85-2d00-499d-aae7-d9503ff04d3c)



Test Code
A test code will be provided soon on Github. It will be based on standard AT communication protocols
