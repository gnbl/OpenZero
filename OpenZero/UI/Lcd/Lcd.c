/// \file Lcd.c
/// 
/// High-level display functions
/// 



#ifndef F_CPU
	#define F_CPU		1000000UL
#endif

#define TICKERSPEED		250		// msec

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Lcd.h"
#include "ZeroLcd.h"
#include "../../Time/Rtc.h"

volatile uint16_t lcd_blinker = 0;


/// \brief .
/// 
/// 
void initLCD(void)
{
	/* Use 32 kHz crystal oscillator */
	/* 1/3 Bias and 1/4 duty, SEG0:SEG24 is used as port pins, COM0:COM3 as common pins */
	LCDCRB = (1<<LCDCS) | (1<<LCDMUX1) |(1<<LCDMUX0) | (1<<LCDPM2) | (1<<LCDPM1)|(1<<LCDPM0);
	/* Using 16 as prescaler selection and 7 as LCD Clock Divide */
	/* gives a frame rate of 49 Hz */
	LCDFRR = (1<<LCDCD2) | (1<<LCDCD1);
	/* Set segment drive time to 125 us and output voltage to 3.3 V*/
	LCDCCR = (1<<LCDDC1) | (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1);
	/* Enable LCD, default waveform and interrupt enabled */
	LCDCRA = (1<<LCDEN) | (1<<LCDIE);
}


/// \brief .
/// 
/// 
void LCD_tickertape( unsigned char *text, unsigned char len )
{
	for( int chars = 0; chars<=len-LCD_MAX_CHARS; chars++ )
	{
		for( int i = 0; i<LCD_MAX_CHARS; i++ )
		{
			Lcd_Map(i,*(text+i+chars));
		}
		
		_delay_ms( TICKERSPEED );
	}
}


/// \brief .
/// 
/// 
void LCD_tick( void )
{
	lcd_blinker++;
}


/// \brief .
/// 
/// 
void LCD_blinkYears( void )
{
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 0 );
	if( lcd_blinker % 2 )
	{
		Lcd_Map(0,' ');
		Lcd_Map(1,' ');
		Lcd_Map(2,' ');
		Lcd_Map(3,' ');
	}
	else
	{
		Lcd_Map(0,'0'+(rtc.year/1000)%10);
		Lcd_Map(1,'0'+(rtc.year/100)%10);
		Lcd_Map(2,'0'+(rtc.year/10)%10);
		Lcd_Map(3,'0'+rtc.year%10);
	}
}


/// \brief .
/// 
/// 
void LCD_blinkMonths( void )
{
	Lcd_Symbol( DOT, 1 );
	Lcd_Symbol( COLON, 0 );
	if( lcd_blinker % 2 )
	{
		Lcd_Map(0,' ');
		Lcd_Map(1,' ');
	}
	else
	{
		Lcd_Map(0,'0'+(rtc.month/10)%10);
		Lcd_Map(1,'0'+rtc.month%10);
	}
	Lcd_Map(2,'0'+(rtc.date/10)%10);
	Lcd_Map(3,'0'+rtc.date%10);
}


/// \brief .
/// 
/// 
void LCD_blinkDate( void )
{
	Lcd_Symbol( DOT, 1 );
	Lcd_Symbol( COLON, 0 );
	Lcd_Map(0,'0'+(rtc.month/10)%10);
	Lcd_Map(1,'0'+rtc.month%10);
	if( lcd_blinker % 2 )
	{
		Lcd_Map(2,' ');
		Lcd_Map(3,' ');
	}
	else
	{
		Lcd_Map(2,'0'+(rtc.date/10)%10);
		Lcd_Map(3,'0'+rtc.date%10);
	}
}


/// \brief .
/// 
/// 
void LCD_blinkHours( void )
{
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 1 );
	if( lcd_blinker % 2 )
	{
		Lcd_Map(0,' ');
		Lcd_Map(1,' ');
	}
	else
	{
		Lcd_Map(0,'0'+(rtc.hour/10)%10);
		Lcd_Map(1,'0'+rtc.hour%10);
	}
	Lcd_Map(2,'0'+(rtc.minute/10)%10);
	Lcd_Map(3,'0'+rtc.minute%10);
}


/// \brief .
/// 
/// 
void LCD_blinkMinutes( void )
{
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 1 );
	Lcd_Map(0,'0'+(rtc.hour/10)%10);
	Lcd_Map(1,'0'+rtc.hour%10);
	if( lcd_blinker % 2 )
	{
		Lcd_Map(2,' ');
		Lcd_Map(3,' ');
	}
	else
	{
		Lcd_Map(2,'0'+(rtc.minute/10)%10);
		Lcd_Map(3,'0'+rtc.minute%10);
	}
}


/// \brief .
/// 
/// 
void LCD_showTemp( uint8_t temp )
{
	Lcd_Symbol( DOT, 1 );
	Lcd_Symbol( COLON, 0 );
	
	// temperature is in 10 x degrees C
	Lcd_Symbol( DOT, 1 );

	if( temp>=100 )
		Lcd_Map(0,'0'+(temp/100)%10);
	else
		Lcd_Map(0,' ');
	if(temp>=10)
		Lcd_Map(1,'0'+(temp/10)%10);
	else
		Lcd_Map(1,' ');
	Lcd_Map(2,'0'+temp%10);
	Lcd_Map(3,'.');
}


/// \brief Display battery voltage.
/// 
/// 
void LCD_showVoltage( uint16_t voltage )
{
	LCD_writeNum(voltage);
	Lcd_Symbol( DOT, 1 );
	Lcd_Symbol( BAT, 1 );
}


/// \brief .
/// 
/// 
void LCD_showSecondsBar( void )
{
	Lcd_FillBar( 2 * rtc.second / 5 );
}


/// \brief .
/// 
/// 
void LCD_showDay( void )
{
	Lcd_Day( rtc.dow );
}


/// \brief .
/// 
/// 
void LCD_showTime( void )
{
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 1 );
	
	Lcd_Map(0,'0'+(rtc.hour/10)%10);
	Lcd_Map(1,'0'+rtc.hour%10);
	Lcd_Map(2,'0'+(rtc.minute/10)%10);
	Lcd_Map(3,'0'+rtc.minute%10);
}


/// \brief .
/// 
/// 
void LCD_writeText( unsigned char *text )
{
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 0 );
	for( int i = 0; i<LCD_MAX_CHARS; i++ )
	{
		Lcd_Map(i,*(text+i));
	}
}


/// \brief Display integer up to four digits, right aligned, leading spaces.
/// 
/// Supports negative numbers, but only shows last three digits because of the '-' sign.
void LCD_writeNum( int16_t num )
{
	// clear symbols
	Lcd_Symbol( DOT, 0 );
	Lcd_Symbol( COLON, 0 );
	
#if 0
	if( num>=1000 )
		Lcd_Map(0,'0'+(num/1000)%10);
	else
		Lcd_Map(0,' ');
	if(num>=100)
		Lcd_Map(1,'0'+(num/100)%10);
	else
		Lcd_Map(1,' ');
	if(num>=10)
		Lcd_Map(2,'0'+(num/10)%10);
	else
		Lcd_Map(2,' ');
	Lcd_Map(3,'0'+num%10);
#else
	// support negative numbers up to 3 digits
	// (TODO: this was added later and may need not be smaller than modifying the previous implementation)
	int8_t m = 0;
	if( num < 0)
	{
		num = -num;
		m = 1;
		Lcd_Map(0,'-');
	}
	
	// 4 digits, starting last
	for(int8_t i = (LCD_MAX_CHARS-1); i >= m; i--)
	{
		// write digit or blank
		if( num || i == 3)
		{
			Lcd_Map(i,'0' + num%10);
		}
		else
		{
			Lcd_Map(i,' ');
		}
		num /= 10;
	}
#endif
}


/// \brief Map <value> of <max> to MAXBARS segments of the progress bar.
/// 
void LCD_progressbar(uint16_t value, uint16_t max)
{
	// calculate segments
	uint32_t numbars = value;
	numbars *= MAXBARS;
	numbars /= max;
	
	// write segments
	for(uint8_t i = 0; i<MAXBARS; i++ )
	{
		if( i<numbars )
		{
			Lcd_Bar( i, 1 );
		}
		else
		{
			Lcd_Bar( i, 0 );
		}
	}
}
