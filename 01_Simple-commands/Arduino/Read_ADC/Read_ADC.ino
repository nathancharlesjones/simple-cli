#define MAX_ADC_VALUE_LEN 6

String command = "";
bool reading_ADC = false;
unsigned long previousMillis = 0;        // will store last time ADC was read
const long interval = 500;           // interval at which to send ADC values (milliseconds)

void setup() {
  Serial.begin(115200);
}

void loop() {
  if( Serial.available() )
  {
    // If a character was received over UART,
    // read that value...
    //    
    char c = Serial.read();
    
    // As long as the character isn't a newline
    // or carriage return, add it to the command
    // string.
    //
    if( (c != '\n') && (c != '\r') )
    {
      command += c;
    }

    // Otherwise, process the command by comparing it 
    // to "on" and "off".
    //
    else
    {
      // Setting <reading_ADC> to true/false turns on/off
      // the "Read ADC" task (see below).
      //
      if( command == "on" ) reading_ADC = true;
      else if ( command == "off" ) reading_ADC = false;
      else
      {
        Serial.print("Unknown command: ");
        Serial.println(command);
      }

      // Then reset the command string to receive 
      // another command.
      //
      command = "";
    }
  }
  
  // Non-blocking ADC read. If the "reading_ADC" task is enabled 
  // and it's been <interval> milliseconds since the last time 
  // we read the ADC (<previousMillis>), then read the ADC and 
  // send out the value over Serial.
  //
  if( reading_ADC )
  {
    if (millis() - previousMillis >= interval)
    {
      previousMillis = millis();
      char buffer[MAX_ADC_VALUE_LEN] = {0};
      
      // Put four-digit integer at the start of buffer, then
      // add newline and carriage return to the end.
      //
      sprintf(buffer, "%4d", analogRead(A0));
      buffer[MAX_ADC_VALUE_LEN-2] = '\n';
      buffer[MAX_ADC_VALUE_LEN-1] = '\r';
      
      Serial.print(buffer);
    }
  }
}
