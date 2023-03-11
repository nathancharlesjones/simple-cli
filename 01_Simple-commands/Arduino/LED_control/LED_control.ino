String command = "";

bool led_is_blinking = true;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 500;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if( Serial.available() )
  {
    // If a character was received over UART,
    // read that value and echo it...
    //    
    char c = Serial.read();
    Serial.print(c);
    
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
      if( command == "on" ) led_is_blinking = true;
      else if ( command == "off" )
      {
        led_is_blinking = false;
        digitalWrite(LED_BUILTIN, LOW);
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
  
  // Non-blocking Blinky. If the "Blinky" task is enabled
  // and it's been <interval> milliseconds since the last 
  // time we toggled the LED (<previousMillis>), then 
  // toggle the LED.
  //
  if( led_is_blinking )
  {
    if (millis() - previousMillis >= interval)
    {
      previousMillis = millis();
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
}
