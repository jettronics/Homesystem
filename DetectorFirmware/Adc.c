

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "Types.h"
#include "Io.h"

#include "Adc.h"




byte bADChannel;
byte bSample;
byte baFiltCnt[adMax];
unsigned int uaRaw[adMax];
byte baFilt[adMax];


void Adc_Init(void)
{
   /* converted data left adjusted */
   bADChannel = 0;
   bSample = 0;
   memset( baFiltCnt, 0, adMax );
   memset( baFilt, 0, adMax );

   ADMUX |= _BV(ADLAR);
   ADCSR |= ( _BV(ADEN) | _BV(ADIE) | _BV(ADSC) | _BV(ADPS2)| _BV(ADPS1)| _BV(ADPS0) );
   return;
}


byte Adc_Get( byte bChannel )
{
   return baFilt[bChannel];
}


ISR( ADC_vect  )
{
   /* called every 105µs*/
   bSample++;
   if( bSample >= 5 )
   {
      /* about every 525µs */
      bSample = 0;
      uaRaw[bADChannel] += (((unsigned int)ADCH) & ((unsigned int)0x00FF));
      baFiltCnt[bADChannel]++;
      if( baFiltCnt[bADChannel] >= 0x08)
      {
         // Every 8ms
         baFilt[bADChannel] = (byte)(uaRaw[bADChannel] >> 3);
         baFiltCnt[bADChannel] = 0;
	      uaRaw[bADChannel] = 0;
      }
	   bADChannel++;
      if( bADChannel >= (byte)adMax )
      {
         bADChannel = 0;
      }
      ADMUX = (bADChannel & 0x01) | _BV(ADLAR);
   }
   ADCSR |= (_BV(ADSC));
}
