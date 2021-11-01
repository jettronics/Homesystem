#ifndef _TIMER_H
#define _TIMER_H


void Timer_Init( void );

/* timer val in ms  */
void Timer_Start( tm_type bTimer, unsigned int uTimeMs );
byte Timer_Active( tm_type bTimer );

#endif
