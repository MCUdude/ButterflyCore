//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: ButterflyDataflash.cpp
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: All AVRs with built-in HW SPI
//
//  Description.: Functions to access the Atmel AT45Dxxx dataflash onboard
//                Butterfly development board
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20011017 - 1.00 - Beta release                                  -  RM
//  20011017 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20040121          added compare and erase function              - M.Thomas
//  20081228          Converted to Arduino Library for Butterfly	  - Dave K
//  20161114          Updated for Arduino 1.6                       - MCUdude
//
//*****************************************************************************

/* 
   remark mthomas: If you plan to use the dataflash functions in own code
   for (battery powered) devices: disable the "chip select" after accessing
   the Dataflash. The current draw with cs enabled is "very" high. You can
   simply use the macro DF_CS_inactive already defined by Atmel after every
   DF access.

   The coin-cell battery on the Butterfly is not a reliable power-source if data
   in the flash-array should be changed (write/erase).
   See the Dataflash datasheet for the current needed during write-accesses.
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ButterflyDataflash.h"



/*****************************************************************************
*
*	Function name : ButterflyDataflash()
*
*	Returns :		Instance
*
*	Parameters :	None
*
*	Purpose :		Constructor
*
******************************************************************************/
ButterflyDataflash::ButterflyDataflash(void)
{
	DF_SPI_init();
}


/*****************************************************************************
*
*	Function name : Activate
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets chip select to activate dataflash chip
*
******************************************************************************/
void ButterflyDataflash::Activate(void)
{
	DF_CS_active;							//to reset dataflash command decoder
}



/*****************************************************************************
*
*	Function name : Deactivate
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Clears chip select to deactivate dataflash chip.
*					This is useful to save power.
*
******************************************************************************/
void ButterflyDataflash::Deactivate(void)
{
	DF_CS_inactive;							//make sure to toggle CS signal in order
}



/*****************************************************************************
*
*	Function name : EnterDeepPowerDown
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Put the chip into deep power-down mode
*
*					After initial power-up, the device will default in standby
*					mode. The Deep Power-down command allows the device to enter
*					into the lowest power consumption mode. 

*					To enter the Deep Power-down mode, the CS pin must first be 
*					asserted. Once the CS pin has been asserted, an opcode 
*					of B9H command must be clocked in via input pin (SI). 
*					After the last bit of the command has been clocked in, 
*					the CS pin must be de-asserted to initiate the Deep 
*					Power-down operation.
*					
*					After the CS pin is de-asserted, the will device enter the 
*					Deep Power-down mode within 3uS. Once the device has entered 
*					the Deep Power-down mode, all instructions are ignored except 
*					for the Resume from Deep Power-down command.
*
*					Dataflash current consumption:
*					-  7 to 15 mA Active Read Current Typical
*					- 25 to 50 uA Standby Current Typical
*					-  5 to 10 uA Deep Power-down Typical
*
******************************************************************************/
void ButterflyDataflash::EnterDeepPowerDown(void)
{
	DF_CS_active;						// Assert CS
	DF_SPI_RW (EnterDeepPowerdown);		// Send power-down command
	DF_CS_inactive;						// Deassert CS
}



/*****************************************************************************
*
*	Function name : ExitDeepPowerDown
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Remove the chip into deep power-down mode
*
*					The Resume from Deep Power-down command takes the device
*					out of the Deep Power-down mode and returns it to the normal
*					standby mode. 
*					
*					To Resume from Deep Power-down mode, the CS pin must first
*					be asserted and an opcode of ABH command must be clocked in 
*					via input pin (SI). After the last bit of the command has 
*					been clocked in, the CS pin must be de-asserted to terminate 
*					the Deep Power-down mode. 
*					
*					After the CS pin is de-asserted, the device will return to 
*					the normal standby mode within 35uS. The CS pin must remain 
*					high during this time before the device can receive any 
*					commands. 
*					
*					After resuming form Deep Powerdown, the device will	return 
*					to the normal standby mode.
*
******************************************************************************/
void ButterflyDataflash::ExitDeepPowerDown(void)
{
	DF_CS_active;						// Assert CS
	DF_SPI_RW (ExitDeepPowerdown);		// Send resume from power-down command
	DF_CS_inactive;						// Deassert CS
}



/*****************************************************************************
*
*	Function name : DF_SPI_init
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets up the HW SPI in Master mode, Mode 3
*					Note -> Uses the SS line to control the DF CS-line.
*
******************************************************************************/
void ButterflyDataflash::DF_SPI_init (void)
{
	PORTB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB0);				//Set MOSI, SCK AND SS as outputs
	SPSR = (1<<SPI2X);                                      //SPI double speed settings
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master mode, mode 3, Fosc/4
}



/*****************************************************************************
*
*	Function name : DF_SPI_RW
*
*	Returns :		uint8_t read from SPI data register (any value)
*
*	Parameters :	uint8_t to be written to SPI data register (any value)
*
*	Purpose :		Read and writes one uint8_t from/to SPI master
*
******************************************************************************/
uint8_t ButterflyDataflash::DF_SPI_RW (uint8_t output)
{
	uint8_t input;
	
	SPDR = output;							//put uint8_t 'output' in SPI data register
	while(!(SPSR & _BV(SPIF)))				//wait for transfer complete, poll SPIF-flag
		;
	input = SPDR;							//read value in SPI data reg.
	
	return input;							//return the uint8_t clocked in from SPI slave
}



/*****************************************************************************
*
*	Function name : ReadDFStatus
*
*	Returns :		One status byte. 
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash chip
*
*					The status register can be used to determine the devices 
*					ready/busy status, page size, a Main Memory Page to Buffer 
*					Compare operation result, the Sector Protection status or 
*					the device density. The Status Register can be read at any 
*					time, including during an internally self-timed program or 
*					erase operation. 
*					
*					Ready/busy status is indicated using bit 7 of the status 
*					register. If bit 7 is a 1, then the device is not busy and
*					is ready to accept the next command. If bit 7 is a 0, then
*					the device is in a busy state.
*					
*					The result of the most recent Main Memory Page to Buffer 
*					Compare operation is indicated using bit 6 of the status 
*					register. If bit 6 is a 0, then the data in the main memory 
*					page matches the data in the buffer.
*					
*					Bit 1 in the Status Register is used to provide information
*					to the user whether or not the sector protection has been 
*					enabled or disabled, either by software-controlled method 
*					or hardware-controlled method. A logic 1 indicates that 
*					sector protection has been enabled .
*					
*					Bit 0 in the Status Register indicates whether the page size
*					of the main memory array is configured for "power of 2" binary
*					page size (256 bytes) or the DataFlash standard page size (264
*					
*					The device density is indicated using bits 5, 4, 3, and 2 of
*					the status register. For the AT45DB041D, the four bits are 0111.
*
******************************************************************************/
uint8_t ButterflyDataflash::ReadDFStatus (void)
{
	uint8_t result;
	
	DF_reset;								//reset dataflash command decoder
	
	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result
	
	//device_id = ((result & 0x3C) >> 2);		//get the device id bits, butterfly dataflash should be 0111
	
	return result;							//return the read status register value
}



/*****************************************************************************
*
*	Function name : BufferToPage
*
*	Returns :		None
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be programmed
*
*	Purpose :		Transfers a page from dataflash SRAM buffer to flash
*					
*					Data written into either buffer 1 or buffer 2 can be 
*					programmed into the main memory. A 1-byte opcode, 83H
*					for buffer 1 or 86H for buffer 2, must be clocked into 
*					the device. For the DataFlash standard page size (264 bytes), 
*					the opcode must be followed by three address bytes consist of
*					4 don't care bits, 11 page address bits (PA10 - PA0) that 
*					specify the page in the main memory to be written and 9 don't 
*					care bits.
*
*					When a low-to-high transition occurs on the CS pin, the part
*					will first erase the selected page in main memory (the erased 
*					state is a logic 1) and then program the data stored in the 
*					buffer into the specified page in main memory. Both the erase
*					and the programming of the page are internally self-timed and
*					should take place in a maximum time of 35mS. During this time,
*					the status register will indicate that the part is busy.
*					
*					Page address bit locations within the 24 address bits:
*					xxxxBA98 7654321x  xxxxxxxx
*					
*					The page address range is 0-2047, requiring 11 address bits
*					to express. This is passed in as a 16 bit value in PageAdr:
*					xxxxxBA9 87654321
*					
*					To write this out PageAdr is first shifted right 7 bits so
*					that the low byte forms the first address byte:
*					xxxxxBA98
*					
*					Then PageAdr is shifted left 1 bit so that the low byte forms
*					the second address byte:
*					7654321x
*						
*					The third address byte is written as all zeros.
*					
******************************************************************************/
void ButterflyDataflash::BufferToPage (uint8_t BufferNo, uint16_t PageAdr)
{
	DF_reset;										// reset dataflash command decoder
		
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)								// program flash page from buffer 1
		DF_SPI_RW( Buf1ToFlashWE );					// buffer 1 to flash with erase op-code
	else	
		DF_SPI_RW( Buf2ToFlashWE );					// buffer 2 to flash with erase op-code

	DF_SPI_RW((uint8_t)(PageAdr >> 7));				// upper part of page address
	DF_SPI_RW((uint8_t)(PageAdr << 1));				// lower part of page address
	DF_SPI_RW(0x00);								// don't cares
	
	DF_reset;										// initiate flash page programming
	
	while(!(ReadDFStatus() & 0x80));				// monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : PageToBuffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer

*					A page of data can be transferred from the main memory to
*					either buffer 1 or buffer 2. To start the operation for the
*					DataFlash standard page size (264 bytes), a 1-byte opcode, 
*					53H for buffer 1 and 55H for buffer 2, must be clocked into
*					the device, followed by three address bytes comprised of 4 
*					don't care bits, 11 page address bits (PA10 - PA0), which 
*					specify the page in main memory that is to be transferred, 
*					and 9 don't care bits.
*					
*					The CS pin must be low while toggling the SCK pin to load 
*					the opcode and the address bytes from the input pin (SI). 
*					The transfer of the page of data from the main memory to 
*					the buffer will begin when the CS pin transitions from a 
*					low to a high state. 
*					
*					During the transfer of a page of data (tXFR), the status 
*					register can be read to determine whether the transfer 
*					has been completed
*					
******************************************************************************/
void ButterflyDataflash::PageToBuffer (uint16_t PageAdr, uint8_t BufferNo)
{
	DF_reset;									//reset dataflash command decoder

	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (BufferNo == 1)							//transfer flash page to buffer 1
		DF_SPI_RW(FlashToBuf1Transfer);			//transfer to buffer 1 op-code
	else
		DF_SPI_RW(FlashToBuf2Transfer);			//transfer to buffer 2 op-code

	DF_SPI_RW((uint8_t)(PageAdr >> 7));			//upper part of page address
	DF_SPI_RW((uint8_t)(PageAdr << 1));			//lower part of page address
	DF_SPI_RW(0x00);							//don't cares
	
	DF_reset;									//init transfer
	
	while(!(ReadDFStatus() & 0x80));			//monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : ContFlashReadEnable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*
*					To start a page read from the DataFlash standard page size (264 bytes), 
*					an opcode of D2H must be clocked into the device followed by three 
*					address bytes (which comprise the 24-bit page and byte address sequence) 
*					and 4 don't care bytes. The first 11 bits (PA10 - PA0) of the 20-bit 
*					address sequence specify the page in main memory to be read, and the 
*					last 9 bits (BA8 - BA0) of the 20-bit address sequence specify the 
*					starting byte address within that page.
*
*					The don't care bytes that follow the address bytes are sent to initialize
*					the read operation. Following the don't care bytes, additional pulses 
*					on SCK result in data being output on the SO (serial output) pin. The 
*					CS pin must remain low during the loading of the opcode, the address 
*					bytes, the don't care bytes, and the reading of data. When the end of
*					a page in main memory is reached, the device will continue reading back
*					at the beginning of the same page. 
*					
*					A low-to-high transition on the CS pin will terminate the read operation 
*					and tri-state the output pin (SO).
*
*					The Main Memory Page Read bypasses both data buffers and leaves the
*					contents of the buffers unchanged.
*
******************************************************************************/
void ButterflyDataflash::ContFlashReadEnable (uint16_t PageAdr, uint16_t IntPageAdr)
{
	DF_reset;													//reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);									//Continuous Array Read op-code
	DF_SPI_RW((uint8_t)(PageAdr >> 7));							//upper part of page address
	DF_SPI_RW((uint8_t)((PageAdr << 1) + (IntPageAdr >> 8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((uint8_t)(IntPageAdr));							//LSB uint8_t of internal page address
	DF_SPI_RW(0x00);											//perform 4 dummy writes
	DF_SPI_RW(0x00);											//in order to intiate DataFlash
	DF_SPI_RW(0x00);											//address pointers
	DF_SPI_RW(0x00);
}



/*****************************************************************************
*
*	Function name : BufferReadEnable
*
*	Returns :		none
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*
*	Purpose :		Sets up to read data from one of the dataflash
*					internal SRAM buffers
*
*					To perform a buffer read from the DataFlash standard 
*					buffer (264 bytes), the opcode must be clocked into the 
*					device followed by three address bytes comprised of 15 
*					don't care bits and 9 buffer address bits (BFA8 - BFA0). 
*					
*					Following the address bytes, one don't care byte must be 
*					clocked in to initialize the read operation. The CS pin 
*					must remain low during the loading of the opcode, the 
*					address bytes, the don't care bytes, and the reading of
*					data. When the end of a buffer is reached, the device will 
*					continue reading back at the beginning of the buffer. 
*					
*					A low-to-high transition on the CS pin will terminate the 
*					read operation and tri-state the output pin (SO).
*
******************************************************************************/
void ButterflyDataflash::BufferReadEnable (uint8_t BufferNo, uint16_t IntPageAdr)
{
	DF_reset;									//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//read uint8_t from buffer 1
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code

	DF_SPI_RW(0x00);							//don't cares
	DF_SPI_RW((uint8_t)(IntPageAdr>>8));		//upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));			//lower part of internal buffer address
	DF_SPI_RW(0x00);							//don't cares to initialize the read operation
}



/*****************************************************************************
*
*	Function name : BufferReadByte
*
*	Returns :		One read byte (any value)
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*
*	Purpose :		Reads one byte from one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
uint8_t ButterflyDataflash::BufferReadByte (uint8_t BufferNo, uint16_t IntPageAdr)
{
	BufferReadEnable( BufferNo, IntPageAdr );
	return DF_SPI_RW(0x00);						//read byte
}



/*****************************************************************************
*
*	Function name : BufferReadStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void ButterflyDataflash::BufferReadStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	BufferReadEnable( BufferNo, IntPageAdr );
	for( uint16_t i = 0; i < No_of_bytes; i++) {
		*(BufferPtr) = DF_SPI_RW(0x00);			//read byte and put it in buffer pointed to by *BufferPtr
		BufferPtr++;							//point to next element in buffer
	}
}



/*****************************************************************************
*
*	Function name : ReadNextByte
*
*	Returns :		The next byte read
*
*	Parameters :	None
*
*	Purpose :		After the dataflash is configured to read bytes this
*					routine may be used to read single bytes without resending
*					the read parameters or resetting the chip.
*
******************************************************************************/
uint8_t ButterflyDataflash::ReadNextByte(void)
{
	return DF_SPI_RW(0x00);
}



/*****************************************************************************
*
*	Function name : BufferWriteEnable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					
*	Purpose :		Sets up for writting bytes to the specified buffer.
*					The user must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functions.
*
*					Data can be clocked in from the input pin (SI) into either
*					buffer 1 or buffer 2. To load data into the DataFlash standard
*					buffer (264 bytes), a 1-byte opcode, 84H for buffer 1 or 87H 
*					for buffer 2, must be clocked into the device, followed by 
*					three address bytes comprised of 15 don't care bits and 9 
*					buffer address bits (BFA8 - BFA0). The 9 buffer address bits 
*					specify the first byte in the buffer to be written.
*
*					After the last address byte has been clocked into the device,
*					data can then be clocked in on subsequent clock cycles. If the
*					end of the data buffer is reached, the device will wrap around
*					back to the beginning of the buffer. 
*
*					Data will continue to be loaded into the buffer until a
*					low-to-high transition is detected on the CS pin.
*
******************************************************************************/
void ButterflyDataflash::BufferWriteEnable (uint8_t BufferNo, uint16_t IntPageAdr)
{
	DF_reset;									//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//write enable to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		
	DF_SPI_RW(0x00);							//Don't care
	DF_SPI_RW((uint8_t)(IntPageAdr>>8));		//Upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));			//Lower part of internal buffer address
}



/*****************************************************************************
*
*	Function name : BufferWriteByte
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to write byte to
*					BufferAdr	->	Specifies which buffer to write to
*					Data		->	Data byte to be written
*
*	Purpose :		Writes one byte to one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
void ButterflyDataflash::BufferWriteByte (uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data)
{
	BufferWriteEnable(BufferNo, IntPageAdr);
	DF_SPI_RW(Data);							//write data byte
}



/*****************************************************************************
*
*	Function name : BufferWriteStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash internal
*					SRAM buffers from AVR SRAM buffer pointed to by *BufferPtr
*
******************************************************************************/
void ButterflyDataflash::BufferWriteStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	BufferWriteEnable(BufferNo, IntPageAdr);
	for( uint16_t i = 0; i < No_of_bytes; i++) {
		DF_SPI_RW(*(BufferPtr));	//write byte pointed at by *BufferPtr to dataflash buffer location
		BufferPtr++;				//point to next element in buffer
	}
}



/*****************************************************************************
*
*	Function name : WriteNextByte
*
*	Returns :		Write the next byte
*
*	Parameters :	None
*
*	Purpose :		After the dataflash is configured to write bytes this
*					routine may be used to write single bytes without resending
*					the parameters or resetting the chip.
*
******************************************************************************/
void ButterflyDataflash::WriteNextByte (uint8_t data)
{
	DF_SPI_RW(data);
}



/*****************************************************************************
*
*	Function name : PageBufferCompare
*
*	Returns :		0 match, 1 if mismatch
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be compared with buffer
*
*	Purpose :		comparte Buffer with Flash-Page
*
*					A page of data in main memory can be compared to the data 
*					in buffer 1 or buffer 2. To initiate the operation for the
*					DataFlash standard page size, a 1-byte opcode, 60H for buffer
*					1 and 61H for buffer 2, must be clocked into the device, 
*					followed by three address bytes consisting of 4 don't care 
*					bits, 11 page address bits (PA10 - PA0) that specify the page
*					in the main memory that is to be compared to the buffer, 
*					and 9 don't care bits.
*
*					The CS pin must be low while toggling the SCK pin to load
*					the opcode and the address bytes from the input pin (SI). 
*					On the low-to-high transition of the CS pin, the data bytes
*					in the selected main memory page will be compared with the
*					data bytes in buffer 1 or buffer 2. During this time (tCOMP),
*					the status register will indicate that the part is busy. On 
*					completion of the compare operation, bit 6 of the status 
*					register is updated with the result of the compare.
*
*   added by Martin Thomas, Kaiserslautern, Germany. This routine was not 
*   included by ATMEL
*					
******************************************************************************/
uint8_t ButterflyDataflash::PageBufferCompare(uint8_t BufferNo, uint16_t PageAdr)
{
	uint8_t stat;
	
	DF_reset;										//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)									
		DF_SPI_RW(FlashToBuf1Compare);	
	else
		DF_SPI_RW(FlashToBuf2Compare);						
	
	DF_SPI_RW((uint8_t)(PageAdr >> 7));				//upper part of page address
	DF_SPI_RW((uint8_t)(PageAdr << 1));				//lower part of page address
	DF_SPI_RW(0x00);								//don't cares
	
	DF_reset;										//reset dataflash command decoder
	
	do {
		stat=ReadDFStatus();
	} while(!(stat & 0x80));						//monitor the status register, wait until busy-flag is high
	
	return (stat & 0x40);
}



/*****************************************************************************
*
*	Function name : PageErase
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page to be erased
*
*	Purpose :		Sets all bits in the given page (all bytes are 0xff)
*
*					The Page Erase command can be used to individually erase
*					any page in the main memory array allowing the Buffer to 
*					Main Memory Page Program to be utilized at a later time. 
*					
*					To perform a page erase in the DataFlash standard page 
*					size (264 bytes), an opcode of 81H must be loaded into 
*					the device, followed by three address bytes comprised of 
*					4 don't care bits, 11 page address bits (PA10 - PA0) that 
*					specify the page in the main memory to be erased and 9 
*					don't care bits.
*
*					When a low-to-high transition occurs on the CS pin, the 
*					part will erase the selected page (the erased state is a 
*					logical 1). The erase operation is internally self-timed 
*					and should take place in a maximum time of 32mS. During 
*					this time, the status register will indicate that the part 
*					is busy.
*
*
******************************************************************************/
void ButterflyDataflash::PageErase (uint16_t PageAdr)
{
	DF_reset;										//reset dataflash command decoder

	DF_SPI_RW(PageEraseCmd);						//Page erase op-code
	DF_SPI_RW((uint8_t)(PageAdr >> 7));				//upper part of page address
	DF_SPI_RW((uint8_t)(PageAdr << 1));				//lower part of page address and MSB of int.page adr.
	DF_SPI_RW(0x00);								//dont cares

	DF_reset;										//initiate flash page erase

	while(!(ReadDFStatus() & 0x80));				//monitor the status register, wait until busy-flag is high
}
