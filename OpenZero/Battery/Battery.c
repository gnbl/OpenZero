/// \file Battery.c
/// 
/// Measure battery voltage, detect power-loss.


#include "Battery.h"


/// \brief Setup battery voltage measurement and power-loss interrupt.
/// 
/// Requires minimal load on battery in order to work, so the H-bridge and optical sensor need to be disabled.
void initBattery(void)
{
	// enable power-loss interrupt
	DDRE &= ~(PE0);
	PORTE &= ~(PE0);
	PCMSK0 |= (1 << PCINT0);	// mask input
	EIMSK |= (1 << PCIE0);	// enable interrupt
	// ISR currently in Main.c
}


/// \brief Measure battery voltage.
/// 
/// Measures internal band-gap voltage reference against the supply voltage.
uint16_t batteryVoltage(void)
{
	uint8_t i;
	uint8_t samples = 4;
	uint32_t result;
	
	// measure internal bandgap with AVCC (AREF connected)
	ADMUX = (1<<REFS0) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1); 
	
	// dummy readout because the mux needs to settle
	ADCSRA = (1<<ADPS2) | (1<<ADEN) | (1<<ADSC);
	while ( ADCSRA & (1<<ADSC) );
	ADC;
	
	// actual measurment
	result = 0; 
	for( i=0; i<samples; i++ )
	{
		ADCSRA |= 1<<ADSC;
		while (ADCSRA & (1<<ADSC));
		result += ADC;
	}
	result /= samples-1;
	
	// calculate actual voltage
	// TODO
	// Two problems:
	//  - could not manage to get compiler to calculate 
	//  - calculation must be wrong, because:
	//     - bandgap voltage correction is needed (0.69)
	//     - same problem on ATtiny26
	// 
	// batt = 1024.0*1.18/0.69*10.0+0.5 / batt;	// Vcc in 1/100 V (bandgap seems way off)
	
	// avoid division by zero
	if(result)
	{
		result = 175119UL / result;	// Vcc in 1/100 V (bandgap seems way off)
	}
	
	if( result < BATTERY_THRESHOLD )
	{
		// ALARM
	}
	
	return (uint16_t)result;
}
