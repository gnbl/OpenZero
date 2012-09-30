#ifndef _BATTERY_H
#define _BATTERY_H


#define POWERLOSS_DDR			DDRE
#define POWERLOSS_PIN			PINE
#define POWERLOSS_PORT		PORTE
#define POWERLOSS					0

#define BATTERY_THRESHOLD	30

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

void			initBattery(void);
uint16_t	batteryVoltage(void);

#endif
