#ifndef _CONFIG_H
#define _CONFIG_H

#ifndef F_CPU
	#define F_CPU					1e6UL
#endif


// select the hardware type below
// 

// BASIC (Sparmatic)
#if 0
	#define TYPE HKT
	#define REV 04
#endif

// ZERO (Sparmatic)
#if 0
	#define TYPE REGZ
	#define REV 00
#endif

// ZERO2 (Sparmatic)
#if 0
	#define TYPE REGZ
	#define REV 00
#endif

// COMET (Sparmatic)
#if 0
	#define TYPE COMET
	#define REV 01
#endif

// THERMy (Aldi)
#if 1
	#define TYPE REG
	#define REV 00
#endif




#if TYPE == REG
	#if REV == 00
		#define WHEEL_PULSES 2
		#define VALVE_PULSES 4
	#endif
#endif

#endif