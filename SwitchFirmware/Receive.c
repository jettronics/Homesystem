
#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"

#include "Receive.h"

// 960µs; Max: 960µ+120µs -> 1080µs
// 960µs; Min: 960µ+80µs  -> 1040µs
#define TIME_SYNC_MAX    135 // digit = 1080µs / (8µs / digit) = 135
#define TIME_SYNC_MIN    130 // digit = 1040µs / (8µs / digit) = 130
// 480µs; H: Max: 480µ+120µs -> 600µs
// 480µs; H: Min: 480µ+80µs  -> 560µs
// 480µs; L: Max: 480µ-80µs  -> 400µs
// 480µs; L: Min: 480µ-120µs -> 360µs
#define TIME_LOW_BIT_H_MAX    75 // digit = 600µs / (8µs / digit) = 75
#define TIME_LOW_BIT_H_MIN    70 // digit = 560µs / (8µs / digit) = 70
#define TIME_LOW_BIT_L_MAX    50 // digit = 400µs / (8µs / digit) = 50
#define TIME_LOW_BIT_L_MIN    45 // digit = 360µs / (8µs / digit) = 45
// 240µs; H: Max: 240µ+120µs -> 360µs
// 240µs; H: Min: 240µ+80µs  -> 320µs
// 240µs; L: Max: 240µ-80µs  -> 160µs
// 240µs; L: Max: 240µ-120µs -> 120µs
#define TIME_HIGH_BIT_H_MAX   45 // digit = 360µs / (8µs / digit) = 45
#define TIME_HIGH_BIT_H_MIN   40 // digit = 320µs / (8µs / digit) = 40
#define TIME_HIGH_BIT_L_MAX   20 // digit = 160µs / (8µs / digit) = 20
#define TIME_HIGH_BIT_L_MIN   15 // digit = 120µs / (8µs / digit) = 15

void(*regFctClbk)(byte);

static byte byTimestampOld = 0;
static byte byTimestampNew = 0;
static byte byLevel = 0;
static byte byCaptureTime = 0;
static byte bySync = 0;
static byte byBitIndex = 0x40;
static byte byHighBitRecognition = 0;
static byte byReceive = 0;
static byte byParity = 0;
static byte byReceivedData = 0;
static byte byDataAvailable = 0;


void Receive_Init( void )
{
   regFctClbk = 0;
   
   /* CK = 16MHz, divisor 128 -> t = 8 µs */
   /* Start Timer Counter */
   TCCR1B |= ( _BV(CS13) );
   
   /* Bandgap select, Interrupt enable, Rising edge */
   ACSR |= ( _BV(ACBG) | _BV(ACIE) | _BV(ACIS1) | _BV(ACIS0) );
   
   return;
}

void Receive_Register( void(*fctClbk)(byte) )
{
	regFctClbk = fctClbk;
}

void Receive_Process( void )
{
   if( byDataAvailable )
   {
      byte byRecVal;

      cli();
      byDataAvailable = 0;
      byRecVal = byReceivedData;
      sei();

      if( regFctClbk != 0 )
	   {
         if( ((byRecVal >> 2) & 0x0F) == DEVICE() )
         {
            regFctClbk( (byRecVal & 0x01) );
         }
	   }		
   }

   return;
}




ISR( ANA_COMP_vect )
{
   /* interrupt happens when Analog comparator detect
      a pulse which toggels between voltage threshold on Analog Comparator input
   */
   byTimestampNew = TCNT1;

   if( ACSR & _BV(ACIS0) )
   {
      /* Rising edge */
      byLevel = 1;
      /* Change to falling edge */
      ACSR &= (~_BV(ACIS0));
   }
   else
   {
      /* Falling edge */
      byLevel = 0;
      /* Change to rising edge */
      ACSR |= _BV(ACIS0);
   }
   
   if( byTimestampNew < byTimestampOld )
   {
      byCaptureTime = (((byte)0xFF - byTimestampOld) + byTimestampNew);
   }
   else
   {
      byCaptureTime = byTimestampNew - byTimestampOld;
   }
   byTimestampOld = byTimestampNew;

   /* check for valid SampleCnt */
   if( (byLevel == 0) &&
       (byCaptureTime <= TIME_SYNC_MAX) &&
       (byCaptureTime > TIME_SYNC_MIN) )
   {
      /* valid Sync */
      bySync = 1;
      byBitIndex = 0x40;
      byHighBitRecognition = 0;
      byParity = 0;
      byReceive = 0;
   }
   else
   if( ((byLevel == 0) &&
        (byCaptureTime <= TIME_LOW_BIT_H_MAX) &&
        (byCaptureTime > TIME_LOW_BIT_H_MIN))
       ||
       ((byLevel == 1) &&
        (byCaptureTime <= TIME_LOW_BIT_L_MAX) &&
        (byCaptureTime > TIME_LOW_BIT_L_MIN)) )
   {
      /* valid Low Bit */
      if( (bySync == 1) && (byHighBitRecognition == 0) )
      {
         byReceive &= ((~byBitIndex) & 0x7F);
         byBitIndex >>= 1;
         byHighBitRecognition = 0;
      }
      else
      {
         byBitIndex = 0x40;
         bySync = 0;
         byParity = 0;
         byReceive = 0;
         return;
      }

   }
   else
   if( ((byLevel == 0) &&
        (byCaptureTime <= TIME_HIGH_BIT_H_MAX) &&
        (byCaptureTime > TIME_HIGH_BIT_H_MIN))
       ||
       ((byLevel == 1) &&
        (byCaptureTime <= TIME_HIGH_BIT_L_MAX) &&
        (byCaptureTime > TIME_HIGH_BIT_L_MIN)) )
   {
      /* valid High Bit */
      if( bySync == 1 )
      {
         byHighBitRecognition++;
         /* one 0 to 1 transition for High Bit is necessary */
         if( byHighBitRecognition >= 2 )
         {
            byHighBitRecognition = 0;
            byReceive |= (byBitIndex & 0x7F);
            if( byBitIndex != 0x01 )
            {
                byParity ^= 1;
            }
            byBitIndex >>= 1;
         }
      }
      else
      {
         byBitIndex = 0x40;
         bySync = 0;
         byParity = 0;
         byReceive = 0;
         return;
      }
   }
   else
   {
      /* no valid sample */
      bySync = 0;
      byBitIndex = 0x40;
      byHighBitRecognition = 0;
      byParity = 0;
      byReceive = 0;
      return;
   }

   /* 0  0000  00  0
          dev|batt|P */
   if( byBitIndex == 0x00 )
   {
      /* Transmission finished */
      bySync = 0;
      byBitIndex = 0x40;

      if( byParity == (byReceive & 0x01) )
      {
         if( byReceive > 0 )
         {
            byReceivedData = (byReceive >> 1) & 0x3F;
            byDataAvailable = 1;
         }
      }
      byParity = 0;
      byReceive = 0;
   }

   return;
}





