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


#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#else 
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include "rom/rtc.h"
#endif

#include <TinyGPS++.h>
#include "RTClib.h"

#include "Global_Settings.h"
#include <SPI.h>
#include <Wire.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define g_SerialDebug Serial
// Set serial for AT commands (to the SIM module)
#define g_SerialAT  Serial1
// Set serial for GPS Module 
#define g_SerialGPS  Serial2



extern unsigned long IRAM_ATTR _millis();
 
volatile float minutes, seconds , secs;
volatile int degree, mins;

DateTime current;  
TinyGPSPlus gps;


int g_GMSStatus = 0;
int g_GMSNetwork = 0;
int g_SecCounter = 0;
int g_MinCounter = 0;
int g_10SecCounter = 0;
int g_BeepCounter = 0;

char g_SIMCellNumber[32];

char g_SMSInNumber[32];
char g_SMSInMessage[256];

char Response_Data[256];
char TOOLS_Buffer[256];

String MessageBuffer;
String TempBuffer;

int  CheckSMS = 1, CheckButtons = 1, CheckRSSI = 1,  g_GSMErrorCounter = 0, TenSecTick = 0;
long LastTimerTop = 0 ;


GSM_Config g_GSM_Config;
GPS_Coordinates g_GPS;
GPS_Coordinates g_GPSLastValidLocation;
USER_INPUT g_UserInput;


   
SPIClass SPI2(HSPI);

/****************************************************************************/
/****************************************************************************/

void DEBUG_Message(char *msg)
{
  g_SerialDebug.println(msg);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void InitGSMData( void )
{
  strcpy( g_GSM_Config.Service , "+33xxxxxxxx"); //TODO : update SIM's message service
  g_GSM_Config.PIN[0] = 0; //TODO : update SIM's PIN code
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/





void Beep( int duration = 100 )
{
    digitalWrite(BUZZER, HIGH);
    delay(duration);
    digitalWrite(BUZZER, LOW);
}


void setup()
{
int res, y, mo, d, t, i;  
uint32_t col;
uint32_t Freq = 0;

  g_GPS.Valid = 0;
  
  pinMode(BP1, INPUT_PULLUP);   
  pinMode(VBATPIN, INPUT);
  pinMode(SIM800_RESET, OUTPUT);
  pinMode(SIM800_DTR, OUTPUT);

  
  digitalWrite(SIM800_RESET, LOW);
  digitalWrite(SIM800_DTR, LOW);
  pinMode(CHARGING, INPUT);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(LED_1, OUTPUT);
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);
  digitalWrite(LED_2, LOW);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_3, LOW);

 
  Beep();

  DisplayLed(LED_YELLOW);
  delay(200);
  DisplayLed(LED_GREEN);
  delay(200);
  DisplayLed(LED_RED);
  delay(200);
  DisplayLed(LED_BLUE);
  delay(200);
  DisplayLed(LED_NONE);

  

/****************************************/
// general setup
  // Set console baud rate
  g_SerialDebug.begin(115200);
  g_SerialDebug.println("Started");

  UpdateStartupBlinker();

  InitGSMData();
  setup_GSM_Modem();
  
  BootGSMModule();
  
  res = GSM_ReadCNUM( g_SIMCellNumber );
  g_SerialDebug.print("SIM Cell Number : ");
  g_SerialDebug.println( g_SIMCellNumber );    

  // GPS Modem 
  g_SerialDebug.println("Setup g_GPS Modem");
  res = setup_GPS_Modem();


  UpdateDisplay();
  g_SerialDebug.println("Started");
 
}

/*********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************/

int ca, pa = 0;
char g_DebugOrder[32];
char g_SIMMessage[64];
int g_SIMMessageCounter = 0;
int PushButton=0, PushMode=0;
int g_DispCnt = 0;

void loop()
{
int x, y, res, a, b, i, PVAL, rssi;
char c;
float Dist;

/***********************************************************/
// purge SIM800 data and manage unsollicited messages
  while (g_SerialAT.available())  // If anything comes in Serial0
  {   
    c = g_SerialAT.read();
    if(c == 0x0d) 
    {
      g_SerialDebug.println();
      g_SerialDebug.print("SIM:");
      g_SerialDebug.println(g_SIMMessage);
      g_SIMMessageCounter = 0;
      g_SIMMessage[g_SIMMessageCounter] = 0;              
    }
    else
    {
      if( ( g_SIMMessageCounter < 63 ) && (c >= 0x20) )
      {
        g_SIMMessage[g_SIMMessageCounter++] = c;              
        g_SIMMessage[g_SIMMessageCounter] = 0;              
      }
    }
  }
/***********************************************************/
  // manage the timer 
 if( ( _millis() - LastTimerTop ) > 9 )   // each 10 ms - 100Hz
  {
    GPS_UpdateData();
    LastTimerTop = _millis();
    CheckButtons = 1;
    g_SecCounter ++;
    g_10SecCounter++;
  }

  if (g_SecCounter >= 100)  // earch 1 s
  {
    g_SecCounter = 0;
    g_MinCounter ++;
  }

  if (g_10SecCounter >= 1000)  // earch 10 s
  {
    g_10SecCounter = 0;
    TenSecTick = 1;
    CheckSMS = 1;
    g_SerialDebug.println("CORE: 10 Sec tick");   
  }

  if (g_MinCounter >= 60)  // each 1 min
  {
    g_MinCounter = 0;
    CheckRSSI = 1;
    
    g_SerialDebug.println("CORE: 1 min tick");   // read it and send it out Serial (USB)
    
    GPS_GetDateTime();
    printDateTime(gps.date, gps.time);
    

  }


/***********************************************************/
  if( CheckButtons )
  {
    CheckButtons = 0;
    GetUserInputs();

    if( g_UserInput.Button )  // if trigger 
    {
      // if button pushed BUT bot released in between, then ignore it
      if( PushButton == BUTTON_NONE )
      {   
        PushButton = g_UserInput.Button;
        // wait for button release
        delay(10);
        do
        {
          GetUserInputs();
          delay(10);
          CheckButtons++;
        }
        while( ( g_UserInput.Button != BUTTON_NONE) && (CheckButtons < 81) );
        if( CheckButtons > 80 )
          PushMode = PUSH_LONG;
        else
            PushMode = PUSH_SHORT;
      }  
    }
    else
    {
        PushButton = BUTTON_NONE;
        PushMode = PUSH_NONE;
    }
    CheckButtons = 0;
  }

  /***************************************/
  // manage button is required
  /***************************************/

  if( CheckSMS )
  {
    CheckSMS = 0;  
    
    res = 0;  
    res = GSM_ReadSMS( g_SMSInNumber, g_SMSInMessage );
    if(res>0)
    {
      // copy message
      //SMS_ProcessMessage( g_SMSInNumber, g_SMSInMessage );
      g_SerialDebug.print("SMS Received : ");
      g_SerialDebug.print(g_SMSInNumber);
      g_SerialDebug.println(g_SMSInMessage);

      // manage POS request
      if( strcmp( strupr(g_SMSInMessage) , "POS" ) == 0 )
      {
        toolsBuildPOSMessage( );
        GSM_SendSMS( g_SMSInNumber , (char *)MessageBuffer.c_str() );
        g_SerialDebug.println("POS message sent");  
      }

      if( strcmp( strupr(g_SMSInMessage) , "UNBEEP" ) == 0 )
      {
        g_BeepCounter = 0;
        g_SerialDebug.println("UNBEEP message processed");  
      }

      if( strcmp( strupr(g_SMSInMessage) , "BEEP" ) == 0 ) 
      {
        g_BeepCounter = 30; // run for 30 BEEPs
        g_SerialDebug.println("BEEP message processed");  
      }
    }
  }

  if( TenSecTick  )
  {
    TenSecTick = 0;
      
// begin manage BEEP
    if( g_BeepCounter )
    {
      Beep();
      delay(100);
      Beep();
      delay(100);
      Beep();

      g_BeepCounter --;
      if(g_BeepCounter<0)
        g_BeepCounter = 0;
    }
// End manage BEEP
  }





  if( CheckRSSI )
  {
    CheckRSSI = 0;
    ReadBatteryLevel();
    GSM_GetStatus();
    
    if( g_GMSNetwork == 0)
    {
      GSM_CheckNetworkRegistration();
    }

    // manage the GSM issue
    if( g_GMSNetwork == 0)
    {
      g_GSMErrorCounter ++;  
    }
    else
    {
      g_GSMErrorCounter = 0;
    }
    // check RSSI
    g_UserInput.RSSI = GSM_ReadRSSI();
  }
}
