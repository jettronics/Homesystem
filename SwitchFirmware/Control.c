
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"
#include "Timer.h"

#include "Control.h"


static void EventSm( byte bVal, sm_ind_type smInd );

static void HandleSwitch( void );

static sm_type stateMachines[smMax];


void Control_Init(void)
{
   memset( &stateMachines, 0, sizeof(stateMachines) );
   
   stateMachines[smSwitch].fStateHndl = &HandleSwitch;
   stateMachines[smSwitch].fEventFct = &EventSm;
   stateMachines[smSwitch].bState = smStateSwitchInit;
   stateMachines[smSwitch].bEvent = 0;

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


static void HandleSwitch( void )
{
   switch( stateMachines[smSwitch].bState )
   {
      case smStateSwitchInit:
      {
         stateMachines[smSwitch].bState = smStateSwitchOff;
      }
      break;

      case smStateSwitchOn:
      {
         if( (stateMachines[smSwitch].bEvent == 0) ||
             (Timer_Active(tmOff) == 0) )
         {
            stateMachines[smSwitch].bState = smStateSwitchOff;
            SWITCH_OFF();
         }
      }
      break;

      case smStateSwitchOff:
      {
         if( stateMachines[smSwitch].bEvent == 1 )
         {
            stateMachines[smSwitch].bState = smStateSwitchOn;
            SWITCH_ON();
            // 10min -> 600s -> 16,384ms per digit -> 36621
            Timer_Start(tmOff, 36621);
         }
      }
      break;

      default:
      break;
   }
}




