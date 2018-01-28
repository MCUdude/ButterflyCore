/*---- ButterflyCore LCD driver - ButterflyLCD.cpp ----|
| Based on the Butteruino project                      |
| https://github.com/davidknaack/butteruino            |
|                                                      |
| Updated and rewritten by MCUdude November 2016       |
| https://github.com/MCUdude/ButterflyCore             |
|                                                      |
| Released to the public domain                         |
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

// Include header
#include "ButterflyLCD.h"


/*
  NAME:      | ButterflyLCD
  PURPOSE:   | Constructs ButterflyLCD
  ARGUMENTS: | None
  RETURNS:   | None
*/
ButterflyLCD::ButterflyLCD(void)
{
  ScrollFlags = 0;
}


/*
  NAME:      | begin
  PURPOSE:   | Initializes the Butterfly's LCD for correct operation and clears the display
  ARGUMENTS: | None
  RETURNS:   | None
*/
void ButterflyLCD::begin(void)
{
  // Set the initial contrast level to maximum:
  setContrast(0x0f);

  // Select asynchronous clock source, enable all COM pins and enable all segment pins:
  LCDCRB  = (1 << LCDCS) | (3 << LCDMUX0) | (7 << LCDPM0);

  // Set LCD prescaler to give a framerate of 64Hz:
  LCDFRR  = (0 << LCDPS0) | (3 << LCDCD0);

  // Enable LCD and set low power waveform, enable start of frame interrupt:
  LCDCRA  = (1 << LCDEN) | (1 << LCDAB) | (1 << LCDIE);

  // Clear LCD
  clear();

  // Wait for the ISR to occur
  wait();
}



/*
  NAME:      | setContrast
  PURPOSE:   | Sets the cursor on the LCD
  ARGUMENTS: | position (default 0)
  RETURNS:   | None
*/
void ButterflyLCD::setCursor(uint8_t position)
{
  cursorPosition = position;
}


/*
  NAME:      | setCursor
  PURPOSE:   | Sets the contrast on the LCD, where 0x00 is the lowest, and 0x0f is the highest
  ARGUMENTS: | None
  RETURNS:   | None
*/
void ButterflyLCD::setContrast(uint8_t level)
{
  LCDCCR = (0x0f & level);
}


/*
  NAME:      | wait
  PURPOSE:   | waits for the ISR to occur and update the LCD
  ARGUMENTS: | None
  RETURNS:   | None
*/
void ButterflyLCD::wait()
{
  // Wait for the LCD to be updated by the ISR
  while(UpdateDisplay == true);
}


/*
  NAME:      | putS_f
  PURPOSE:   | Displays a string from flash onto the Butterfly's LCD
  ARGUMENTS: | Pointer to the start of the flash string
  RETURNS:   | None
*/
void ButterflyLCD::print_f(const char *FlashData)
{
  /* Rather than create a new buffer here (wasting RAM), the TextBuffer global
    is re-used as a temp buffer. Once the ASCII data is loaded in to TextBuffer,
    LCD_puts is called with it to post-process it into the correct format for the
    LCD interrupt.                                                                */

  strcpy_P((char*)&TextBuffer[0], FlashData);
  print((char*)&TextBuffer[0]);
}


/*
  NAME:      | print
  PURPOSE:   | Displays a string from SRAM onto the Butterfly's LCD
  ARGUMENTS: | Pointer to the start of the SRAM string
  RETURNS:   | None
*/
void ButterflyLCD::print(const char Data[])//const char *Data)
{
  ClearNext       = false;  // print always clears the current output
  uint8_t LoadB       = 0;
  uint8_t CurrByte;

  do
  {
    CurrByte = *(Data++);

    switch (CurrByte)
    {
      case 'a'...'z':
        CurrByte &= ~(1 << 5);                        // Translate to upper-case character
      case '*'...'_':                                 // Valid character, load it into the array
        TextBuffer[LoadB++] = (CurrByte - '*');
        break;
      case 0x00:                                      // Null termination of the string - ignore for now so the nulls can be appended below
        break;
      default:                                        // Space or invalid character, use 0xFF to display a blank
        TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
    }
  }
  while (CurrByte && (LoadB < LCD_TEXTBUFFER_SIZE));

  ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE) ? LCD_FLAG_SCROLL : 0x00);

  for (uint8_t Nulls = 0; Nulls < 7; Nulls++)
    TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping

  TextBuffer[LoadB] = 0x00;                           // Null-terminate string

  StrStart      = 0;
  StrEnd        = LoadB;
  ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
  UpdateDisplay = true;

  // Wait for the ISR to occur
  wait();
}


/*
  NAME:      | print
  PURPOSE:   | prints signed integer numbers
  ARGUMENTS: | number to print, base
  RETURNS:   | None
*/
void ButterflyLCD::print(int32_t number, uint8_t base)
{
  if(base > 1)
  {
    if(number < 0) 
    {
      number = -number;
      printNumber(number, base, true);   // Add a minus in front
    }
    else 
      printNumber(number, base, false); // Don't add anything
  }
}



/*
  NAME:      | clear
  PURPOSE:   | Clear the LCD
  ARGUMENTS: | None
  RETURNS:   | None
*/
void ButterflyLCD::clear(void)
{
  for(uint8_t Nulls = 0; Nulls < (LCD_DISPLAY_SIZE + 1); Nulls++)
  {
    TextBuffer[Nulls] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping
  }
  
  TextBuffer[LCD_DISPLAY_SIZE + 1] = 0x00;          // Null-terminate string

  ClearNext     = false;
  StrStart      = 0;
  StrEnd        = LCD_DISPLAY_SIZE + 1;
  ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
  UpdateDisplay = true;
  clearDisplay  = true;

  // Wait for the ISR to occur
//  wait();
}


/*
  NAME:      | showColons
  PURPOSE:   | Routine to turn on or off the LCD's colons
  ARGUMENTS: | Boolean - true to turn on colons
  RETURNS:   | None
*/
void ButterflyLCD::showColons(const uint8_t ColonsOn)
{
  ShowColons    = ColonsOn;
  UpdateDisplay = true;

  // Wait for the ISR to occur
  wait();
}


/*
  NAME:      | write
  PURPOSE:   | Routine to print characters to the LCD. This used by the print routines to output chars
  ARGUMENTS: | ASCII character to print
  RETURNS:   | None
*/
void ButterflyLCD::write(uint8_t b)
{
  appendc((char)b);
}



/********************************
******** PRIVATE METHODS ********
********************************/

/*
  NAME:      | appendc
  PURPOSE:   | Appends a character from SRAM onto the Butterfly's LCD
       This is primarly used for interfacing to the Write
       function from the print class.
  ARGUMENTS: | The charater to append
  RETURNS:   | None
*/
void ButterflyLCD::appendc(char Data)
{
  // ClearNext indicates that a println was used and so new lines should first clear old lines.
  if (ClearNext)
    clear();

  uint8_t LoadB = StrEnd - LCD_DISPLAY_SIZE - 1;

  // If no more room is available in the buffer, shift
  // the contents back and drop the first character.
  if (LoadB == LCD_TEXTBUFFER_SIZE) {
    for (uint8_t i = 0; i < LCD_TEXTBUFFER_SIZE - 1; i++)
      TextBuffer[i] = TextBuffer[i + 1];
    LoadB = LCD_TEXTBUFFER_SIZE - 1;
  }

  switch (Data)
  {
    case '\n':                               // Writing a newline will cause a clear before the next write
      ClearNext = true;
      break;
    case 'a'...'z':
      Data &= ~(1 << 5);                     // Translate to upper-case character
    case '*'...'_':                          // Valid character, load it into the array
      TextBuffer[LoadB++] = (Data - '*');
      break;
    case 0x00:                               // Null termination of the string - ignore for now so the nulls can be appended below
      break;
    default:                                 // Space or invalid character, use 0xFF to display a blank
      TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
  }

  ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE) ? LCD_FLAG_SCROLL : 0x00);

  for (uint8_t Nulls = 0; Nulls < 7; Nulls++)
    TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping

  TextBuffer[LoadB] = 0x00;                           // Null-terminate string

  StrStart      = 0;
  StrEnd        = LoadB;
  ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
  UpdateDisplay = true;

  // Wait for the ISR to occur
  wait();
}



void ButterflyLCD::printNumber(uint32_t number, uint8_t base, bool table)
{
  char buf[8 * sizeof(long) + 1 + table]; // Assumes 8-bit chars plus zero byte
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';
  
  do 
  {
    char c = number % base;
    number /= base;
    
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  }
  while(number);

  // Add minus if negative number
  if(table == true)
    *--str = '-';
   print(str);
}


/*
  NAME:      | writeChar (static, inline)
  PURPOSE:   | Routine to write a character to the correct LCD registers for display
  ARGUMENTS: | Character to display, LCD character number to display character on
  RETURNS:   | None
*/
static inline void writeChar(const uint8_t Byte, const uint8_t Digit)
{
  uint8_t* BuffPtr = (uint8_t*)(LCD_LCDREGS_START + (Digit >> 1));
  uint16_t SegData = 0x0000;

  if (Byte != LCD_SPACE_OR_INVALID_CHAR)              // Null indicates invalid character or space
    SegData = pgm_read_word(&LCD_SegTable[Byte]);

  for (uint8_t BNib = 0; BNib < 4; BNib++)
  {
    uint8_t MaskedSegData = (SegData & 0x0000F);

    if (Digit & 0x01)
      *BuffPtr = ((*BuffPtr & 0x0F) | (MaskedSegData << 4));
    else
      *BuffPtr = ((*BuffPtr & 0xF0) | MaskedSegData);

    BuffPtr += 5;
    SegData >>= 4;
  }
}


/*
  NAME:      | LCD_vect (ISR, blocking)
  PURPOSE:   | ISR to handle the display and scrolling of the current display string onto the LCD
  ARGUMENTS: | None
  RETURNS:   | None
*/
ISR(LCD_vect)
{
  // Store current cursor position
  uint8_t previousCursor = cursorPosition;
  if (ScrollFlags & LCD_FLAG_SCROLL)
  {
    if (!(ScrollCount--))
    {
      UpdateDisplay = true;
      ScrollCount   = LCD_SCROLLCOUNT_DEFAULT;
    }
  }

  if (UpdateDisplay)
  {
    // Should the whole display be cleared?
    if (clearDisplay == true)  
      cursorPosition = 0;
    
    for (uint8_t Character = cursorPosition; Character < LCD_DISPLAY_SIZE; Character++)
    {
      uint8_t Byte = (StrStart + Character - cursorPosition);

      if (Byte >= StrEnd)
        Byte -= StrEnd;

      writeChar(TextBuffer[Byte], Character);
    }

    if ((StrStart + LCD_DISPLAY_SIZE) == StrEnd)    // Done scrolling message on LCD once
      ScrollFlags |= LCD_FLAG_SCROLL_DONE;

    if (StrStart++ == StrEnd)
      StrStart = 1;

    if (ShowColons)
      *((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x01;
    else
      *((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x00;

    UpdateDisplay = false;                          // Clear LCD management flags, LCD update is complete
    clearDisplay = false;
    cursorPosition = previousCursor;
  }
}
