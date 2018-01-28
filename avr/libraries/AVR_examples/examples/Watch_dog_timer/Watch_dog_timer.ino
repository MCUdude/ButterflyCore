/********************************************************* 
 This sketch demonstrates the use of a watchdog timer.
 The watchdog timer causes a processor reset to occur if
 it is allowed to time out. When the sketch are running
 inside the second while loop, the watch dog timer is
 consistently resat to prevent a timeout. When PB0 are 
 pulled high, it breaks put of the while loop and causes 
 the watch dog timer to reset the processor.
 ********************************************************/ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>


uint16_t counter;

int main(void)
{
  DDRB |= 0x10; 		 // Set PB4 as output, ignore the rest
  //DDRB |= _BV(PB4);	 // Alternative method
  //DDRB |= (1 << PB4);  // Alternative method

  DDRB &= ~0x11; 		 // Set PB5 as input, ignore the rest
  //DDRB &= _BV(PB5);	 // Alternative method
  //DDRB &= ~(1 << PB5); // Alternative method
  
  PORTB |= 0x11; // Enable pullup on PB1

  
  while((PINB & _BV(PB5)) == 1) // Wait for PB5 to be pulled down
  {
    PORTB ^= 0x10; //Toggle PB4 while waiting
    _delay_ms(100);
  }


  wdt_enable(WDTO_1S); // Enable WDT with 1 second timeout

  while((PINB & _BV(PB5)) == 0) // Wait for PB5 to be pulled high
  {
    wdt_reset(); //Reset WDT while PB5 are pulled low
    
    counter++;

    if(counter >= 50000)
    {
      PORTB ^= 0x10; // Toggle PB4 every 50000th time
      counter = 0;
    }
  }

  
  while(1); // The watchdog will reset the microcontroller after 1 second
}

