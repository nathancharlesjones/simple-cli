#define MAX_ADC_VALUE_LEN 6

String command = "";

void setup() {
  Serial.begin(115200);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
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
      if( command == "r" )
      {
        char buffer[MAX_ADC_VALUE_LEN] = {0};
      
        // Put four-digit integer at the start of buffer, then
        // add newline and carriage return to the end.
        //
        sprintf(buffer, "%4d", analogRead(A1));
        buffer[MAX_ADC_VALUE_LEN-2] = '\n';
        buffer[MAX_ADC_VALUE_LEN-1] = '\r';
        
        Serial.print(buffer);
      }
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
}
