/// \file Main.c
/// 
/// See README.txt
/// 
/// Original header: Created: 15-2-2012 21:16:33 / Author: Willem


#ifndef F_CPU
	#define F_CPU	1e6UL
#endif

// AVR-GCC
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

// Modules
#include "Main.h"
#include "Config.h"
#include "Time/Rtc.h"
#include "UI/Lcd/Lcd.h"
#include "UI/Lcd/ZeroLcd.h"
#include "UI/Controls/Controls.h"
#include "UI/Menu/Menu.h"
#include "Valve/Valve.h"
#include "Temperature/Temp.h"
#include "Battery/Battery.h"
#include "Control/Control.h"

volatile RUNSTATE runstate = NORMAL_STATE;
volatile TIMESETPHASE timesetphase = TIMESET_UNKNOWN;
volatile PROBINGPHASE probingphase = PROBING_UNKNOWN;
volatile MAINMENU mainmenu = TEMP;

volatile uint16_t adcValue = 0;						// in V x 1000
volatile uint16_t adcVref = 3000;					// in V x 1000
volatile uint16_t adcTemp = 0;

volatile int16_t revCounter = 0;




/// \brief Initialise the microcontroller and hardware.
/// 
void init(void)
{
	
	// Analog Comparator Disable
	ACSR = (1<<ACD);
	
	// watchdog disable
	wdt_disable();
	
	initRTC();
	initControls();
	initLCD();
	initValve();
	initBattery();
	
	// timer0 is being used as a global 'heartbeat'
	// i.e. for blinking in the LCD
	// and for running a temperature check at regular intervals
	TCCR0A = (1<<CS02) | (1<<CS00);	// timer clock = system clock / 1024 
	TIFR0 = (1<<TOV0);							// clear pending interrupts
	TIMSK0 = (1<<TOIE0);						// enable timer0 overflow Interrupt: timer clock / 256 = 3.8 Hz
}


/// \brief The main program function with endless loop.
/// 
int main(void)
{
	// Initialize
	init();
	
	// Enable Global Interrupts
	sei();
	
	#if 0
	// Test voltage measuring
	mainmenu = LAST_ITEM;
	while(1)
	{
		uint16_t v = batteryVoltage();
		LCD_showVoltage(v);
	}
	#endif
	
	
	// start a probe run to find the "fully open" and "fully closed" positions
	doProbe();
	
	// move valve (TODO: for testing)
	valvePosition( 100 );
	
	// initialize the NTC sensor and start the 1st measurement
	// consequent measurements will be done every tick
	initTemp();
	
	runstate = NORMAL_STATE;
	
	while (1)
	{
		Control();
		Menu();
		
		// go to sleep but wake up if any button is pressed
		set_sleep_mode( SLEEP_MODE_ADC );
		sleep_mode();
	}
}



/// \brief LCD Start of Frame.
/// 
/// Asynchronously updates according to system states.
/// TODO: commented a few lines to enable debug output
ISR(LCD_vect) 
{ 

#if 0
	// using LCD for debug output
	return;
#endif

	// clear LCD
	Lcd_SymbolsOff();
	Lcd_DaysOff();
	Lcd_FillBar( 0 );
	
	
	if( ADC_CONVERSION_BUSY )
	{
		Lcd_Symbol( LOCK, 1 );
	}
	else
	{
		Lcd_Symbol( LOCK, 0 );
	}
	
	
	switch( runstate )
	{
		case TEMPSET_STATE :
			Lcd_Symbol( ICE, 1 );
			LCD_showTemp( targetTemp );
			break;
		
		case MENU_STATE :
			switch( mainmenu )
			{
				case TEMP :
				LCD_writeText((unsigned char *)"TEMP");
				break;
				
				case TIME :
				LCD_writeText((unsigned char *)"TIME");
				break;
				
				default :
				LCD_writeText((unsigned char *)"MENU");
				break;
			}
			break;
		
		case TIMESET_STATE :
			LCD_showDay();
			switch( timesetphase )
			{
				case TIMESET_YEAR :
					LCD_blinkYears();
					break;
				
				case TIMESET_MONTH :
					LCD_blinkMonths();
					break;
				
				case TIMESET_DATE :
					LCD_blinkDate();
					break;
				
				case TIMESET_HOURS :
					LCD_blinkHours();
					break;
				
				case TIMESET_MINUTES :
					LCD_blinkMinutes();
					break;
				
				case TIMESET_START :
				default:
					LCD_writeText( (unsigned char *)"TIME" );
					break;
			}
			break;
		
		case PROBING_STATE :
			switch( probingphase )
			{
				case PROBING_UNKNOWN :
					LCD_writeText( (unsigned char *)"INIT" );
					break;
				
				case PROBING_START :
					LCD_writeNum( adcVref );
					break;
				
				case PROBING_RUNNING_CCW :
					Lcd_Symbol( INSIDE, 1);
					// LCD_writeNum( adcValue );	// current
					LCD_writeNum( revCounter );	// position
					break;
				
				case PROBING_END_CCW :
					LCD_writeText( (unsigned char *)"CLSD" );
					break;
				
				case PROBING_RUNNING_CW :
					Lcd_Symbol( OUTSIDE, 1);
					// LCD_writeNum( adcValue );	// current
					LCD_writeNum( revCounter );	// position
					break;
				
				case PROBING_END_CW :
					LCD_writeText( (unsigned char *)"OPEN" );
					break;
				
				case PROBING_END :
					LCD_writeText( (unsigned char *)"DONE" );
					break;
				
				default :
					LCD_writeNum( revCounter );
					break;
			}
			break;
		
		default:
			switch( mainmenu )
			{
				case TEMP :
					LCD_showTemp( adcTemp );
					break;
				
				case TIME :
					LCD_showDay();
					LCD_showTime();
					LCD_showSecondsBar();
					break;
				
				default:
					Lcd_Symbol(BAG, 1 );
					LCD_writeNum( adcValue );
					break;
			}
			break;
	}
}


/// \brief ADC Conversion Complete.
/// 
/// Reads temperature and motor current.
ISR( ADC_vect )
{
	unsigned long temp;	// not a temperature, just temporary
	
	// read ADC result register(s)
	adcValue = ADC;
	
	switch( runstate )
	{
		case NORMAL_STATE :
			// read temperature
			// Rt = 120k * adc / (1024 - adc )
			temp = 120 * adcValue;
			temp /= (1024 - adcValue );			// in k Ohms, this is the calculated R of the NTC based on Vcc
			adcTemp = findNTCTemp( (unsigned int)temp );
			
			// disable sensor and ADC
			ADCSRA &= ~(1<<ADEN)|(1<<ADIE);
			NTC_SENSOR_OFF;
			
			break;
		
		case PROBING_STATE :
			// read motor current, handle probing
			
			// ??
			if( probingphase == PROBING_START )
			{
				temp = adcValue;
				temp *= 3000;
				temp /= 1024;
				adcVref = temp;		// in V x 1000
			}
			
			// check limits
			if( probingphase == PROBING_RUNNING_CW )
			{
				if( adcValue < ADC_THRESHOLD_OPEN )
				{
					probingphase = PROBING_END_CW;
				}
			}
			else if( probingphase == PROBING_RUNNING_CCW )
			{
				if( adcValue < ADC_THRESHOLD_CLOSE || revCounter >= MAXREVPULSES )
				{
					probingphase = PROBING_END_CCW;
				}
			}
			
			// restart conversion
			ADCSRA |= (1<<ADSC);
			break;
		
		default:
			break;
	}
}


/// \brief System tick.
/// 
/// 3.8 Hz, see init()
ISR(TIMER0_OVF_vect) 
{ 
	// LCD blinking
	LCD_tick();
	
	// start temperature measurement
	// see ADC ISR
	NTC_SENSOR_ON;
	ADCSRA |= (1<<ADSC);
}


/// \brief The pin change interrupt PCI0 will trigger if any enabled PCINT7..0 pin toggles.
/// 
/// Applies to 
///   - optical sensor
///   - power-loss detection
ISR( PCINT0_vect )
{
	// power-loss
	if(POWERLOSS_PIN & (1 << POWERLOSS))
	{
		// disable everything to save power
		// TODO: put this in a function
		
		Lcd_Symbol(BAT, 1 );	// TESTING (barely visible)
		
		// ADC
		ADCSRA = 0;
		
		// A, C, D, G: LCD
		 DDRB = 0;
		PORTB = 0;
		
		 DDRE = 0;
		PORTE = 0;
		
		 DDRF = 0;
		PORTF = 0;
		
		// Disable LCD
		// Wait until a new frame is started.
		while ( !(LCDCRA & (1<<LCDIF)) );
		// Set LCD Blanking and clear interrupt flag
		// by writing a logical one to the flag.
		LCDCRA = (1<<LCDEN)|(1<<LCDIF)|(1<<LCDBL);
		// Wait until LCD Blanking is effective.
		while ( !(LCDCRA & (1<<LCDIF)) );
		// Disable LCD
		LCDCRA = (0<<LCDEN);
		
		// shut down everything else
		PRR = (1<<PRLCD) | (1<<PRTIM1) | (1<<PRSPI) | (1<<PRUSART0) | (1<<PRADC);
		
		 DDRA = 0;
		PORTA = 0;
		
		 DDRC = 0;
		PORTC = 0;
		
		 DDRD = 0;
		PORTD = 0;
		
		 DDRG = 0;
		PORTG = 0;
		
		
		// write data to EEPROM
		
	}
	else
	{
		// TODO: use different state
		// opto-sensor pulse
		if( probingphase == PROBING_RUNNING_CW )
		{
			// opening valve
			revCounter--;
		}
		else if( probingphase == PROBING_RUNNING_CCW )
		{
			// closing valve
			revCounter++;
		}
	}
}


/// \brief The pin change interrupt PCI1 will trigger if any enabled PCINT15..8 pin toggles.
/// 
/// Applies to 
///   - MENU/OK/time keys
///   - +/- keys or rotary encoder
ISR( PCINT1_vect )
{
	// see Controls.c
}

