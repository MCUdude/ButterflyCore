/*----- ButterflyCore NTC temperature example ---------|
|                                                      |
| Based on the Butteruino project                      |
| https://github.com/davidknaack/butteruino            |
|                                                      |
| Updated and rewritten by MCUdude November 2016       |
| https://github.com/MCUdude/ButterflyCore             |
|                                                      |
| Released to the public domain                        |
|                                                      |
| This example will print the current temperature on   |
| the Butterfly LCD library.                           |
|-----------------------------------------------------*/
 
#include "Butterfly.h"

// Create an object of the ButterflyLCD class
ButterflyLCD lcd;

// Create an object of the ButterflyTemp class
ButterflyTemp temp(CELSIUS);

void setup() 
{
  lcd.begin(); 
  
  // Print welcome message
  lcd.print("Butterfly NTC temp");

  // Wait for the message to finish scrolling
  delay(5000);

  // Enable 8x oversampling for improved accuracy
  //temp.overSample(true);
}


void loop() 
{ 
  // Set cursor on the first digit
  lcd.setCursor(0);

  // Print current temperature in Celsius
  lcd.print(temp.getTemp(CELSIUS)); // Yank? Replace CELSIUS with FARENHEIT 

  // Set cursor on the third digit
  lcd.setCursor(2);

  // Print 'C
  lcd.print(".C");

  // Wait a little
  delay(1000);
}
