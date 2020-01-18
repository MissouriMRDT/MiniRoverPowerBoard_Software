// Mini Rover Power Board Software
// Main Header File
//
// Created 2020 by Jacob Lipina, jrlwd5@mst.edu, (314)795-2263
//
//
// Pinmap /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Sensor Measurments
#define PACK_IMEAS_PIN		A5
#define V_ACT_SENSE_PIN		A4
#define V_LOG_SENSE_PIN		A3

// Enables & Switches
#define BUZZER_EN_PIN		2
#define ESTOP_EN_PIN		3
#define LOG_SW_EN_PIN 		4
#define RIGHT_SW_PIN		11
#define LEFT_SW_PIN			12

// Controller
#define CONTROLLER_DAT_PIN	A2
#define CONTROLLER_ATT_PIN	A1
#define CONTROLLER_CMD_PIN	A0
#define CONTROLLER_CLK_PIN	13

// VNH H-Bridge Chips
#define VNH_IN_A1_PIN		5
#define VNH_IN_A2_PIN		8
#define VNH_IN_B1_PIN		7
#define VNH_IN_B2_PIN	  	10
#define VNH_LEFT_PWM		6
#define VNH_RIGHT_PWM		9

// Constants //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define VCC						3300

// ACS722LLCTR-40AU-T IC Sensor Specs 
#define SENSOR_SENSITIVITY   	0.066    //volts/amp
#define SENSOR_SCALE         	0.1      //volts/amp
#define SENSOR_BIAS           	VCC * SENSOR_SCALE

#define CURRENT_MAX           	40000//mA
#define CURRENT_MIN           	0//mA
#define OVERCURRENT           	35000 //mA
#define CURRENT_ADC_MIN     	0 //bits
#define CURRENT_ADC_MAX     	1024 //bits

// Voltage Measurments
#define PACK_VOLTS_MIN          0 //mV		//Lowest possible pack voltage value we could measure. 
#define PACK_VOLTS_MAX        	8400 //mV	//Highest possible pack voltage value we could measure.
#define PACK_UNDERVOLTAGE   	5400 //mV	//If pack voltage reaches this voltage, turn off rover and BMS suicide.
#define PACK_LOWVOLTAGE     	6100 //mV	//If pack voltage reaches this voltage, notify of low voltage.
#define PACK_SAFETY_LOW     	PACK_UNDERVOLTAGE - 3000 //mV	//Even though I shouldn't be need to measure a voltage lower than undervoltage 
#define PACK_EFFECTIVE_ZERO   	1000 //mV 	//Pack voltage below this value is considered to be adc noise and not valid measurments. This is because the battery cannot physically be this low. So there must be an error with reading the voltage
#define PACK_V_ADC_MIN      	0 //bits
#define PACK_V_ADC_MAX      	4096 //bits

// Delays
#define DEBOUNCE_DELAY      	10 //msec		//After an overcurrent or undervoltage is detected, delay by this and measure the pin again to confirm whether error is occurring.
#define RECHECK_DELAY    		10000 //msec	//Used to measure time since first overcurrent. If second overcurrent occurs within this delay time, BMS commits suicide.
#define RESTART_DELAY     		5000 //msec		//Used after first overcurrent as delay before turning rover back on.
#define LOGIC_SWITCH_REMINDER 	60000 //msec 	//Every cycle of this period of time, the buzzer notifys someone that logic switch was forgotten.
#define IDLE_SHUTOFF_TIME   	600000 //msec or 10 minutes	//After this period of time passes, the BMS will commit suicide.

// Function Declarations //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getMainCurrent(uint16_t &main_current);

void reactOverCurrent();

void getLogVoltage(uint16_t &log_voltage);

void reactUnderVoltage();

void reactLowVoltage(uint16_t log_voltage);

void getActVoltage(uint16_t &act_voltage);

void reactForgottenLogicSwitch();

void reactEstopReleased();

void notifyEstop(); //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep

void notifyLogicSwitch(); //Buzzer sound: beeep beeep

void notifyEstopReleased(); //Buzzer sound: beep

//void notifyReboot(); //Buzzer sound: beeeeeeeeeep beeep beeep

void notifyOverCurrent(); //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep

void notifyUnderVoltage(); //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep

void notifyLowVoltage(); //Buzzer Sound: beeep beeep beeep
