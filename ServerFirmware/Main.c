/*
 * Created: 09.02.2019 10:48:31
 * Author : Jens
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Spi.h"
#include "Receive.h"
#include "Io.h"
#include "Control.h"



static void avr_init(void);


int main(void)
{
	avr_init();

   Spi_SlaveInit(0);
	Receive_Init();
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

	//Rec   nc    nc    nc    TP1   TP2   nc    nc
	//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
	// I     I     I     I     O     O     I     I
	// 0     0     0     0     1     1     0     0
	DDRA = 0x0C;
	// 0     1     1     1     0     0     1     1
	PORTA = 0x73;

	//Reset TP3   nc    nc    CS0   SCK   MISO  MOSI
	//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
	// I     O     I     I     I     I     O     I
	// 0     1     0     0     0     0     1     0
	DDRB = 0x42;
	// 0     0     1     1     1     1     0     1
	PORTB = 0x3D;

	// Enable and lock PLL
	PLLCSR |= _BV(PLLE);

	// Wait for PLL to lock (app. 100 ms)
	while( !(PLLCSR & _BV(PLOCK)) )
	{
		;
	}

	return;
}


