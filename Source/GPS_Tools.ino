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

extern unsigned long IRAM_ATTR _millis();

void GPS_GetDateTime( void )
{
  current = DateTime( gps.date.year(),gps.date.month(),gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second() );
}

int setup_GPS_Modem()
{
  
  g_SerialDebug.println("Setup GPS Baud Rate");  
  g_SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  g_GPS.Valid = 0;

  return(1);
}  


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = _millis();
  do 
  {
    while (g_SerialGPS.available())
      gps.encode(g_SerialGPS.read());
  } while (_millis() - start < ms);
}




void GPS_UpdateData()
{
  smartDelay(5);

  g_GPS.SatValid = gps.satellites.isValid();
  g_GPS.NbSat = gps.satellites.value();

  g_GPS.LocValid = gps.location.isValid();
  g_GPS.Lat = gps.location.lat();
  g_GPS.Lng = gps.location.lng();
  
  g_GPS.AltValid = gps.altitude.isValid();
  g_GPS.Altitude = gps.altitude.meters();
  
  g_GPS.SpdValid = gps.speed.isValid();
  g_GPS.Speed = gps.speed.kmph();

  if( gps.location.age() < 60000 )
    g_GPS.Valid = 1;
  else
    g_GPS.Valid = 0;

  g_GPSLastValidLocation.Valid = g_GPS.Valid;

  if(g_GPS.Valid)
  {
    g_GPSLastValidLocation.SatValid = g_GPS.SatValid;
    g_GPSLastValidLocation.NbSat = g_GPS.NbSat;
    
    g_GPSLastValidLocation.LocValid = g_GPS.LocValid;
    g_GPSLastValidLocation.Lat = g_GPS.Lat;
    g_GPSLastValidLocation.Lng = g_GPS.Lng;
    
    g_GPSLastValidLocation.SpdValid = g_GPS.SpdValid;
    g_GPSLastValidLocation.Speed = g_GPS.Speed;
    
    g_GPSLastValidLocation.AltValid = g_GPS.AltValid;
    g_GPSLastValidLocation.Altitude = g_GPS.Altitude;
    
    g_GPSLastValidLocation.TimeStamp = ComputeTimeStamp();
    
    TempBuffer = (char)(gps.date.day() / 10 + '0');
    TempBuffer = TempBuffer + (char)(gps.date.day() % 10 + '0');
    TempBuffer = TempBuffer + "/";
    TempBuffer = TempBuffer + (char)(gps.date.month() / 10 + '0');
    TempBuffer = TempBuffer + (char)(gps.date.month() % 10 + '0');
    TempBuffer = TempBuffer + "/";
    TempBuffer = TempBuffer + gps.date.year();
    TempBuffer = TempBuffer + " - ";

    TempBuffer = TempBuffer + (char)(gps.time.hour() / 10 + '0');
    TempBuffer = TempBuffer + (char)(gps.time.hour() % 10 + '0');
    TempBuffer = TempBuffer + ":";
    TempBuffer = TempBuffer + (char)(gps.time.minute() / 10 + '0');
    TempBuffer = TempBuffer + (char)(gps.time.minute() % 10 + '0');
    TempBuffer = TempBuffer + ":";
    TempBuffer = TempBuffer + (char)(gps.time.second() / 10 + '0');
    TempBuffer = TempBuffer + (char)(gps.time.second() % 10 + '0');
    strcpy(g_GPSLastValidLocation.DateTime, TempBuffer.c_str() );
  }
}

static void printFloat(float val, bool valid, int len, int prec)
{
  
  if (!valid)
  {
    while (len-- > 1)
      g_SerialDebug.print('*');
  }
  else
  {
    g_SerialDebug.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      g_SerialDebug.print(' ');
  }
  g_SerialDebug.print(' ');
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len-1] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  g_SerialDebug.print(sz);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    g_SerialDebug.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}



static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  g_SerialDebug.print(F("GPS data:"));
  if (!d.isValid())
  {
    g_SerialDebug.print(F("*NO DATE* "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    g_SerialDebug.print(sz);
  }
  
  if (!t.isValid())
  {
    g_SerialDebug.print(F("*NO TIME* "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    g_SerialDebug.print(sz);
  }

  g_SerialDebug.print(F("Age: "));
  printInt(d.age(), d.isValid(), 5);

  g_SerialDebug.print(F("Nb Sat: "));
  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  
  g_SerialDebug.print(F("hdop: "));
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  
  g_SerialDebug.println();

  smartDelay(0);
}
  
