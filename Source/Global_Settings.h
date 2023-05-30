/***************************************************************************************************************
MIT License

Copyright (c) 2023 BSF Conception

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***************************************************************************************************************/

#define BUZZER        23
#define CHARGING      36

#define LED_1            19
#define LED_2            18
#define LED_3            5

#define MODEM_TX             27
#define MODEM_RX             26
#define SIM800_RESET        25
#define SIM800_DTR            12

#define I2C_SDA              21
#define I2C_SCL              22

#define GPS_TX             32
#define GPS_RX             34

#define BP1           15  

#define VBATPIN       4

/*********************************************/

#define LED_NONE     -1
#define LED_YELLOW   0
#define LED_GREEN    1
#define LED_RED      2
#define LED_BLUE     3




#define ERROR_NOGMSNET  1
#define ERROR_CGATT1    2
#define ERROR_CIPSTART  3
#define ERROR_CIPSEND   4
#define ERROR_CIPSENDKO 5
#define ERROR_CIPSPRT   6
#define ERROR_CGATT0    7
#define ERROR_CONNECTKO  8
#define ERROR_PLANEMODE  9
#define ERROR_BADCONFIG  10
#define ERROR_CICCR    11
#define ERROR_DISABLED    12


#define MODULEID_SIZE       16
#define PHONENUMBER_SIZE  16

#include <Wire.h>

#define _min(a,b) ((a)<(b)?(a):(b))
#define _max(a,b) ((a)>(b)?(a):(b))


#define _minmax(a,b,c) ((a)<(b)?(b):( (a)>(c)?(c):(a)  ))
#define _minmaxloop(a,b,c) ((a)<(b)?(c):( (a)>(c)?(b):(a)  ))



#define BUTTON_NONE      0
#define BUTTON_UP        5
#define BUTTON_DOWN      6

#define PUSH_NONE      0
#define PUSH_SHORT      1
#define PUSH_LONG       2


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */



char _CRLF[] = "\r\n";

char _OK[] = "OK";
char _AT[]  = "AT";   // check COM
//char _ATF[] = "AT&F";  // Factory default config -> to remove
char _ATD[] = "AT&D0";  // ignores status on DTR (change on 12/05/2023))
char _ATEO[] = "ATE0";  // ECHO OFF
char _ATCLIP1[] = "AT+CLIP=1";    // Request calling line identification
char _ATCMEE0[] = "AT+CMEE=0";    // No Mobile Error Code
char _ATCMEE2[] = "AT+CMEE=2";    // Verbose  Mobile Error Code
char _ATCMGF1[] = "AT+CMGF=1";    // set the SMS mode to text 
char _ATCNMI2[] = "AT+CNMI=2,0";  // SMS message indication
char _ATCPMS[] = "AT+CPMS=\"SM\",\"SM\",\"SM\""; // manage SMS storage
char _ATCPBS[] = "AT+CPBS=\"SM\"";     // select phonebook memory storage


char _ATCMGL[] ="AT+CMGL=\"REC UNREAD\"";  // check is an SMS is present
char _ATCMGDA[] ="AT+CMGDA=\"DEL INBOX\"";   // delete this sms   *SIM800 AT+CMGD              
char _ATCMGDALL[] ="AT+CMGDA=\"DEL ALL\"";   // delete all sms    *SIM800 AT+CMGD
char _ATGSN[] = "AT+GSN";  // rtetrieve the IMEI of the device
char _ATPWRDWN[] = "AT+CPOWD=1";  // Power down


char _ATCGATT1[] PROGMEM ="AT+CGATT=1";     // Attached GPRS Service

char _ATCSCA[] PROGMEM ="AT+CSCA=\"";     // send SMS service address
char _ATCPIN[] PROGMEM ="AT+CPIN=";       //  enter PIN code
char _ATCSCS[] PROGMEM ="AT+CSCS=\"GSM\"";     // select character set
char _ATCGSMS3[] PROGMEM ="AT+CGSMS=3";      // service for SMS message
char _ATCREG[] PROGMEM ="AT+CREG?";       // manange Registration
char _ATCSQ[] PROGMEM ="AT+CSQ";   // read RSSI
char _ATCNUM[] PROGMEM ="AT+CNUM";   // read SIM Num



typedef struct {
  int Valid;
  int SatValid; // the nb of sal is valid
  int NbSat;
  int LocValid; // the location is valid
  double Lat;
  double Lng;
  int AltValid; // the altitude is valid
  double Altitude;
  int SpdValid; // the speed is valid
  double Speed;
  double Accuracy;
  long TimeStamp;
//  long GPSDelay;
  char DateTime[32];
  char Date[12];
  char Time[12];
//  int Compas;
} GPS_Coordinates;




typedef struct {
  int Button;
  int Charging;
  int VBat;
  int VBatValue;
  int RSSI;
} USER_INPUT;


 
typedef struct {
  // HT_Home
  char Service[PHONENUMBER_SIZE];
  char PIN[MODULEID_SIZE];  
} GSM_Config;
