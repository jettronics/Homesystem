
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "Types.h"

#include "Adc.h"
#include "Io.h"
#include "Timer.h"
#include "Transmit.h"

#include "Control.h"


static void EventSm( byte bVal, sm_ind_type smInd );

static void HandleDetection( void );

static sm_type stateMachines[smMax];
static byte bTransmitCounter = 0;

// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));


// Function Implementation
void wdt_init(void)
{
   MCUSR = 0;
   wdt_disable();

   return;
}

/*
set_sleep_mode(SLEEP_MODE_PWR_DOWN);
cli();
sleep_enable();
//sleep_bod_disable();
//sei();
sleep_cpu();
*/

void Control_Init(void)
{
   memset( &stateMachines, 0, sizeof(stateMachines) );
   
   stateMachines[smDetection].fStateHndl = &HandleDetection;
   stateMachines[smDetection].fEventFct = &EventSm;
   stateMachines[smDetection].bState = smStateInit;
   stateMachines[smDetection].bEvent = 0;

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

static void HandleDetection( void )
{
   byte bTransmit;
   byte bAdcBatt;

   switch( stateMachines[smDetection].bState )
   {
      case smStateInit:
      {
         Timer_Start(tmOff, 6000);
         Timer_Start(tmSound, 1000);
         Timer_Start(tmSend, 100);

         stateMachines[smDetection].bState = smStateSend;
      }
      break;

      case smStateSend:
      {
         if( Timer_Active(tmSend) == 0 )
         {
            if( Timer_Active(tmOff) == 0 )
            {
               if( Adc_Get(adDetector) < DETECTOR_OFF_THRESHOLD )
               {
                  stateMachines[smDetection].bState = smStatePowerOff;
               }
               else
               {
                  Timer_Start(tmOff, 6000);
               }
            }
            else
            if( Timer_Active(tmSound) == 0 )
            {
               Timer_Start(tmSound, 1000);
               stateMachines[smDetection].bState = smStateSound;
               SOUND_ON();
            }
            else
            if( bTransmitCounter < 30 )
            {
               bTransmitCounter++;

               Timer_Start(tmSend, 18);

               bTransmit = 0;
               bTransmit |= ((DEVICE() << 2) & 0x3C);

               bAdcBatt = Adc_Get(adBattery);
               if( bAdcBatt >= 196 ) //7,7V
               {
                  bTransmit |= (((byte)ctNormalBatt) & 0x03);
               }
               else
               if( (bAdcBatt >= 168) && (bAdcBatt < 196) ) //6,6V...7,7V
               {
                  bTransmit |= (((byte)ctLowBatt) & 0x03);
               }
               else
               if( (bAdcBatt >= 140) && (bAdcBatt < 168) ) //5,5V...6,6V
               {
                  bTransmit |= (((byte)ctVeryLowBatt) & 0x03);
               }
               else
               {
                  bTransmit |= (((byte)ctCriticalBatt) & 0x03);
               }

               Transmit_Send(bTransmit);
            }
         }
      }
      break;
      
      case smStateSound:
      {
         if( Timer_Active(tmSound) == 0 )
         {
            Timer_Start(tmSound, 1000);
            bTransmitCounter = 0;
            stateMachines[smDetection].bState = smStateSend;
            SOUND_OFF();
            TP2_OFF();
         }
      }
      break;
      
      case smStatePowerOff:
      {
         if( Adc_Get(adDetector) < DETECTOR_OFF_THRESHOLD )
         {
            TP1_ON();
            wdt_enable(WDTO_2S);
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            cli();
            sleep_enable();
            //sleep_bod_disable();
            sei();
            POWER_OFF();
            sleep_cpu();
            
            // wake up after sleep by Watchdog only
            POWER_ON();
            sleep_disable();
            wdt_init();
/*
            while( 1 )
            {
               TP1_ON();
               POWER_OFF();
               TP2_OFF();
               TP1_OFF();
            }
*/
         }

         TP2_ON();
         TP1_OFF();
         POWER_ON();
         Timer_Start(tmOff, 6000);
         Timer_Start(tmSound, 1000);
         bTransmitCounter = 0;
         stateMachines[smDetection].bState = smStateSend;

      }
      break;

      default:
      break;
   }
}

