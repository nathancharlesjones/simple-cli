// Reference:
// https://docs.arduino.cc/tutorials/communication/wifi-nina-examples#wifinina-simple-web-server-wifi

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient client;

String string_from_UART = "";
bool led_is_blinking = true;
unsigned long previousMillis = 0;        // will store last time LED was updated
long on_time = 500;           // interval at which to blink (milliseconds)
long off_time = 500;           // interval at which to blink (milliseconds)

void setup()
{
  Serial.begin(115200);      // initialize serial communication
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  initWiFi();
}

void loop() {
  // Connect to a client, if not already
  //
  if( !client )
  {
    client = server.available();   // listen for incoming clients
    if( client ) client.connected();
  }

  /******************************************************
   * NOTE: The code below is copied, nearly verbatim, from
   * 02_Command-plus-value/Arduino/Arduino.ino, which just 
   * goes to show that our CLI cares not where the command
   * string comes from, only that it is composed of ASCII 
   * characters that adhere to the simple message dictionary
   * defined below.
   * 
   * The only changes were to "if( Serial.available() )"
   * (which became "if( client && client.available() )")
   * and to "Serial.print/.println" (which became 
   * "client.print/.println").
  ******************************************************/  
  if( client && client.available() )                               // if you get a client,
  {
    // If a character was received over WiFi,
    // read that value and echo it...
    //    
    char c = client.read();
    client.print(c);
    
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
      string_from_UART.trim();

      if( string_from_UART.length() > 0 )
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
            client.print(on_time*100.0/(on_time + off_time));
            client.println("%");
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
            client.print(1000.0/(on_time + off_time));
            client.println(" Hz");
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
          client.print("Unknown command: ");
          client.println(string_from_UART);
        }
        string_from_UART = "";
      }
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

void initWiFi()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
