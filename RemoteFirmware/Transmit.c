

#include <avr/interrupt.h>
#include <util/parity.h>

#include "Types.h"

#include "Io.h"
#include "Control.h"
#include "Transmit.h"


static void ToggleHFPin( void );
static void SetHFPin( void );
static void ResetHFPin( void );
static void StartTransmit( void );
static void StopTransmit( void );


transmit_type bSendState;
byte bHFPin;
byte bBitCnt;
byte bParity;
byte bBitSample;
byte bTransmitData;
byte bSendData;
byte bNewRequest;




void Transmit_Init(void)
{
  /* TCCR1A and TCCR1B are initially zero */
  ResetHFPin();
  bSendState = SEND_IDLE;
  bBitCnt = 0;
  bParity = 0;
  bBitSample = 0;
  bTransmitData = 0;
  bSendData = 0;
  bNewRequest = 0;

  /* after every match reset timer counter to 0, */
  TCCR1B |= _BV(CTC1);

  /* enable TOIE interrupt */
  TIMSK |= _BV(TOIE1);

}

void Transmit_Send( byte bSend )
{
   bSendData = bSend;
   bNewRequest = 1;
}

void Transmit_Process( void )
{
   if( (bSendState == SEND_IDLE) && (bNewRequest == 1) )
   {
      bNewRequest = 0;
      bTransmitData = bSendData;
      bSendState = SEND_START;
      bBitCnt = 5;
      bBitSample = 0x20;
      bParity = parity_even_bit( bTransmitData );
      StartTransmit();
   }
}


static void StartTransmit( void )
{
  /* Timer Counter to 0 */
  TCNT1 = 0;
  /* Compare value -> toggle with 240µs */
  OCR1C = 240;
  /* Start Timer Counter */
  TCCR1B |= ( _BV(CS12) | _BV(CS10));

}


static void StopTransmit( void )
{
  /* Stop Timer Counter */
  TCCR1B &= (~(_BV(CS12) | _BV(CS10)));
}


static void ToggleHFPin( void )
{
   if( bHFPin == 1 )
   {
      bHFPin = 0;
      HF_OFF();
   }
   else
   {
      bHFPin = 1;
      HF_ON();
   }
}

static void SetHFPin( void )
{
   bHFPin = 1;
   HF_ON();
}

static void ResetHFPin( void )
{
   bHFPin = 0;
   HF_OFF();
}


ISR( TIMER1_OVF1_vect )
{
  switch( bSendState )
  {
     case SEND_START:
        SetHFPin();
        bBitCnt--;
        if( bBitCnt == 0 )
        {
           ResetHFPin();
           bSendState = SEND_BIT_5_1;
        }
     break;

     case SEND_BIT_5_1:
     case SEND_BIT_4_1:
     case SEND_BIT_3_1:
     case SEND_BIT_2_1:
     case SEND_BIT_1_1:
     case SEND_BIT_0_1:
        if( bTransmitData & bBitSample )
        {
           ToggleHFPin();
        }
        bBitSample >>= 1;
        bSendState++;
     break;

     case SEND_BIT_5_0:
     case SEND_BIT_4_0:
     case SEND_BIT_3_0:
     case SEND_BIT_2_0:
     case SEND_BIT_1_0:
     case SEND_BIT_0_0:
        ToggleHFPin();
        bSendState++;
     break;

     case SEND_PARITY_BIT_1:
        if( bParity )
        {
           ToggleHFPin();
        }
        bSendState = SEND_PARITY_BIT_0;
     break;
     case SEND_PARITY_BIT_0:
        ToggleHFPin();
        bSendState = SEND_STOP;
        bBitCnt = 6;
     break;

     case SEND_STOP:
        ResetHFPin();
        bBitCnt--;
        if( bBitCnt == 0 )
        {
           StopTransmit();
           bSendState = SEND_IDLE;
        }
     break;

     default:
     break;
  }

}
