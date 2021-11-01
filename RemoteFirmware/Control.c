
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"
#include "Timer.h"
#include "Transmit.h"

#include "Control.h"


static void EventSm( byte bVal, sm_ind_type smInd );

static void HandleRemCtrl( void );

static void RemCtrlSend( void );

static sm_type stateMachines[smMax];
static byte bTransmitCounter = 0;
static byte bWaitDetector = 0;
static byte bKeyPressed = 0;



void Control_Init(void)
{
   memset( &stateMachines, 0, sizeof(stateMachines) );
   
   stateMachines[smRemCtrl].fStateHndl = &HandleRemCtrl;
   stateMachines[smRemCtrl].fEventFct = &EventSm;
   stateMachines[smRemCtrl].bState = smStateInit;
   stateMachines[smRemCtrl].bEvent = 0;

   return;
}

void Control_Event( byte bVal )
{
   byte bIndex;
   for( bIndex = 0; bIndex < smMax; bIndex++ )
   {
      (*stateMachines[bIndex].fEventFct)(bVal, bIndex);
   }
}

void Control_Process( void )
{
   byte bIndex;
   
   for( bIndex = 0; bIndex < smMax; bIndex++ )
   {
      if( stateMachines[bIndex].fStateHndl != 0 )
      {
         (*stateMachines[bIndex].fStateHndl)();
      }
   }
   
   return;
}


static void EventSm( byte bVal, sm_ind_type smInd )
{
   stateMachines[smInd].bEvent = bVal;
}

static void HandleRemCtrl( void )
{
   switch( stateMachines[smRemCtrl].bState )
   {
	  case smStateInit:
      {
		 bKeyPressed = REMCTRL_KEY_PRESSED();
		 Timer_Start(tmSound, 1000);
         Timer_Start(tmSend, 200);
         stateMachines[smRemCtrl].bState = smStateSend;
      }
      break;

      case smStateSend:
      {
         SOUND_ON();
         RemCtrlSend();
         if( Timer_Active(tmSound) == 0 )
         {
            Timer_Start(tmSound, 500);
            stateMachines[smRemCtrl].bState = smStatePause;
         }
      }
      break;
      
      case smStatePause:
      {
         SOUND_OFF();
		 bKeyPressed = REMCTRL_KEY_PRESSED();
         if( bKeyPressed == (byte)0 )
         {
            if( Timer_Active(tmOff) == 0 )
            {
               if( bWaitDetector == 0 )
               {
                  Timer_Start(tmOff, 100);
                  bWaitDetector = 1;
               }
               else
               {
                  stateMachines[smRemCtrl].bState = smStatePowerOff;
               }
            }
         }
         else
         if( Timer_Active(tmSound) == 0 )
         {
            Timer_Start(tmSound, 1000);
            bTransmitCounter = 0;
            stateMachines[smRemCtrl].bState = smStateSend;
         }
      }
      break;
      
      case smStatePowerOff:
      {
         cli();
         SOUND_OFF();
         POWER_OFF();
		 // Sleep enable
		 MCUCR |= 0x20;
		 // Power down
		 MCUCR |= 0x10;
		 while(1) 
		 {
			SOUND_OFF();
			POWER_OFF();
		 };
      }
      break;

      default:
      break;
   }
}


static void RemCtrlSend( void )
{
   byte bTransmit;

   if( Timer_Active(tmSend) == 0 )
   {
      if( bKeyPressed != (byte)0 )
      {
         bWaitDetector = 0;

         if( bTransmitCounter < 20 )
         {
            bTransmitCounter++;

            Timer_Start(tmSend, 18);

            bTransmit = 0;
            bTransmit |= ((DEVICE() << 2) & 0x3C);

            if( (REMCTRL_POWER() & bKeyPressed) != 0 )
            {
               bTransmit |= (((byte)rcPower) & 0x03);
            }
            else
            if( (REMCTRL_NEXT() & bKeyPressed) != 0 )
            {
               bTransmit |= (((byte)rcNext) & 0x03);
            }
            else
            if( (REMCTRL_VOLUP() & bKeyPressed) != 0 )
            {
               bTransmit |= (((byte)rcVolup) & 0x03);
            }
            else
            {
               bTransmit |= (((byte)rcVoldn) & 0x03);
            }

            Transmit_Send(bTransmit);
         }

      }
   }
   return;
}


