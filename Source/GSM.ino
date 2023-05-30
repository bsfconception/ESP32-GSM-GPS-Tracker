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

extern char Response_Data[256];
extern char TOOLS_Buffer[256];

void setup_GSM_Modem()
{
    // Set GSM module baud rate and UART pins
    g_SerialDebug.println("Setup GSM Baud Rate");
  g_SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);

}


/**************************************************/
void BootGSMModule( void )
{
  g_SerialDebug.println("GSM_Reset");

  GSM_Reset();

  GSM_WaitForModuleReady();

  g_SerialDebug.println("GSM_Init");
  GSM_Init(1);
  
  g_UserInput.RSSI = GSM_ReadRSSI();
}
/**************************************************/

void GSM_Reset( void )
{
  digitalWrite(SIM800_RESET, HIGH);
  delay(1000);
  digitalWrite(SIM800_RESET, LOW);
  delay(1000);
}


void GSM_GetStatus( void )
{
    // checks if the module is started
  g_GMSStatus = GSM_sendATcommand(_AT, _OK, 2000);
}


void GSM_WaitForModuleReady( void )
{
int answer;
  do
  {
   UpdateStartupBlinker();
   answer = GSM_sendATcommand(_AT, _OK, 2000);
  }
  while(answer == 0);
}

/*****************************************************************************************/


char GSM_UpdateService()
{
char answer=0, i;
String Command;

  
 // send SMS service address
  Command = (char *)_ATCSCA;
  Command = Command  + g_GSM_Config.Service;
  Command = Command  + "\"";
  answer = GSM_sendATcommand((char *)Command.c_str(), _OK, 500);
  if(answer == 0)
    return(0);  
  delay(200);
  return(1);
} 

int GSM_Init( int display )
{
char answer=0, i;
String Command;
int TryLoop = 0;
  

  g_GMSStatus = 0;
  g_GMSNetwork = 0;

  while( g_SerialAT.available() > 0) 
    g_SerialAT.read();    // Clean the input buffer

  UpdateStartupBlinker();

  // checks if the module is started
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_AT, _OK, 2000);
    delay(1000);
    TryLoop++;
  }
  while( (TryLoop<20) && (answer==0) );
  if(answer == 0)
    return(0);
  delay(50);
    
  UpdateStartupBlinker();

  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATD, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  // switch off echo
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATEO, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  //enter PIN code
  if( strlen( g_GSM_Config.PIN ) > 0 )
  {
    Command = (char*)_ATCPIN;
    Command = Command  + g_GSM_Config.PIN;
    // send PIN number
    TryLoop = 0;
    do
    {
      answer = GSM_sendATcommand((char*)Command.c_str(), _OK, 2000);
      TryLoop++;
    }
    while( (TryLoop<3) && (answer==0) );
      
    if(answer == 0)     return(0);  
  }
  
  UpdateStartupBlinker();
  
  // Request calling line identification
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATCLIP1, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  TryLoop = 0;
  do
  {
  answer = GSM_sendATcommand(_ATCMEE0, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  if(answer == 0)     return(0);
    
  UpdateStartupBlinker();
  
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand((char *)_ATCSCS, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
 
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  // init SMS storage
  // Disable messages about new SMS from the GSM module 
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATCNMI2, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  
  if(answer == 0)     return(0);  
  
  UpdateStartupBlinker();

// send AT command to init memory for SMS in the SIM card
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATCPMS, /*"+CPMS:"*/ _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  
  // select phonebook memory storage
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATCPBS, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();

  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand((char *)_ATCGSMS3, _OK, 500);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  
  if(answer == 0)     return(0);  

  UpdateStartupBlinker();
  
  // set the SMS mode to text 
  TryLoop = 0;
  do
  {
    answer = GSM_sendATcommand(_ATCMGF1, _OK, 2000);
    TryLoop++;
  }
  while( (TryLoop<3) && (answer==0) );
  
  if(answer == 0)     return(0);  
  
  UpdateStartupBlinker();

  if( strlen( g_GSM_Config.Service ) > 0 )
  {
    // send SMS service address
    Command = _ATCSCA;
    Command = Command  + g_GSM_Config.Service;
    Command = Command  + "\"";
    
    g_SerialDebug.println( (char *)Command.c_str() );

    TryLoop = 0;
    do
    {
      answer = GSM_sendATcommand((char *)Command.c_str(), _OK, 1000);
      TryLoop++;
      UpdateStartupBlinker();

    }
    while( (TryLoop<3) && (answer==0) );
  
    if(answer == 0)
      return(0);  
    delay(200);
  }  
    
  UpdateStartupBlinker();
 
  GSM_GetStatus();

  UpdateStartupBlinker();

  GSM_CheckNetworkRegistration();

  g_SerialDebug.print("g_GMSStatus : ");
  g_SerialDebug.println(g_GMSStatus);
  g_SerialDebug.print("g_GMSNetwork : ");
  g_SerialDebug.println(g_GMSNetwork);
  
  return(1);
}


void GSM_CheckNetworkRegistration( void )
{
char i;  
uint8_t x=0,  answer=0;

  g_GMSNetwork = 0;
  
  i = 0;
  do
  {
    answer = GSM_sendATcommand((char *)_ATCREG, _OK , 1000);
    if(answer == 0)     
      return;  
  
    if( strstr( Response_Data, "+CREG:0,1" ) != NULL )
    {
      g_GMSNetwork = 1;
      return;  
    }
    if( strstr( Response_Data, "+CREG:0,5" ) != NULL )
    {
      g_GMSNetwork = 1;
      return;  
    }
    delay(2000);
    i++;
  }  
  while(i<10);
}





int GSM_ReadRSSI( void )
{
int ret_val = 0, answer;
  char *p_char;
  byte status;
  
  
    if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
    {
      g_SerialDebug.println("GSM Status Error");
      return(0);
    }


  answer = GSM_sendATcommand((char *)_ATCSQ, _OK, 2000);
  if(answer == 0)
   return(-1); 

  if( strstr(Response_Data, "+CSQ:") ) 
  { 
    p_char = strchr(Response_Data,':');
    
    if (p_char != NULL) 
    {
      ret_val = atoi(p_char+1);
    }
    else 
    {
      ret_val = 0;
    }

  }
  delay(10);

  return (ret_val);
}


int GSM_ReadCNUM( char *Number )
{
int ret_val = 0, answer;
  char *p_char, *n_char;
  
  
  byte status;
    if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
    {
      g_SerialDebug.println("GSM Status Error");
      return(0);
    }


  answer = GSM_sendATcommand((char *)_ATCNUM, _OK, 2000);
  if(answer == 0)
   return(-1); 

   g_SerialDebug.print("AT answer:");
   g_SerialDebug.println(Response_Data);

  if( strstr(Response_Data, "+CNUM:") ) 
  { 
    p_char = strchr(Response_Data,',');
    
    if (p_char != NULL) 
    {
      p_char+=2;
      n_char = strchr(p_char,'"');
      if( n_char )
        *(n_char) = 0;
      strncpy( Number, p_char, 16 );
      Number[15] = 0;
      ret_val = 1;
    }
    else 
    {
      ret_val = 0;
    }

  }
  delay(10);

  return (ret_val);
 
}

/*************************************************************************************************************************/
/*************************************************************************************************************************/



int8_t GSM_sendATcommand(char* ATcommand, char* expected_answer, unsigned long timeout)
{

uint8_t x=0,  answer=0;
char  res;
unsigned long previous;
  

    g_SerialAT.println(ATcommand);    // Send the AT command 

    if(expected_answer != NULL)
    {
      x = 0;
      previous = _millis();
      // this loop waits for the answer
      res = 0;
      do
      {
          if(g_SerialAT.available() != 0)
          {    
              // if there are data in the UART input buffer, reads it and checks for the asnwer
              res = g_SerialAT.read();
              if(res != ' ')
              {
                if(x<250)
                {
                  Response_Data[x] = res;
                  x++;
                  Response_Data[x] = 0;
                }
                // check if the desired answer  is in the response of the module
                if (strstr(Response_Data, expected_answer) != NULL)    
                {
                    answer = 1;
                }
              }
          }
          delay(1);
      }
      while((answer == 0) && ((_millis() - previous) < timeout));    
    }
    else
    {
      answer = 1;
    }

  return answer;
}

int8_t GSM_sendATcommandExt(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned long timeout)
{
uint8_t x=0,  answer=0;
char  res;
unsigned long previous;
  

  g_SerialAT.println(ATcommand);    // Send the AT command 



  if(expected_answer1 != NULL)
  {
    x = 0;
    previous = _millis();
    // this loop waits for the answer
    res = 0;
    do
    {
        if(g_SerialAT.available() != 0)
        {    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            res = g_SerialAT.read();
            if(res != ' ')
            {
              if(x<250)
              {
                Response_Data[x] = res;
                x++;
                Response_Data[x] = 0;
              }
              // check if the desired answer  is in the response of the module
              if (strstr(Response_Data, expected_answer1) != NULL)    
              {
                  answer = 1;
              }
              if( expected_answer2 != NULL )
              {
                if (strstr(Response_Data, expected_answer2) != NULL)    
                {
                    answer = 2;
                }
              }
            }
        }
        delay(1);
    }
    while((answer == 0) && ((_millis() - previous) < timeout));    
  }
  else
  {
    answer = 1;
  }
  return answer;
}



int8_t GSM_waitATreply(char* expected_answer, unsigned long timeout){

uint8_t x=0,  answer=0;
char  res;
unsigned long previous;
  

  x = 0;
  
  previous = _millis();
  // this loop waits for the answer
  do
  {
      if(g_SerialAT.available() != 0)
      {    
          // if there are data in the UART input buffer, reads it and checks for the asnwer
          res = g_SerialAT.read();
          if(res != ' ')
          {
            if(x<250)
            {
              Response_Data[x] = res;
              x++;
              Response_Data[x] = 0;
            }
            // check if the desired answer  is in the response of the module
            if (strstr(Response_Data, expected_answer) != NULL)    
            {
                answer = 1;
            }
          }
      }
      delay(1);
  }
  while((answer == 0) && ((_millis() - previous) < timeout));    
  
  return answer;
}
