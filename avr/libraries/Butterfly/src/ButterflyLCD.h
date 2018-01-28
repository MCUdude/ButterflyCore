/*----- ButterflyCore LCD driver - ButterflyLCD.h -----|
| Based on the Butteruino project                      |
| https://github.com/davidknaack/butteruino            |
|                                                      |
| Updated and rewritten by MCUdude November 2016       |
| https://github.com/MCUdude/ButterflyCore             |
|                                                      |
| Released to the public domain                        |
|                                                      |
| This is a basic driver for the Butterfly LCD.        |
| It offers the ability to change the contrast         |
| and display strings (scrolling or static) from       |
| flash or SRAM memory.                                |
| This has been completly rewritten from the Atmel     |
| code; in this version, as much processing as         |
| possible is performed by the string display routines |
| rather than the interrupt so that the interrupt      |
| executes as fast as possible.                        |
|-----------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "Arduino.h"


#ifndef BUTTERFLYLCD_H
#define BUTTERFLYLCD_H

// Defines:
#define LCD_LCDREGS_START          ((uint8_t*)&LCDDR0)
#define LCD_SPACE_OR_INVALID_CHAR  0xFF
#define LCD_WAIT_FOR_SCROLL_DONE() do {while (!(ScrollFlags & LCD_FLAG_SCROLL_DONE)) {}} while(0)
#define LCD_SCROLLCOUNT_DEFAULT    6
#define LCD_DELAYCOUNT_DEFAULT     20
#define LCD_TEXTBUFFER_SIZE        20 // Change this if you need more than 20 characters
#define LCD_SEGBUFFER_SIZE         19
#define LCD_DISPLAY_SIZE           6
#define LCD_FLAG_SCROLL            (1 << 0)
#define LCD_FLAG_SCROLL_DONE       (1 << 1)


// Butterfly segment table
const uint16_t PROGMEM LCD_SegTable[] = 
{
  0xEAA8,     // '*'
  0x2A80,     // '+'
  0x4000,     // ','
  0x0A00,     // '-'
  0x0A51,     // '.' Degree sign
  0x4008,     // '/'
  0x5559,     // '0'
  0x0118,     // '1'
  0x1e11,     // '2
  0x1b11,     // '3
  0x0b50,     // '4
  0x1b41,     // '5
  0x1f41,     // '6
  0x0111,     // '7
  0x1f51,     // '8
  0x1b51,     // '9'
  0x0000,     // ':' (Not defined)
  0x0000,     // ';' (Not defined)
  0x8008,     // '<'
  0x1A00,     // '='
  0x4020,     // '>'
  0x0000,     // '?' (Not defined)
  0x0000,     // '@' (Not defined)
  0x0f51,     // 'A' (+ 'a')
  0x3991,     // 'B' (+ 'b')
  0x1441,     // 'C' (+ 'c')
  0x3191,     // 'D' (+ 'd')
  0x1e41,     // 'E' (+ 'e')
  0x0e41,     // 'F' (+ 'f')
  0x1d41,     // 'G' (+ 'g')
  0x0f50,     // 'H' (+ 'h')
  0x2080,     // 'I' (+ 'i')
  0x1510,     // 'J' (+ 'j')
  0x8648,     // 'K' (+ 'k')
  0x1440,     // 'L' (+ 'l')
  0x0578,     // 'M' (+ 'm')
  0x8570,     // 'N' (+ 'n')
  0x1551,     // 'O' (+ 'o')
  0x0e51,     // 'P' (+ 'p')
  0x9551,     // 'Q' (+ 'q')
  0x8e51,     // 'R' (+ 'r')
  0x9021,     // 'S' (+ 's')
  0x2081,     // 'T' (+ 't')
  0x1550,     // 'U' (+ 'u')
  0x4448,     // 'V' (+ 'v')
  0xc550,     // 'W' (+ 'w')
  0xc028,     // 'X' (+ 'x')
  0x2028,     // 'Y' (+ 'y')
  0x5009,     // 'Z' (+ 'z')
  0x1441,     // '['
  0x8020,     // '\'
  0x1111,     // ']'
  0x0000,     // '^' (Not defined)
  0x1000      // '_'
};

// Needs to be outside the class because of the ISR
static volatile uint8_t TextBuffer[LCD_TEXTBUFFER_SIZE /*LCD text*/ + LCD_DISPLAY_SIZE /*Nulls for scrolling*/ + 1 /*Null termination*/];
static volatile uint8_t StrStart = 0;
static volatile uint8_t StrEnd = 0;
static volatile uint8_t ScrollCount = 0;
static volatile uint8_t UpdateDisplay = false;
static volatile uint8_t ShowColons = false;
static volatile uint8_t ClearNext = false;
static volatile uint8_t ScrollFlags;
static volatile uint8_t cursorPosition = 0;
static volatile bool clearDisplay = false;
static volatile bool lcdUpdated = false;

class ButterflyLCD
{
  public:
    // Constructor
    ButterflyLCD(void);

    // Public methods
    void begin(void);
    void setContrast(uint8_t level = 0x0f);
    void setCursor(uint8_t position = 0);
    void wait();
    void print(String);
    void print_f(const char *FlashData);
    void print(const char Data[]);//const char *Data);
    void print(int32_t number, uint8_t base = DEC);
    void clear(void);
    void showColons(const uint8_t ColonsOn);
    void write(uint8_t);

  private:
    // Private methods
    void appendc(char Data);
    static inline void writeChar(const uint8_t Byte, const uint8_t Digit);
    void printNumber(uint32_t number, uint8_t base = DEC, bool table = false);
};

#endif
