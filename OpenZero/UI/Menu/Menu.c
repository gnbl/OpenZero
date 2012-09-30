/// \file Menu.c
/// 
/// User interaction via menu.


#include "Menu.h"
#include "../../Main.h"
#include "../../Time/Rtc.h"
#include "../../Temperature/Temp.h"
#include "../Controls/Controls.h"


/// \brief User interaction via menu.
/// 
/// 
void Menu(void)
{
	if( menuButtonPressed() )
	{
		switch( runstate )
		{
			case NORMAL_STATE :
				runstate = MENU_STATE;
				break;
				
			default :
				runstate = NORMAL_STATE;
				break;
		}
	} // end if( menuButtonPressed )
	
	if( timeButtonPressed() )
	{
		switch( runstate )
		{
			case NORMAL_STATE :
				runstate = TIMESET_STATE;
				timesetphase = TIMESET_START;
				
				_delay_ms( 500 );
				
				// show time with hours blinking
				timesetphase = TIMESET_YEAR;
				break;
				
			default :
				runstate = NORMAL_STATE;
				break;
		}
	} // end if( timeButtonPressed )
	
	if( okButtonPressed() )
	{
		switch( runstate )
		{
			case MENU_STATE :
				switch( mainmenu )
				{
					case TEMP :
						runstate = TEMPSET_STATE;
						break;
					
					case TIME :
						runstate = TIMESET_STATE;
						timesetphase = TIMESET_START;
						
						_delay_ms( 500 );
						
						// show time with hours blinking
						timesetphase = TIMESET_YEAR;
						break;
					
					default:
						break;
				}
				break;
			
			case TEMPSET_STATE :
				runstate = MENU_STATE;
				break;
			
			case TIMESET_STATE :
				switch( timesetphase )
				{
					case TIMESET_YEAR :
						timesetphase = TIMESET_MONTH;
						break;
					
					case TIMESET_MONTH :
						timesetphase = TIMESET_DATE;
						break;
					
					case TIMESET_DATE :
						timesetphase = TIMESET_HOURS;
						break;
					
					case TIMESET_HOURS :
						timesetphase = TIMESET_MINUTES;
						break;
					
					case TIMESET_MINUTES :
						timesetphase = TIMESET_YEAR;
						break;
					
					default :
						break;
				}
				break;
			
			default :
				break;
		}
	} // end if( okButtonPressed )
	
	ROTARYBUTTON rotaryButton = readRotaryButton();
	
	if( rotaryButton == ROTARY_UP )
	{
		switch( runstate )
		{
			case NORMAL_STATE :
			case MENU_STATE :
				mainmenu++;
				if( mainmenu == LAST_ITEM )
				{
					mainmenu = 0;
				}
			break;
			
			case TIMESET_STATE :
				increaseClock( timesetphase );
				break;
					
			case TEMPSET_STATE :
				if( targetTemp >= 500 )
				{
					targetTemp = 0;
				}
				else
				{
					targetTemp += 5;
				}
				break;
			
			default :
				break;
		}
	} // end if( BUTTON_UP_PRESSED )
	
	if( rotaryButton == ROTARY_DOWN )
	{
		switch( runstate )
		{
			case NORMAL_STATE :
			case MENU_STATE :
				if( mainmenu == 0 )
				{
					mainmenu = LAST_ITEM;
				}
				mainmenu--;
			break;
					
			case TIMESET_STATE :
				decreaseClock( timesetphase );
				break;
					
			case TEMPSET_STATE :
				if( targetTemp == 0 )
				{
					targetTemp = MAXTEMP;
				}
				else
				{
					targetTemp -= 5;
				}
				break;
			
			default :
				break;
		}
	} // end if( BUTTON_DOWN_PRESSED )
}
