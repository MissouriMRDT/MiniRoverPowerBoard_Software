// Mini Rover Power Board Software
// Main Header File
//
// Created 2020 by Jacob Lipina, jrlwd5@mst.edu, (314)795-2263
//
//
// Libraries ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MiniRoverPowerBoard_Software.h"

// Setup & Main Loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t main_current, act_voltage, log_voltage;

void setup() 
{
  Serial.begin(9600);

}

void loop() 
{
  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getMainCurrent(int16_t &main_current)
{
	//Serial.print("adc current:  ");
	//Serial.println(analogRead(PACK_I_MEAS_PIN));
 
  	main_current = ((map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX)*1069)/1000);//*950)/1000);

	//Serial.print("current:  ");
	//Serial.println(main_current);
  	if(main_current > OVERCURRENT)
  	{
  	  delay(DEBOUNCE_DELAY);
  	  if(map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) > OVERCURRENT)
  	  {
  	    overcurrent_state = true;
	
  	    if(num_overcurrent == 1)
  	    {
  	      num_overcurrent++;
  	    }//end if 
  	  }//end if 
  	}//end if
  	return;
}//end func