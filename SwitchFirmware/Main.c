/*
 * Created: 09.02.2019 10:48:31
 * Author : Jens
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Timer.h"
#include "Receive.h"
#include "Io.h"
#include "Control.h"



static void avr_init(void);


int main(void)
{
	avr_init();

	Receive_Init();
	Timer_Init();
	Control_Init();

	Receive_Register(Control_Event);

	sei();

	while( 1 )
	{
		// Tasks here.
	   Receive_Process();
		Control_Process();
	}

	return(0);
}


static void avr_init(void)
{
	// Initialize device here.

	// Pull up resistor activation set PORTXn to 1 if configured as input

	//Dev   Dev   Dev   Dev   TP1   TP2    Batt  Detect
	//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
	// I     I     I     I     O     O     I     I
	// 0     0     0     0     1     1     0     0
	DDRA = 0x0C;
	// 1     1     1     1     0     0     0     0
	PORTA = 0xF0;

	//Reset TP3   Pow   Snd   HF1   nc    nc    nc
	//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
	// I     O     O     O     O     I     I     I
	// 0     1     1     1     1     0     0     0
	DDRB = 0x78;
	// 0     0     1     0     0     1     1     1
	PORTB = 0x27;

	// Enable and lock PLL
	PLLCSR |= _BV(PLLE);

	// Wait for PLL to lock (app. 100 ms)
	while( !(PLLCSR & _BV(PLOCK)) )
	{
		;
	}

	return;
}


