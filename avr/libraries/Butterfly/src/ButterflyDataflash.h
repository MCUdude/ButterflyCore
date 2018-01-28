/*-- ButterflyCore dataflash - ButterflyDataflash.h ---|
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

#ifndef dataflash_h
#define dataflash_h

#include <stdint.h> 

#define PageBits 9
#define PageSize 264

#define DF_CS_inactive PORTB |= _BV(0)
#define  DF_CS_active PORTB &= ~_BV(0)
#define DF_reset DF_CS_inactive; DF_CS_active

//Dataflash opcodes
#define FlashPageRead           0x52  // Main memory page read
#define FlashToBuf1Transfer     0x53  // Main memory page to buffer 1 transfer
#define Buf1Read                0x54  // Buffer 1 read
#define FlashToBuf2Transfer     0x55  // Main memory page to buffer 2 transfer
#define Buf2Read                0x56  // Buffer 2 read
#define StatusReg               0x57  // Status register
#define AutoPageReWrBuf1        0x58  // Auto page rewrite through buffer 1
#define AutoPageReWrBuf2        0x59  // Auto page rewrite through buffer 2
#define FlashToBuf1Compare      0x60  // Main memory page to buffer 1 compare
#define FlashToBuf2Compare      0x61  // Main memory page to buffer 2 compare
#define ContArrayRead           0x68  // Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1           0x82  // Main memory page program through buffer 1
#define Buf1ToFlashWE           0x83  // Buffer 1 to main memory page program with built-in erase
#define Buf1Write               0x84  // Buffer 1 write
#define FlashProgBuf2           0x85  // Main memory page program through buffer 2
#define Buf2ToFlashWE           0x86  // Buffer 2 to main memory page program with built-in erase
#define Buf2Write               0x87  // Buffer 2 write
#define Buf1ToFlash             0x88  // Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash             0x89  // Buffer 2 to main memory page program without built-in erase
#define PageEraseCmd            0x81  // Page erase, added by Martin Thomas
#define EnterDeepPowerdown      0xB9  // Enter Deep Powerdown mode
#define ExitDeepPowerdown       0xAB  // Exit Deep powerdown mode


class ButterflyDataflash
{
  public:
    // Constructor
  	ButterflyDataflash(void);
  
    // Public methods
  	uint8_t ReadDFStatus (void);
  	void Activate(void);
  	void Deactivate(void);
  	
  	void EnterDeepPowerDown(void);
  	void ExitDeepPowerDown(void);
  
  	void BufferToPage(uint8_t BufferNo, uint16_t PageAdr);
  	void PageToBuffer(uint16_t PageAdr, uint8_t BufferNo);
  
  	void ContFlashReadEnable(uint16_t PageAdr, uint16_t IntPageAdr);
  	void BufferReadEnable(uint8_t BufferNo, uint16_t IntPageAdr);
  	uint8_t BufferReadByte(uint8_t BufferNo, uint16_t IntPageAdr);
  	void BufferReadStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
  	uint8_t ReadNextByte(void);
  	
  	void BufferWriteEnable(uint8_t BufferNo, uint16_t IntPageAdr);
  	void BufferWriteByte(uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data);
  	void BufferWriteStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
  	void WriteNextByte(uint8_t data);
  
  	uint8_t PageBufferCompare(uint8_t BufferNo, uint16_t PageAdr);
  	void PageErase(uint16_t PageAdr);
  
  private:
    // Private methods
    void DF_SPI_init(void);
    uint8_t DF_SPI_RW(uint8_t output);
};

#endif
