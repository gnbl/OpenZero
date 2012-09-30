#ifndef _VALVE_H
#define _VALVE_H

#ifndef F_CPU
	#define F_CPU		1000000UL
#endif

#define MOTOR_ENABLE			DDRE  |=  ((1<<DDE6)|(1<<DDE7))
#define RUN_MOTOR_CW			PORTE |=  (1<<PE7)&~(1<<PE6)
#define RUN_MOTOR_CCW			PORTE |=  (1<<PE6)&~(1<<PE7)
#define STOP_MOTOR				PORTE &= ~((1<<PE6)|(1<<PE7))
#define MOTOR_DISABLE			DDRE  &= ~((1<<DDE6)|(1<<DDE7))

#define MOTOR_CW		 100
#define MOTOR_CCW		-100
#define MOTOR_STOP	   0

#if TYPE == REG
#define MAXREVPULSES	500
#else
#error Unknown hardware constant for the number of actuator impulses.
#endif


#define OPTO_ENABLE				DDRE  |=  (1<<DDE2)
#define OPTO_SENSOR_ON		PORTE |=  (1<<PE2);
#define OPTO_SENSOR_OFF		PORTE &= ~(1<<PE2);
#define OPTO_DISABLE			DDRE  &= ~(1<<DDE2);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "../UI/Controls/Controls.h"
#include "../Main.h"

typedef enum {
	VALVE_OPENING,
	VALVE_OPEN,
	VALVE_CLOSING,
	VALVE_CLOSED,
	VALVE_UNKNOWN
} VALVESTATE;

extern volatile VALVESTATE valvestate;

void		initValve(			void );
void		doProbe(				void );
void		openValve(			void );
void		closeValve(			void );
void		powerMotor( 		int8_t );
int8_t	valvePosition(	int16_t );

#endif