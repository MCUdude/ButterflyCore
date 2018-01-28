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
| This example will show some of the fetures of the    |
| Butterfly LCD library.                               |
|-----------------------------------------------------*/

#include "ButterflyLCD.h"

// Create an object of the ButterflyLCD class
ButterflyLCD lcd;

void setup()
{
  // Initialize LCD
  lcd.begin();

  // Print message from RAM
  lcd.print("Hello World");

  // Wait for the message to finish scrolling
  delay(3500);

  // Clear LCD
  lcd.clear();
  
  // Print message from flash memory. This is the most efficient way
  lcd.print_f(PSTR("Message from flash"));

  // Wait for the message to finish scrolling
  delay(5000);

  // Clear LCD
  lcd.clear();

  // Show colons
  lcd.showColons(true);

  // Wait a little while
  delay(2000);
  
  // Disable colons
  lcd.showColons(false); 

  // Wait a little while
  delay(1000);
  
  // The scolling is interrupt based, so the  text will
  // continue to scroll even if the loop() is empty
  lcd.print_f(PSTR("Scolling text"));
}


void loop()
{
	// Empty loop
}