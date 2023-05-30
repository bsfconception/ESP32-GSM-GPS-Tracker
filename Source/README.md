The test code is provided "as is" but 100% functionnal and compiled using Arduino IDE

![image](https://github.com/bsfconception/ESP32-GSM-GPS-Tracker/assets/84618082/ea34c178-200e-4a96-8ced-014bd8f98578)

The test code includes
- IO management (Buzzer, Button, Leds)
- GPS module management 
  - serial connection
  - NEMA decoding 
  - GPS position extraction (thanks to TinyGPS++ library)
  - Date and time retrieval 
- SIM800 GMS module control
  -  Serial connection
  -  AT protocol communication
  -  Module startup and configuration
  -  SMS Management

By default, the sotfware listen to SMS messages and triggers the corresping action
  - 'POS' : returns the GPS position of the module (including GooGleMap URL)
  - 'BEEP' : triggers 3 'beep' each 10 seconds. Repeated 30 times
  - 'UNBEEP': Cancels the 'BEEP' order

In order to adapt the test code to your SIM cart, only update these 2 lines :

  - strcpy( g_GSM_Config.Service , "+33xxxxxxxx"); //TODO : update SIM's message service
  - g_GSM_Config.PIN[0] = 0; //TODO : update SIM's PIN code



