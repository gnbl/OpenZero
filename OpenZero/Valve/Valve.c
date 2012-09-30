/// \file Valve.c
/// 
/// Valve control.
/// 
/// original header: Valve.c / Created: 3-3-2012 14:05:45 / Author: Willem


#include "Valve.h"
#include "../Main.h"
#include "../UI/Lcd/Lcd.h"

extern volatile int16_t revCounter;
uint16_t valvePosMax = 0;


volatile VALVESTATE valvestate = VALVE_UNKNOWN;



/// \brief Configure hardware for motor control (output, sensor).
/// 
void initValve( void )
{
	// motor output
	MOTOR_ENABLE;	// Pin E6 / Pin E7 provide power to the motor and are both outputs
	
	// optical sensor
	// supply
	OPTO_ENABLE;	// Pin E2 provides power to the opto-sensor LED and is output
	
	// input
	DDRE &= ~(1<<DDE1);						// PINE1 = PCINT1 attached to opto-sensor collector is input
#ifdef THERMY
	// THERMy (REV.0.0 REG) has external pull-up resistor
	PORTE |= (1<<PE1);						// PINE1 = PCINT1 attached to opto-sensor collector, enable internal pull-up
#endif

	// another input with ADC, not used
	DDRF  &= ~(1<<DDF0);					// attached to opto-sensor collector
	PORTF &= ~(1<<PF0);						// attached to opto-sensor collector
	
	// pin-change interrupt for opto-sensor
	EIFR |= (1<<PCIF0); // | (1<<PCIF0) | (1<<INTF1); // clear interrupt flags
	PCMSK0 |= (1<<PCINT1);				// But only mask PCINT1
	EIMSK |= (1<<PCIE0);					// Enable interrupt-on-change interrupts for PCINT0-PCINT7
	// the ISR is in Main.c
	
	
	// current sense
	// see doProbe() below
	
}


/// \brief Find opened/closed position of valve.
/// 
void doProbe( void )
{
	runstate = PROBING_STATE;
	
	// setup ADC for motor current measurement
	ADMUX = (1<<REFS0)|(1<<MUX1);			// ref. voltage = AVcc, channel = ADC2
	ADCSRA = (1<<ADPS2)|(1<<ADIE);		// 1:16 prescaler
	_delay_ms( 300 );									// settle ADC mux
	
	// take some time to take a measurement without the motor running, go to sleep and wake up if the measurements are done
	probingphase = PROBING_START;
	ADCSRA |= (1<<ADEN);							// enable ADC
	ADCSRA |= (1<<ADSC);							// start first conversion
	set_sleep_mode( SLEEP_MODE_ADC );
	sleep_mode();
	
	// enable valve position sensing
	OPTO_ENABLE;
	OPTO_SENSOR_ON;
	
	// open valve fully
	openValve();
	revCounter = 0;
	// while( !okButtonPressed() ) ; // wait for button (TESTING)
	_delay_ms( 300 );	// wait for motor current to settle
	
	// close valve fully
	closeValve();
	valvePosMax = revCounter;
	// while( !okButtonPressed() ) ; // wait for button (TESTING)
	_delay_ms( 300 );	// wait for motor current to settle
	
	// open valve fully again
	openValve();
	
	// disable ADC
	probingphase = PROBING_END;
	ADCSRA &= ~((1<<ADEN)|(1<<ADIE));
	
	// disable valve position sensing
	OPTO_SENSOR_OFF;
	OPTO_DISABLE;
	
	_delay_ms( 300 );	// ??
	
	runstate = NORMAL_STATE;
}


/// \brief Open the valve fully with current limit.
/// 
void openValve( void )
{
	valvestate = VALVE_OPENING;
	
	probingphase = PROBING_RUNNING_CW;
	powerMotor(MOTOR_CW);
	while( probingphase == PROBING_RUNNING_CW );
	
	valvestate = VALVE_OPEN;
	
	powerMotor(MOTOR_STOP);
	
	// TESTING
	// LCD_writeText( (unsigned char *)"REVS");
	// _delay_ms( 1000 );
	// LCD_writeNum( revCounter );
	// revCounter = 0;
	// _delay_ms( 3000 );
}


/// \brief Close the valve fully with current limit.
/// 
void closeValve( void )
{
	valvestate = VALVE_CLOSING;
	
	probingphase = PROBING_RUNNING_CCW;
	powerMotor(MOTOR_CCW);
	while( probingphase == PROBING_RUNNING_CCW ) ;
	
	valvestate = VALVE_CLOSED;
	
	powerMotor(MOTOR_STOP);
	
	// TESTING
	// LCD_writeText( (unsigned char *)"REVS");
	// _delay_ms( 1000 );
	// LCD_writeNum( revCounter );
	// revCounter = 0;
	// _delay_ms( 3000 );
}


/// \brief Motor control with H-bridge (two outputs).
/// 
/// Power-loss detection requires the motor outputs to be High-Z (tristate, inputs)
/// The parameter might be useful for PWM-control in the future.
void powerMotor(int8_t motor_power)
{
	switch (motor_power)
	{
		case MOTOR_CW:	// open
			MOTOR_ENABLE;
			RUN_MOTOR_CW;
			break;
		
		case MOTOR_CCW:	// close
			MOTOR_ENABLE;
			RUN_MOTOR_CCW;
			break;
		
		case MOTOR_STOP:
		default:
			STOP_MOTOR;
			MOTOR_DISABLE;
			break;
	}
}


/// \brief Move valve to specified position.
/// 
/// TODO:
///   - function is blocking
///   - motor current not checked
int8_t valvePosition(int16_t target)
{
#warning valvePosition() does not check the motor current!

	uint8_t error = 0;
	// validate parameter
	if (target > valvePosMax)
	{
		return 1;
	}
	if (target == revCounter)
	{
		return 0;
	}
	
	OPTO_ENABLE;
	OPTO_SENSOR_ON;
	
	// TODO: just to display revCounter
	runstate = PROBING_STATE;
	probingphase = PROBING_RUNNING_CCW;
	
	// direction
	if (target > revCounter)
	{
		powerMotor(MOTOR_CCW);	// close
		while( revCounter < target);	// BLOCKING!
		powerMotor(MOTOR_STOP);
	}
	else if (target < revCounter)
	{
		powerMotor(MOTOR_CW);	// open
		while( revCounter > target);	// BLOCKING!
		powerMotor(MOTOR_STOP);
	}
	
	OPTO_SENSOR_OFF;
	OPTO_DISABLE;
	
	// TODO: just to display revCounter
	probingphase = PROBING_END;
	runstate = NORMAL_STATE;
	
	return error;
}
