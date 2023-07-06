

#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"
#include "Control.h"
#include "Transmit.h"

typedef enum
{
   SEND_IDLE = 0,
   SEND_START,
   SEND_BIT_11_1,
   SEND_BIT_11_0,
   SEND_BIT_10_1,
   SEND_BIT_10_0,
   SEND_BIT_9_1,
   SEND_BIT_9_0,
   SEND_BIT_8_1,
   SEND_BIT_8_0,
   SEND_BIT_7_1,
   SEND_BIT_7_0,
   SEND_BIT_6_1,
   SEND_BIT_6_0,
   SEND_BIT_5_1,
   SEND_BIT_5_0,
   SEND_BIT_4_1,
   SEND_BIT_4_0,
   SEND_BIT_3_1,
   SEND_BIT_3_0,
   SEND_BIT_2_1,
   SEND_BIT_2_0,
   SEND_BIT_1_1,
   SEND_BIT_1_0,
   SEND_BIT_0_1,
   SEND_BIT_0_0,
   SEND_STOP
}
transmit_type;

static void ToggleHFPin( void );
static void SetHFPin( void );
static void ResetHFPin( void );
static void StartTransmit( void );
static void StopTransmit( void );


transmit_type bSendState;
byte bHFPin;
byte bBitCnt;
unsigned int uiBitSample;
unsigned int uiTransmitData;
unsigned int uiSendData;
byte bNewRequest;


const unsigned int uiCrcCode[][] =
{
   {0, 0, 0, 0},
   {269, 354, 444, 467},
   {538, 629, 683, 708},
   {791, 888, 934, 969},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {0, 0, 0, 0},
   {2832, 2943, 2977, 3022},
   {3123, 3164, 3202, 3309}
};



void Transmit_Init(void)
{
  /* TCCR1A and TCCR1B are initially zero */
  ResetHFPin();
  bSendState = SEND_IDLE;
  bBitCnt = 0;
  uiBitSample = 0;
  uiTransmitData = 0;
  uiSendData = 0;
  bNewRequest = 0;

  /* after every match reset timer counter to 0, */
  TCCR1B |= _BV(CTC1);

  /* enable TOIE interrupt */
  TIMSK |= _BV(TOIE1);

}

void Transmit_Send( byte bSend )
{
   uiSendData = uiCrcCode[DEVICE][(bSend & 0x03)];
   if( uiSendData != 0 )
   {
      bNewRequest = 1;
   }
}

void Transmit_Process( void )
{
   if( (bSendState == SEND_IDLE) && (bNewRequest == 1) )
   {
      bNewRequest = 0;
      uiTransmitData = uiSendData;
      bSendState = SEND_START;
      bBitCnt = 5;
      uiBitSample = 0x800;
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
           bSendState = SEND_BIT_11_1;
           bBitCnt = 6;
        }
     break;

     case SEND_BIT_11_1:
     case SEND_BIT_10_1:
     case SEND_BIT_9_1:
     case SEND_BIT_8_1:
     case SEND_BIT_7_1:
     case SEND_BIT_6_1:
     case SEND_BIT_5_1:
     case SEND_BIT_4_1:
     case SEND_BIT_3_1:
     case SEND_BIT_2_1:
     case SEND_BIT_1_1:
     case SEND_BIT_0_1:
        if( bTransmitData & uiBitSample )
        {
           ToggleHFPin();
        }
        uiBitSample >>= 1;
        bSendState++;
     break;

     case SEND_BIT_11_0:
     case SEND_BIT_10_0:
     case SEND_BIT_9_0:
     case SEND_BIT_8_0:
     case SEND_BIT_7_0:
     case SEND_BIT_6_0:
     case SEND_BIT_5_0:
     case SEND_BIT_4_0:
     case SEND_BIT_3_0:
     case SEND_BIT_2_0:
     case SEND_BIT_1_0:
     case SEND_BIT_0_0:
        ToggleHFPin();
        bSendState++;
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
