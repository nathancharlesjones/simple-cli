String string_from_UART = "";

bool led_is_blinking = true;
unsigned long previousMillis = 0;        // will store last time LED was updated
long on_time = 500;           // interval at which to blink (milliseconds)
long off_time = 500;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
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
      string_from_UART += c;
    }
    
    // Otherwise, process the command. Message dictionary:
    //   "on"          | Turns on the Blinky task.
    //   "off"         | Turns off the Blinky task.
    //   "dc <val>"    | Sets the duty cycle; <val> is an (optional)
    //                   integer percent. Prints the current value
    //                   for the duty cycle if <val> is omitted.
    //   "freq <val>"  | Sets the frequency; <val> is an (optional)
    //                   floating frequency in hertz. Prints the current
    //                   value for the frequency if <val> is omitted.
    //
    else
    {
      // Parse the command. Start by looking for the space that separates
      // "dc" and "freq" from their (optional) values.
      //
      String command = "", args = "";
      int index_of_first_space = string_from_UART.indexOf(' ');

      // If a space is found...
      //
      if( index_of_first_space != -1 )
      {
        // ...set <command> equal to that first word and set <args> equal 
        // to the rest of the command (i.e. <val>).
        //
        command = string_from_UART.substring(0, index_of_first_space);
        args = string_from_UART.substring(index_of_first_space + 1);
      }

      // If NO space is found, args remains empty. Set <command> to the
      // exact string received over UART.
      //
      else
      {
        command = string_from_UART;
      }

      // Process <command> and <args>
      //
      if( command == "on" ) led_is_blinking = true;
      else if( command == "off" )
      {
        led_is_blinking = false;
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if( command == "dc" )
      {
        // If <args> is empty, then print out the current value for duty cycle
        //
        if( args == "" )
        {
          Serial.print(on_time*100.0/(on_time + off_time));
          Serial.println("%");
        }

        // Otherwise, recompute the duty cycle. <args> has just one value, so we
        // can convert the entire string to a float. (If there were more values, 
        // we would need to further subdivide <args> like we did above, by looking
        // for spaces that separated each value.)
        //
        else
        {
          int period = on_time + off_time;
          on_time = args.toFloat() * period / 100.0;
          off_time = period - on_time;
        }
      }
      else if( command == "freq" )
      {
        // If <args> is empty, then print out the current value for frequency
        //
        if( args == "" )
        {
          Serial.print(1000.0/(on_time + off_time));
          Serial.println(" Hz");
        }
        
        // Otherwise, recompute the frequency. <args> has just one value, so we
        // can convert the entire string to a float. (If there were more values, 
        // we would need to further subdivide <args> like we did above, by looking
        // for spaces that separated each value.)
        //
        else
        {
          float freq = 1000.0 / ( on_time + off_time );
          
          on_time = on_time * freq / args.toFloat();
          if( on_time < 1 ) on_time = 1;
          
          off_time = off_time * freq / args.toFloat();
          if( off_time < 1 ) off_time = 1;
        }
      }
      else
      {
        Serial.print("Unknown command: ");
        Serial.println(string_from_UART);
      }
      string_from_UART = "";
    }
  }
  
  // Non-blocking Blinky. If the "Blinky" task is enabled
  // then check if the LED is currently on or off.
  //
  if( led_is_blinking )
  {
    // If the LED is on, and has been on for at least <on_time> 
    // milliseconds, then turn the LED off.
    //
    if( digitalRead(LED_BUILTIN) )
    {
      if (millis() - previousMillis >= on_time)
      {
        previousMillis = millis();
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    
    // Otherwise check if it has been off for at least <off_time> 
    // milliseconds and, if it has, turn the LED on.
    //
    else
    {
      if (millis() - previousMillis >= off_time)
      {
        previousMillis = millis();
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
  }
}
