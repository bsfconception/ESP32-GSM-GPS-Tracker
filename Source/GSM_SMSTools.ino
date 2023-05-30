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

char GSM_SendSMS(char *number_str, char *message_str) 
{
char answer, i;
  
    
  g_SerialDebug.print("SMS:Try to Send Message to : ");
  g_SerialDebug.println(number_str);

  if(strlen(number_str) == 0 )
    return(0);

  if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
  {
    g_SerialDebug.println("GSM Status Error");
    return(0);
  }
  
  g_SerialDebug.println("Sending Message");

  sprintf(TOOLS_Buffer, "AT+CMGS=\"%s\"" , number_str);
  g_SerialAT.println( TOOLS_Buffer );
  
  
  answer = GSM_waitATreply( (char *)">", 1000);
  if(answer)
  {
    g_SerialDebug.print( "Sending : ");
    g_SerialDebug.println( message_str );
    // send SMS text
    g_SerialAT.print(message_str); 
    g_SerialAT.write((char)0x1A);
   
    answer = GSM_waitATreply( _OK, 10000);
    if(answer)
      return(answer);
  }
     
  return (answer);  
}



char GSM_IsSMSPresent( void )
{
char ret_val = 0, answer;
  char *p_char;
  byte status;

  if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
  {
    g_SerialDebug.println("GSM Status Error");
    return(0);
  }

  answer = GSM_sendATcommand(_ATCMGL, _OK, 2000);
  if(answer == 0)
   return(-1); 

  // something was received but what was received?
  // ---------------------------------------------
  if( strstr(Response_Data, "+CMGL:") ) 
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

char GSM_GetSMS(char position, char *phone_number, char *SMS_text, byte max_SMS_len)
{
  
  
  char ret_val = 0, answer;
  char *p_char; 
  char *p_char1;
  byte len;
  if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
  {
    g_SerialDebug.println("GSM Status Error");
    return(0);
  }

  if (position == 0) return (0);
  
  phone_number[0] = 0;  // end of string for now
  
  
  g_SerialAT.print("AT+CMGR=");
  g_SerialAT.println((int)position);  


  answer = GSM_waitATreply( _OK, 5000);

  
  if( strstr(Response_Data, _OK) ) 
  { 
    // extract phone number string
    // ---------------------------
    p_char = strchr(Response_Data,',');
    if (p_char != NULL) 
    {
      p_char1 = p_char+2; // we are on the first phone number character
      p_char = strchr((char *)(p_char1),'"');
      if (p_char != NULL) 
      {
        *p_char = 0; // end of string
        strcpy(phone_number, (char *)(p_char1));
      }
      ret_val = 1;
      // get SMS text and copy this text to the SMS_text buffer
      // ------------------------------------------------------
      p_char = strchr(p_char+1, 0x0a);  // find <LF>
      if (p_char != NULL) 
      {
        // next character after <LF> is the first SMS character
        p_char++; // now we are on the first SMS character 

        // find <CR> as the end of SMS string
        p_char1 = strchr((char *)(p_char), 0x0d);  
        if (p_char1 != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char1 = 0; 
          // in case there is not finish sequence <CR><LF> because the SMS is
          // too long (more then 130 characters) sms text is finished by the 0x00
          // directly in the gsm.WaitResp() routine
  
          // find out length of the SMS (excluding 0x00 termination character)
          len = strlen(p_char);
  
          if (len < max_SMS_len) 
          {
            // buffer SMS_text has enough place for copying all SMS text
            // so copy whole SMS text
            // from the beginning of the text(=p_char position) 
            // to the end of the string(= p_char1 position)
            strcpy(SMS_text, (char *)(p_char));
          }
          else 
          {
            strncpy(SMS_text, (char *)(p_char), max_SMS_len - 2);
            SMS_text[max_SMS_len-1] = 0; // finish string
          }
        }
      }
    }
  }
  delay(100);
  return (ret_val);  
}


char GSM_DeleteSMS( void )
{

  
  char ret_val = -1, answer;

    if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
    {
      g_SerialDebug.println("GSM Status Error");
      return(0);
    }

  g_SerialDebug.println("SMS Delete Message");  

  answer = GSM_sendATcommand(_ATCMGDA, _OK, 7000);
  
   return (answer);
}


int GSM_ReadSMS( char *Number, char *Message )
{
char pos, res;  
  

  Number[0] = 0;
  Message[0] = 0;  

  if( ( g_GMSStatus == 0 ) || (g_GMSNetwork == 0) )
  {
    g_SerialDebug.println("GSM Status Error");
    return(0);
  }

    
  pos = GSM_IsSMSPresent();
  if(pos<=0)
  {
    return(0);
  }
   
  if (pos>0) 
  {
    // read new SMS
    res = GSM_GetSMS(pos, Number, Message, 127); // 08022023 increase
    // purge SMS
   
    GSM_DeleteSMS();
    return(1);
  }
  return(0);
}
