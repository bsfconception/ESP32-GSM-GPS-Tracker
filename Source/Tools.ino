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

extern char TOOLS_Buffer[256];
extern String TempBuffer;
extern String MessageBuffer;

int g_LedBlinker = 0;

unsigned long IRAM_ATTR _millis()
{
    return (unsigned long) (esp_timer_get_time() / 1000);
}


int Interface_GetButton( void )
{
int bp1, bp2;

  bp1 = digitalRead(BP1);
  if(bp1 == 0)
    return(BUTTON_UP);
  return(BUTTON_NONE);
}


void GetUserInputs( void )
{
  g_UserInput.Button = Interface_GetButton();
  g_UserInput.Charging = digitalRead(CHARGING);
}

void ReadBatteryLevel( void ) 
{
  int BATVal;
  BATVal = analogRead(VBATPIN);
  g_SerialDebug.print("ReadBAT:");
  g_SerialDebug.println(BATVal);
  g_UserInput.VBatValue = BATVal; 
}


int toolsBuildPOSMessage( void )
{
  ConvertTimeStampToString( g_GPSLastValidLocation.TimeStamp, TOOLS_Buffer );
  MessageBuffer = "Heure : " + String(TOOLS_Buffer) + "\n";

  MessageBuffer = MessageBuffer + "GPS\nhttps://www.google.com/maps/place/#LAT#+#LNG#";

  DegMinSec(fabs(g_GPSLastValidLocation.Lat));
  TempBuffer = String(degree);
  TempBuffer = TempBuffer + "%C2%B0";
  TempBuffer = TempBuffer + String(mins);
  TempBuffer = TempBuffer + "'";
  TempBuffer = TempBuffer + String(secs, 3);
  if(g_GPSLastValidLocation.Lat>0)
  {
    TempBuffer = TempBuffer + "%22N";
  }
  else
  {
    TempBuffer = TempBuffer + "%22S";
  }
  MessageBuffer.replace("#LAT#", TempBuffer );

  DegMinSec(fabs(g_GPSLastValidLocation.Lng));
  TempBuffer = String(degree);
  TempBuffer = TempBuffer + "%C2%B0";
  TempBuffer = TempBuffer + String(mins);
  TempBuffer = TempBuffer + "'";
  TempBuffer = TempBuffer + String(secs, 3);
  if(g_GPSLastValidLocation.Lng>0)
  {
    TempBuffer = TempBuffer + "%22E";
  }
  else
  {
    TempBuffer = TempBuffer + "%22W";
  }

  MessageBuffer.replace("#LNG#", TempBuffer );
  MessageBuffer.replace(" ", "" );
  MessageBuffer = MessageBuffer + "\n";
  
  MessageBuffer = MessageBuffer + "GPS Position : " + String(g_GPSLastValidLocation.Lat, 7) + ";" +String(g_GPSLastValidLocation.Lng, 7)+ "\n";
  MessageBuffer = MessageBuffer + "GPS Speed : " + String(g_GPSLastValidLocation.Speed, 2) + "\n";

  return(1);            
}




long ComputeTimeStamp( void )
{
    return( (3600*current.hour()) + (60*current.minute())  + current.second() );
}


void ConvertTimeStampToString( long Stamp, char *Out )
{
long h, m, s;
int i = 0;
  if (Stamp < 0)
    Stamp = 0;
  h = Stamp / 3600;
  m = (Stamp - (3600*h)) /60;
  s = (Stamp - (3600*h) - (60*m));

    *(Out + i++) = (char)(h / 10 + '0');
    *(Out + i++) = (char)(h % 10 + '0');
    *(Out + i++) = 'h';
    *(Out + i++) = ' ';
    *(Out + i++) = (char)(m / 10 + '0');
    *(Out + i++) = (char)(m % 10 + '0');
    *(Out + i++) = 'm';
    *(Out + i++) = 0;
    
}

/**********************************************************************************************/
void DegMinSec( double tot_val)    /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = seconds;
}


/********************************************/

void DisplayLed( int color )
{
 switch( color )
  {
      case LED_NONE:
      default:
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, LOW);
        digitalWrite(LED_3, LOW);
        break;
      case LED_RED:
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, LOW);
        digitalWrite(LED_3, HIGH);
        break;
      case LED_BLUE:
        digitalWrite(LED_1, HIGH);
        digitalWrite(LED_2, HIGH);
        digitalWrite(LED_3, LOW);
        break;
      case LED_GREEN:
        digitalWrite(LED_1, HIGH);
        digitalWrite(LED_2, LOW);
        digitalWrite(LED_3, LOW);
        break;
      case LED_YELLOW:
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, HIGH);
        digitalWrite(LED_3, HIGH);
        break;
  }

}


void UpdateDisplay( void )
{

  if( !(g_MinCounter & 0x01) )
  {
    if( (g_GMSStatus == 1) && (g_GMSNetwork == 1) )
    { // GSM OK
      if( g_GPS.Valid != 0 )
      {
        // ALL OK - GREEN
        DisplayLed( LED_GREEN );
      }
      else
      {
        // NO GPS OK - BLUE
        DisplayLed( LED_BLUE );
      }
    }
    else // NO GMS
    {
      if( g_GPS.Valid != 0 )
      {
        // GPS OK - ORANGE
        DisplayLed( LED_YELLOW );
      }
      else
      {
        // NO GPS - RED
        DisplayLed( LED_RED );
      }
    }
  }
  else
  {
    DisplayLed( LED_NONE );
  }
}



void UpdateStartupBlinker( void )
{
  DisplayLed( g_LedBlinker & 0x03 );
  g_LedBlinker++;
  delay(200);
}

  
