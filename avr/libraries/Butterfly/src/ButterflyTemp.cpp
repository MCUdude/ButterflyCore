#include "ButterflyTemp.h"
#include "Arduino.h"




ButterflyTemp::ButterflyTemp(int temperatureUnit)
{
    _temperatureUnit = temperatureUnit;
    _overSample = false;
}

int16_t ButterflyTemp::getTemp()
{
  return getTemp(_temperatureUnit);
}

int16_t ButterflyTemp::getTemp(int unit)
{
  // get the first sample from the temp sensor
  int v = analogRead(NTCPIN);

  // if using oversampling add 7 more samples and then divide by 8
  if(_overSample)
  {
    for(uint8_t i = 0; i < 7; i++)    
      v += analogRead(NTCPIN);
    v >>= 3;
  }
  
  // convert the a2d reading to temperature, depending on unit setting
  switch (unit) 
  {
    case CELSIUS:
      v = mapToC(v);
      break;
      
    case FAHRENHEIT:
      v = mapToF(v);
      break;
  }
  
  return v;
}


int16_t ButterflyTemp::mapToF(int16_t a2d)
{  
  uint8_t i;
  for (i=0; i<=141; i++){   // Find the temperature
    if ((uint16_t)a2d > pgm_read_word_near( TEMP_Fahrenheit_pos +i )){
       break;
    }
  }     
  return i;
}


int16_t ButterflyTemp::mapToC(int16_t a2d)
{
  int16_t v = 0;
  
  if(a2d > 810) // If it's a negative temperature
  {   
    for (uint8_t i = 0; i <= 25; i++){   // Find the temperature
      if ((uint16_t)a2d <= pgm_read_word_near(TEMP_Celsius_neg + i))
      {
        v = 0-i; // Make it negative
        break;
      }
    }
  } 
  
  else if (a2d < 800) // If it's a positive temperature
  {  
    for (uint8_t i = 0; i < 100; i++) 
    {
      if ((uint16_t)a2d >= pgm_read_word_near(TEMP_Celsius_pos + i))
      {
        v = i; 
        break;
      }
    }        
  }
  return v;
}

void ButterflyTemp::overSample(bool enable)
{
  // Enable oversampling if enable is true
  _overSample = enable;
}

