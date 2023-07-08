
#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"

#include "Receive.h"

// 960µs; Max: 960µ+120µs -> 1080µs
// 960µs; Min: 960µ+80µs  -> 1040µs
//#define TIME_SYNC_MAX    135 // digit = 1080µs / (8µs / digit) = 135
//#define TIME_SYNC_MIN    130 // digit = 1040µs / (8µs / digit) = 130
#define TIME_SYNC_MAX    68  // digit = 1080µs / (16µs / digit) = 68
#define TIME_SYNC_MIN    53 // digit = 840µs / (16µs / digit) = 53
// 480µs; H: Max: 480µ+120µs -> 600µs
// 480µs; H: Min: 480µ+80µs  -> 560µs
// 480µs; L: Max: 480µ-80µs  -> 400µs
// 480µs; L: Min: 480µ-120µs -> 360µs
//#define TIME_LOW_BIT_H_MAX    75 // digit = 600µs / (8µs / digit) = 75
//#define TIME_LOW_BIT_H_MIN    70 // digit = 560µs / (8µs / digit) = 70
//#define TIME_LOW_BIT_L_MAX    50 // digit = 400µs / (8µs / digit) = 50
//#define TIME_LOW_BIT_L_MIN    45 // digit = 360µs / (8µs / digit) = 45
#define TIME_LOW_BIT_H_MAX    38 // digit = 600µs / (16µs / digit) = 38
#define TIME_LOW_BIT_H_MIN    35 // digit = 560µs / (16µs / digit) = 35
#define TIME_LOW_BIT_L_MAX    25 // digit = 400µs / (16µs / digit) = 25
#define TIME_LOW_BIT_L_MIN    23 // digit = 360µs / (16µs / digit) = 23
// 240µs; H: Max: 240µ+120µs -> 360µs
// 240µs; H: Min: 240µ+80µs  -> 320µs
// 240µs; L: Max: 240µ-80µs  -> 160µs
// 240µs; L: Max: 240µ-120µs -> 120µs
//#define TIME_HIGH_BIT_H_MAX   45 // digit = 360µs / (8µs / digit) = 45
//#define TIME_HIGH_BIT_H_MIN   40 // digit = 320µs / (8µs / digit) = 40
//#define TIME_HIGH_BIT_L_MAX   20 // digit = 160µs / (8µs / digit) = 20
//#define TIME_HIGH_BIT_L_MIN   15 // digit = 120µs / (8µs / digit) = 15
#define TIME_HIGH_BIT_H_MAX   23 // digit = 360µs / (16µs / digit) = 23
#define TIME_HIGH_BIT_H_MIN   20 // digit = 320µs / (16µs / digit) = 20
#define TIME_HIGH_BIT_L_MAX   10 // digit = 160µs / (16µs / digit) = 10
#define TIME_HIGH_BIT_L_MIN    7 // digit = 120µs / (16µs / digit) = 7

//#define DEBOUNCE_THRESHOLD     3

void(*regFctClbk)(byte);
//static void DebounceData( byte byRec );
static byte SyndromDecoding( unsigned int uiRec );

static byte byTimestampOld = 0;
static byte byTimestampNew = 0;
//static byte byLevel = 0;
static byte byCaptureTime = 0;
static byte bySync = 0;
static unsigned int uiBitIndex = 0x0800;
static byte byHighBitRecognition = 0;
static unsigned int uiReceive = 0;
static unsigned int uiReceivedData = 0;
static byte byDataAvailable = 0;

const unsigned int uiCheckMatrix[] =
{
   0x04e0, /*0b 0100 1110 0000*/
   0x0690, /*0b 0110 1001 0000*/
   0x0348, /*0b 0011 0100 1000*/
   0x0d44, /*0b 1101 0100 0100*/
   0x0a42, /*0b 1010 0100 0010*/
   0x09c1  /*0b 1001 1100 0001*/
};


void Receive_Init( void )
{
   regFctClbk = 0;
   
   /* CK = 16MHz, divisor 256 -> t = 16 µs */
   TCCR0 |= _BV(CS02);

   /* CK = 16MHz, divisor 128 -> t = 8 µs */
   /* Start Timer Counter */
   //TCCR1B |= ( _BV(CS13) );
   
   /* Bandgap select, Interrupt enable, Toggle edge */
   ACSR |= ( _BV(ACBG) | _BV(ACIE) );
   
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
      static unsigned int uiRecVal = 0;
      static byte byRecOk = 0;
      static byte byRec = 0;

      TP2_ON();

      cli();
      byDataAvailable = 0;
      uiRecVal = uiReceivedData;
      sei();

      TP2_OFF();

      //DebounceData(byRecVal);
      byRecOk = SyndromDecoding(uiRecVal);

      if( byRecOk != 0 )
      {
         byRec = (uiRecVal >> 6) & 0x3F;
    	   if( regFctClbk != 0 )
	      {
		      regFctClbk( byRec );
	      }
      }

      TP2_OFF();

   }

   return;
}

static byte SyndromDecoding( unsigned int uiRec )
{
   static unsigned int uiTest = 0;
   static byte byCntBit = 0;
   static unsigned int uiBitCheck = 0;
   byte byRet = 1;

   for( byte byIndex1 = 0; byIndex1 < 6; byIndex1++ )
   {
      uiTest = uiRec & uiCheckMatrix[byIndex1];
	   byCntBit = 0;
	   uiBitCheck = 1;
	   for( byte byIndex2 = 0; byIndex2 < 12; byIndex2++ )
	   {
		   if( (uiTest & uiBitCheck) != 0 )
		   {
		      byCntBit++;
		   }
		   uiBitCheck <<= 1;
	   }
	   if( (byCntBit & 0x01) == 0x01 )
	   {
	      TP2_ON();
         byRet = 0;
	      break;
	   }
   }

   return byRet;
}

/*
static void DebounceData( byte byRec )
{
   static byte byDebounceVal = 0;
   static byte byDebounceCnt = 0;

   if( byDebounceVal == byRec )
   {
      byDebounceCnt++;
      if( byDebounceCnt >= DEBOUNCE_THRESHOLD )
      {
         byDebounceCnt = 0;
         if( regFctClbk != 0 )
         {
            regFctClbk( byRec );
         }
      }
   }
   else
   {
      byDebounceVal = byRec;
      byDebounceCnt = 0;
   }

}
*/

ISR( ANA_COMP_vect )
{
   /* interrupt happens when Analog comparator detect
      a pulse which toggels between voltage threshold on Analog Comparator input
   */
   //byTimestampNew = TCNT1;
   byTimestampNew = TCNT0;

   //if( ACSR & _BV(ACO) )
   //{
      /* Rising edge here -> Falling edge on Pin PA7 (AIN1) */
      //byLevel = 0;
   //}
   //else
   //{
      /* Falling edge here -> Rising edge on Pin PA7 (AIN1) */
      //byLevel = 1;
   //}
   
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
   /*if( (byLevel == 1) &&
       (byCaptureTime <= TIME_SYNC_MAX) &&
       (byCaptureTime > TIME_SYNC_MIN) )*/
   if( (byCaptureTime <= TIME_SYNC_MAX) &&
       (byCaptureTime > TIME_SYNC_MIN) )
   {
      /* valid Sync */
      TP1_ON();
      bySync = 1;
      uiBitIndex = 0x0800;
      byHighBitRecognition = 0;
      uiReceive = 0;
      TP1_OFF();
   }
   else
   /*if( ((byLevel == 1) &&
        (byCaptureTime <= TIME_LOW_BIT_H_MAX) &&
        (byCaptureTime > TIME_LOW_BIT_H_MIN))
       ||
       ((byLevel == 0) &&
        (byCaptureTime <= TIME_LOW_BIT_L_MAX) &&
        (byCaptureTime > TIME_LOW_BIT_L_MIN)) )*/
   if( (byCaptureTime <= TIME_LOW_BIT_H_MAX) &&
       (byCaptureTime > TIME_LOW_BIT_L_MIN) )
   {
      /* valid Low Bit */
      if( (bySync == 1) && (byHighBitRecognition == 0) )
      {
         TP1_ON();
         uiReceive &= ((~uiBitIndex) & 0x0FFF);
         uiBitIndex >>= 1;
         byHighBitRecognition = 0;
         TP1_OFF();
      }
      else
      {
         uiBitIndex = 0x0800;
         bySync = 0;
         uiReceive = 0;
         return;
      }

   }
   else
   /*if( ((byLevel == 1) &&
        (byCaptureTime <= TIME_HIGH_BIT_H_MAX) &&
        (byCaptureTime > TIME_HIGH_BIT_H_MIN))
       ||
       ((byLevel == 0) &&
        (byCaptureTime <= TIME_HIGH_BIT_L_MAX) &&
        (byCaptureTime > TIME_HIGH_BIT_L_MIN)) )*/
   if( (byCaptureTime <= TIME_HIGH_BIT_H_MAX) &&
       (byCaptureTime > TIME_HIGH_BIT_L_MIN) )
   {
      /* valid High Bit */
      if( bySync == 1 )
      {
         TP1_ON();
         byHighBitRecognition++;
         /* one 0 to 1 transition for High Bit is necessary */
         if( byHighBitRecognition >= 2 )
         {
            byHighBitRecognition = 0;
            uiReceive |= (uiBitIndex & 0x0FFF);
            uiBitIndex >>= 1;
         }
         TP1_OFF();
      }
      else
      {
         uiBitIndex = 0x0800;
         bySync = 0;
         uiReceive = 0;
         return;
      }
   }
   else
   {
      /* no valid sample */
      bySync = 0;
      uiBitIndex = 0x0800;
      byHighBitRecognition = 0;
      uiReceive = 0;
      return;
   }

   if( uiBitIndex == 0x0000 )
   {
      /* Transmission finished */
      bySync = 0;
      uiBitIndex = 0x0800;

      uiReceivedData = uiReceive & 0x0FFF;
      byDataAvailable = 1;
      uiReceive = 0;
   }
   
   return;
}





