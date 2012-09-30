/// \file Controls.c
/// \brief User inputs (buttons, wheel).
/// 
/// 
/// Created: 3-3-2012 18:11:20
/// Author: Willem

#include "Controls.h"

unsigned char previousRotaryState = 0;


/// \brief .
/// 
/// 
void initControls( void )
{
	// all pins are input by default, so there's no need to set the direction
	
	// enable internal pull-up resistors
	PORTB |= (1<<PB0);	//  "+"   button on wheel
	PORTB |= (1<<PB4);	// "MENU" button
	PORTB |= (1<<PB5);	// "TIME" button
	PORTB |= (1<<PB6);	//  "OK"  button
	PORTB |= (1<<PB7);	//  "-"   button on wheel
	
// Why can I not get PCINT1_vector to work???
//	EIMSK |= (1<<PCIE1);				// Enable interrupt-on-change interrupts for PCINT8-PCINT15 which includes the push buttons
//	PCMSK1 |= (1<<PCINT8)|(1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15);
}


/// \brief Quadrature decoder, debouncing with delays.
/// 
/// 
ROTARYBUTTON readRotaryButton( void )
{
	// read inputs
	unsigned char rotaryState = (BUTTON_UP_PRESSED<<1) | BUTTON_DOWN_PRESSED;
	
	// has changed
	if( rotaryState != previousRotaryState )
	{
		// wait for contacts to settle (debounce)
		_delay_ms( DEBOUNCE_TIME );
		
		// read again and check
		if( rotaryState == ((BUTTON_UP_PRESSED<<1) | BUTTON_DOWN_PRESSED) )
		{
			#if 0
			// show on LCD
			LCD_writeNum( (previousRotaryState<<2) | rotaryState );
			_delay_ms(321);
			#endif
			
			// decode
			switch( (previousRotaryState<<2) | rotaryState )
			{
				// turning CW (+)
				// Thermy notches every other, but reads: 8,1,7,14
				case  1:	// 0001
				case  7:	// 0111
				case  8:	// 1000
				// case 12:	// 1100
				case 14:	// 1110
					previousRotaryState = rotaryState;
					return ROTARY_UP;
					break;
				
				// turning CCW (-)
				// Thermy notches every other, but reads: 2,11,13,4,
				case  2:	// 0010
				case  4:	// 0101
				case 11:	// 1011
				case 13:	// 1101
					previousRotaryState = rotaryState;
					return ROTARY_DOWN;
					break;
			}
		}
	}
	
	return ROTARY_UNKNOWN;
}


/// \brief .
/// 
/// 
unsigned char menuButtonPressed( void )
{
	if( BUTTON_MENU_PRESSED )
	{
		_delay_ms( DEBOUNCE_TIME );
		
		// poll again after a debounce period
		if( BUTTON_MENU_PRESSED )
		{
			// wait until button is released
			while( BUTTON_MENU_PRESSED ) ;
			// continue here if button is still depressed
			
			return 1;
		}
	} // end if( BUTTON_MENU_PRESSED )
	
	return 0;
}


/// \brief .
/// 
/// 
unsigned char timeButtonPressed( void )
{
	if( BUTTON_TIME_PRESSED )
	{
		_delay_ms( DEBOUNCE_TIME );
		
		// poll again after a debounce period
		if( BUTTON_TIME_PRESSED )
		{
			// wait until button is released
			while( BUTTON_TIME_PRESSED );
			// continue here if button is still depressed
			
			return 1;
		}
	} // end if( BUTTON_TIME_PRESSED )
	
	return 0;
}


/// \brief .
/// 
/// 
unsigned char okButtonPressed( void )
{
	if( BUTTON_OK_PRESSED )
	{
		_delay_ms( DEBOUNCE_TIME );
		
		// poll again after a debounce period
		if( BUTTON_OK_PRESSED )
		{
			// wait until button is released
			while( BUTTON_OK_PRESSED );
			// continue here if button is still depressed
			
			return 1;
		}
	} // end if( BUTTON_OK_PRESSED )
	
	return 0;
}
