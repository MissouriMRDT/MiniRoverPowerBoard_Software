// Mini Rover Power Board Software
// Main Header File
//
// Created 2020 by Jacob Lipina, jrlwd5@mst.edu, (314)795-2263
//
//
// Libraries ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MiniRoverPowerBoard_Software.h"

// Setup & Main Loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(9600);

  pinMode(PACK_IMEAS_PIN, INPUT);
  pinMode(V_ACT_SENSE_PIN, INPUT);
  pinMode(V_LOG_SENSE_PIN, INPUT);
  pinMode(RIGHT_SW_PIN, INPUT);
  pinMode(LEFT_SW_PIN, INPUT);

  pinMode(BUZZER_EN_PIN, OUTPUT);
  pinMode(ESTOP_EN_PIN, OUTPUT);
  pinMode(LOG_SW_EN_PIN, OUTPUT);
  pinMode(CONTROLLER_DAT_PIN, OUTPUT);
  pinMode(CONTROLLER_ATT_PIN, OUTPUT);
  pinMode(CONTROLLER_CMD_PIN, OUTPUT);
  pinMode(CONTROLLER_CLK_PIN, OUTPUT);
  pinMode(VNH_IN_A1_PIN, OUTPUT);
  pinMode(VNH_IN_A2_PIN, OUTPUT);
  pinMode(VNH_IN_B1_PIN, OUTPUT);
  pinMode(VNH_IN_B2_PIN, OUTPUT);

  Serial.println("Setup Complete.");
}

void loop() 
{
  uint16_t main_current, act_voltage, log_voltage;
  int num_loop = 0;
  
  getMainCurrent(main_current);
  reactOverCurrent();

  getLogVoltage(log_voltage);
  reactUnderVoltage();
  reactLowVoltage(log_voltage);

  getActVoltage(act_voltage);
  reactEstopReleased();
  reactForgottenLogicSwitch();

  num_loop ++;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Static Variables for Below Functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Current 
static int num_overcurrent = 0;
static bool overcurrent_state = false;
static float time_of_overcurrent = 0;
  
  //Voltage
static bool pack_undervoltage_state = false;
static bool low_voltage_state = false;
static int num_low_voltage_reminder = 0;
static int time_of_low_voltage = 0;

  //Logic Switch
static bool forgotten_logic_switch = false;
static int num_out_voltage_loops = 0;
static int time_switch_forgotten = 0;
static int time_switch_reminder = 0;
static bool estop_released_beep = false;


// Functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getMainCurrent(uint16_t &main_current)
{
	//Serial.print("adc main current: ");
	//Serial.println(analogRead(PACK_IMEAS_PIN));
 
  	main_current = map(analogRead(PACK_IMEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX); //*1069)/1000); //only scale if measurment too inacurate

	//Serial.print("Main current: ");
	//Serial.println(main_current);
  	if(main_current > OVERCURRENT)
  	{
  	  delay(DEBOUNCE_DELAY);
  	  if(map(analogRead(PACK_IMEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) > OVERCURRENT)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactOverCurrent()
{
  if(overcurrent_state == true)
  {
    if(num_overcurrent == 0)
    {
      digitalWrite(ESTOP_EN_PIN, LOW);
    
      time_of_overcurrent = millis(); 
      num_overcurrent++;
    
      notifyOverCurrent();
    }//end if
    else if(num_overcurrent == 1)
    {
      if(millis() >= (time_of_overcurrent + RESTART_DELAY))
      {
        digitalWrite(ESTOP_EN_PIN, HIGH);
      }//end if
      if(millis() >= (time_of_overcurrent + RESTART_DELAY + RECHECK_DELAY))
      {
        overcurrent_state = false;
        num_overcurrent = 0;
        time_of_overcurrent = 0;
      }//end if
    }//end else if
    else
    {
      digitalWrite(ESTOP_EN_PIN, LOW);

      notifyOverCurrent();

      digitalWrite(LOG_SW_EN_PIN, HIGH); //Mini Suicide   
    }//end else
  }//end if
  return;
}//end func

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getLogVoltage(uint16_t &log_voltage)
{  
  int adc_read = analogRead(V_LOG_SENSE_PIN);
  //Serial.print("adc log voltage : ");
  //Serial.println(adc_read);
 
  log_voltage = map(adc_read, PACK_V_ADC_MIN, PACK_V_ADC_MAX, PACK_VOLTS_MIN, PACK_VOLTS_MAX); //Mult by 1215 then divide by 1000 if scaling needed
  //Serial.print("mapped log voltage : ");
  //Serial.println(log_voltage);

  if((log_voltage > PACK_SAFETY_LOW) && (log_voltage < PACK_UNDERVOLTAGE))
  {
    delay(DEBOUNCE_DELAY);
    if(((map(analogRead(V_LOG_SENSE_PIN), PACK_V_ADC_MIN, PACK_V_ADC_MAX, PACK_VOLTS_MIN, PACK_VOLTS_MAX)) < PACK_UNDERVOLTAGE)
        && ((map(analogRead(V_LOG_SENSE_PIN), PACK_V_ADC_MIN, PACK_V_ADC_MAX, PACK_VOLTS_MIN, PACK_VOLTS_MAX) > PACK_SAFETY_LOW)))
    {
      pack_undervoltage_state = true;
    }//end if
  }//end if    
  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactUnderVoltage()
{
  //Serial.println("reactUnderVoltage");
  if(pack_undervoltage_state == true)
  {
    digitalWrite(ESTOP_EN_PIN, LOW);
    notifyUnderVoltage();
    digitalWrite(LOG_SW_EN_PIN, HIGH); //Mini Suicide
  }//end if
  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactLowVoltage(uint16_t log_voltage)
{
  //Serial.println("reactLowVoltage");
  if((log_voltage > PACK_UNDERVOLTAGE) && (log_voltage <= PACK_LOWVOLTAGE) && (low_voltage_state = false))//first instance of low voltage
  { 
    low_voltage_state = true;
    notifyLowVoltage();
    time_of_low_voltage = millis();
    num_low_voltage_reminder = 1;
  }//end if
  else if((log_voltage > PACK_UNDERVOLTAGE) && (log_voltage <= PACK_LOWVOLTAGE) && (low_voltage_state = true))//following instances of low voltage
  {
    if(millis() >= (time_of_low_voltage + (num_low_voltage_reminder * LOGIC_SWITCH_REMINDER)))
    { 
      notifyLowVoltage();
      num_low_voltage_reminder++;
    }//end if
  }//end else if
  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getActVoltage(uint16_t &act_voltage)
{
  int adc_reading = analogRead(V_ACT_SENSE_PIN);
  //Serial.print("adc vout : ");
  //Serial.println(adc_reading);

  act_voltage = map(adc_reading, PACK_V_ADC_MIN, PACK_V_ADC_MAX, PACK_VOLTS_MIN, PACK_VOLTS_MAX);
  //Serial.print("mapped vout : ");
  //Serial.println(act_voltage);

  if(act_voltage > PACK_SAFETY_LOW)
  {
    forgotten_logic_switch = false;
    time_switch_forgotten = 0;
    num_out_voltage_loops = 0;
  }//end if
  if(act_voltage < PACK_EFFECTIVE_ZERO)
  {
    delay(DEBOUNCE_DELAY);

    if((map(analogRead(V_ACT_SENSE_PIN), PACK_V_ADC_MIN, PACK_V_ADC_MAX, PACK_VOLTS_MIN, PACK_VOLTS_MAX)) < PACK_EFFECTIVE_ZERO)
    {
      forgotten_logic_switch = true;
      estop_released_beep = false;
      num_out_voltage_loops++;
    }//end if 
  }//end if
  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactForgottenLogicSwitch()
{
  if(forgotten_logic_switch == true)
  {
    if(num_out_voltage_loops == 1)
    {
      time_switch_forgotten = millis();
      time_switch_reminder = millis();
    }//end if
    if(num_out_voltage_loops > 1)
    {
      if(millis() >= time_switch_reminder + LOGIC_SWITCH_REMINDER)
      {
        time_switch_reminder = millis();
        notifyLogicSwitch();
      }//end if
      if(millis() >= time_switch_forgotten + IDLE_SHUTOFF_TIME)
      {
        digitalWrite(LOG_SW_EN_PIN, HIGH); //Mini Suicide 
      }//end if   
    }//end if 
  }//end if
  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactEstopReleased()
{
  if(forgotten_logic_switch == false && estop_released_beep == false)
  {
    estop_released_beep = true;
    notifyEstopReleased();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyEstop() //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyLogicSwitch() //Buzzer sound: beeep beeep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyEstopReleased() //Buzzer sound: beep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(75);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void notifyReboot() //Buzzer sound: beeeeeeeeeep beeep beeep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_EN_PIN, LOW);
  
  return;
}//end func*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyOverCurrent() //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(3000);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyUnderVoltage() //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(150);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(150);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(150);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(150);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(2000);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyLowVoltage() //Buzzer Sound: beeep beeep beeep
{
  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(250);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);
  delay(250);

  digitalWrite(BUZZER_EN_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_EN_PIN, LOW);

  return;
}//end func
