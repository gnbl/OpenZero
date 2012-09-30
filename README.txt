Firmware for mechatronic radiator thermostat based on Atmel AVR microcontroller, written in C.

WORK IN PROGRESS, please contribute.

Made in Germany by Eurotronic Technology GmbH, several product models (Sparmatic Comet / Zero / Basic, Aldi Thermy) are based on similar hardware.

The aim is to create firmware supporting all hardware features, allowing further development of control algorithms and additional wireless communications, improving and surpassing the original features.

Documentation in German http://www.mikrocontroller.net/articles/Sparmatic_Heizungsthermostate
Discussion in German http://www.mikrocontroller.net/topic/237375

Developed with AVR/Atmel Studio (includes AVR-GCC)

USE AT YOUR OWN RISK.

Microcontroller: ATmega169P(V), 8 MHz / 8 internal RC oscillator, 32 Khz crystal
Supply:          2 AA alkaline batteries
Valve actuator:  brushed DC motor controlled by discrete H-bridge
Display:         Segment LCD (4 chars/digits and symbols) driven by ATmega169 internal LCD controller
Input:           5 keys or 3 keys and "scroll" wheel
Temp.-sens.:     NTC internal