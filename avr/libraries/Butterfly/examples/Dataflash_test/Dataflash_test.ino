/*--------- ButterflyCore dataflash example -----------|
| Based on the Butteruino project                      |
| https://github.com/davidknaack/butteruino            |
|                                                      |
| Updated and rewritten by MCUdude November 2016       |
| https://github.com/MCUdude/ButterflyCore             |
|                                                      |
| Released to the public domain                        |
|                                                      |
| A port of Atmels datalflash libary for the onboard   |
| AT45DB041B5 SPI flash memory                         |
|-----------------------------------------------------*/

#include "Butterfly.h"

// Create an object of the ButterflyLCD class
ButterflyLCD lcd;

// Create an object of the ButterflyDataflash class
ButterflyDataflash flash;

// Fuction to test the flash chip. Returns true if successful
bool test()
{
    bool result = true;

    // Set up to write data to SRAM buffer 1, starting at byte 0
    flash.BufferWriteEnable(1,0);
    // Write a series of bytes into the buffer
    for(uint8_t i = 2; i < 52; i++)
        flash.WriteNextByte(i);

    // Write SRAM buffer 1 to flash page 1
    flash.BufferToPage(1, 1);            
    
    // Set up to read directly from flash page 1, starting at byte 0
    flash.ContFlashReadEnable(1, 0);

    // Read a series of bytes and check the value
    for (uint8_t i = 2; i < 52; i++) {
        if ( i != flash.ReadNextByte()) 
        {
            result = false;
            break;
        }
    }

    // Deactivate the flash chip when not in use
    flash.Deactivate();    
    return result;
}


void setup()
{
  // Initialize the LCD
  lcd.begin();

  // Print message
  lcd.print("Flash test");
  
  delay(2000);

  //Clear the LCD
  lcd.clear();
  
  if (test() == true)
  {
    // Flash write succeeded
    lcd.print("Success");
  }  
  else
  {
    // Flash write failed
    lcd.print("Failure");
  }
}

void loop()
{
  // Empty loop
}
