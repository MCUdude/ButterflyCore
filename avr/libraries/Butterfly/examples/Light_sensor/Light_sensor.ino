/*----------- ButterflyCore LCD example ---------------|
|                                                      |
| Based on the Butteruino project                      |
| https://github.com/davidknaack/butteruino            |
|                                                      |
| Updated and rewritten by MCUdude November 2016       |
| https://github.com/MCUdude/ButterflyCore             |
|                                                      |
| Released to the public domain                        |
|                                                      |
| This example will read the light sensor and display  |
| its value on the LCD.                                |
|-----------------------------------------------------*/
 
#include "Butterfly.h"

// Create an object of the ButterflyLCD class
ButterflyLCD lcd;

void setup() 
{
  // Initialize LCD
  lcd.begin(); 

  // Print welcome message
  lcd.print("Light sensor");

  // Wait for the message to finish scrolling
  delay(5000);
}

void loop() 
{ 
  // Read light sensor, connected to A2
  lcd.print(analogRead(LIGHT_SENSOR));

  // Wait a little
  delay(1000);
}