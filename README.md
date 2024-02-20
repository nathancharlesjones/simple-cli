# simple-cli
Watch me present on this topic at the 2023 Embedded Online Conference [here](https://embeddedonlineconference.com/session/Building_a_Simple_Command_Line_Interface).

An introduction to building a simple command-line interface for an embedded system. Each of the projects shown include examples for both Arduino and the STM32 (save the two projects in "Going Further").

## Read-UART
Demonstrates how to read a string over UART and echo it back to the terminal, all while blinking an LED.

## Simple-commands

### LED_control
Demonstrates how to interpret single word commands such as "on" and "off" to control an LED.

### Read_ADC
Demonstrates how to return a value when asked. Run the included PyQT script (after setting the COM port and baud rate correctly) to see a live graph of ADC values  (assuming you have a potentiometer or other analog voltage signal connected to the analog pin being read in the code).

## Command-plus-value
Demonstrates how to interpret commands that may include a numeric value. These projects allow a person to modify a blinking LED's duty cycle and frequency in addition to being able to turn it on or off.

## Going Further

### Arduino
Ports the "Command-plus-value" project to the Arduino Nano 33 IoT, allowing for LED control over WiFi.

### STM32
Demonstrates the use of the Memfault CLI library.
