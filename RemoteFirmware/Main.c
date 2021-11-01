/*
 * DetectorFirmware.c
 *
 * Created: 09.02.2019 10:48:31
 * Author : Jens
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Timer.h"
#include "Transmit.h"
#include "Io.h"
#include "Control.h"



static void avr_init(void);


int main(void)
{
	avr_init();

	Transmit_Init();
	Timer_Init();
	Control_Init();

	sei();

	while( 1 )
	{
		// Tasks here.
		Control_Process();
		Transmit_Process();
	}

	return(0);
}


static void avr_init(void)
{
	// Initialize device here.

	// Pull up resistor activation set PORTXn to 1 if configured as input

	//Dev   Dev   Dev   Dev   TP1   TP2   nc    nc
	//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
	// I     I     I     I     O     O     I     I
	// 0     0     0     0     1     1     0     0
	DDRA = 0x0C;
	// 0     0     0     0     0     0     1     1
	PORTA = 0x03;

	//Reset TP3   Pow   Snd   HF1   nc    nc    nc
	//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
	// I     O     O     O     O     I     I     I
	// 0     1     1     1     1     0     0     0
	DDRB = 0x78;
	// 0     0     1     0     0     1     1     1
	PORTB = 0x27;

	// As soon as possible activate power
	POWER_ON();

	// Enable and lock PLL
	PLLCSR |= _BV(PLLE);

	// Wait for PLL to lock (app. 100 ms)
	while( !(PLLCSR & _BV(PLOCK)) )
	{
		;
	}

	return;
}


