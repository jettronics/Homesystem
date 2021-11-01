
#include <string.h>
#include <avr/interrupt.h>


#include "Types.h"
#include "Io.h"

#include "Timer.h"

unsigned int uaTimerVal[tmMax];
byte baActive[tmMax];
volatile unsigned int uCounter = 0;
byte bTimerIndex = 0;



void Timer_Init(void)
{
   memset( uaTimerVal, 0, tmMax );
   memset( baActive, 0, tmMax );

   uCounter = 0;
   bTimerIndex = 0;

   TCNT0 = 0;

   /* reset prescaler */
   TCCR0 |= _BV(PSR0);
   /* Start Timer Counter CK = 64MHz/4 = 16MHz -> 16MHz/64 = 250kHz*/
   TCCR0 |= (_BV(CS01) | _BV(CS00));

   /* enable TOIE interrupt */
   TIMSK |= _BV(TOIE0);

   return;
}



void Timer_Start( tm_type bTimer, unsigned int uTimeMs )
{
   static unsigned long int ulTotalTime = 0;
   unsigned int uExpireTime;

   ulTotalTime = ((unsigned long int)uCounter) + ((unsigned long int)uTimeMs);

   if(ulTotalTime >= ((unsigned long int)65535))
   {
      uExpireTime = (unsigned int)(ulTotalTime - ((unsigned long int)65535));
   }
   else
   {
      uExpireTime = (unsigned int)ulTotalTime;
   }
   
   cli();
   uaTimerVal[bTimer] = uExpireTime;
   baActive[bTimer] = 1;
   sei();
}


byte Timer_Active( tm_type bTimer )
{
   return baActive[bTimer];
}


ISR( TIMER0_OVF0_vect )
{
   /* every 1,024ms called */
   uCounter++;
   for(bTimerIndex = 0; bTimerIndex < tmMax; bTimerIndex++ )
   {
      if( baActive[bTimerIndex] == 1 )
      {
         if( uCounter == uaTimerVal[bTimerIndex] )
         {
            baActive[bTimerIndex] = 0;
         }
      }
   }
}

