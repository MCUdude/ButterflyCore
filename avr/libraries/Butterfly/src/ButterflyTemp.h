/* AVR Butterfly temperature sensor library */
#include "Arduino.h"

#ifndef BUTTERFLYTEMP_h
#define BUTTERFLYTEMP_h

#define CELSIUS     0
#define FAHRENHEIT  1

// The NTC is connected to PF0, analog pin 0
#define NTCPIN A0


// This table is from the AVR Butterfly sample from ATMEL
// Positive Fahrenheit temps (ADC-value) for 0 to 140 degrees F
const uint16_t PROGMEM TEMP_Fahrenheit_pos[] =  
{   
  938, 935, 932, 929, 926, 923, 920, 916, 913, 909, 906, 902, 898, 
  894, 891, 887, 882, 878, 874, 870, 865, 861, 856, 851, 847, 842, 
  837, 832, 827, 822, 816, 811, 806, 800, 795, 789, 783, 778, 772, 
  766, 760, 754, 748, 742, 735, 729, 723, 716, 710, 703, 697, 690, 
  684, 677, 670, 663, 657, 650, 643, 636, 629, 622, 616, 609, 602, 
  595, 588, 581, 574, 567, 560, 553, 546, 539, 533, 526, 519, 512, 
  505, 498, 492, 485, 478, 472, 465, 459, 452, 446, 439, 433, 426, 
  420, 414, 408, 402, 396, 390, 384, 378, 372, 366, 360, 355, 349, 
  344, 338, 333, 327, 322, 317, 312, 307, 302, 297, 292, 287, 282, 
  277, 273, 268, 264, 259, 255, 251, 246, 242, 238, 234, 230, 226, 
  222, 219, 215, 211, 207, 204, 200, 197, 194, 190, 187
};


// This table is from the AVR Butterfly sample from ATMEL
// Positive Celsius temps (ADC-value) for 0 to 60 degrees C
const uint16_t PROGMEM TEMP_Celsius_pos[] =    
{ 
  806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
  636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
  452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
  297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
  187
};
  

// This table is from the AVR Butterfly sample from ATMEL
// Negative Celsius temps (ADC-value) from -1 to -15 degrees C
const uint16_t PROGMEM TEMP_Celsius_neg[] = {815,825,834,843,851,860,868,876,883,891,898,904,911,917,923};


class ButterflyTemp
{
  public:
    // Constructor
    ButterflyTemp(int16_t units);

    // Public methods
    int16_t getTemp();
    int16_t getTemp(int16_t units);
    void overSample(bool enable);
    
  private:
    // Private methods
    int16_t mapToF(int16_t a2d);
    int16_t mapToC(int16_t a2d);   

    // Private variables
    uint8_t _temperatureUnit;
    bool _overSample = false;

};

#endif
