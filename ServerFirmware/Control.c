
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "Types.h"

#include "Io.h"
#include "Spi.h"

#include "Control.h"


static void EventSm( byte bVal, sm_ind_type smInd );

static void HandleRemote( void );

static sm_type stateMachines[smMax];


void Control_Init(void)
{
   memset( &stateMachines, 0, sizeof(stateMachines) );
   
   stateMachines[smRemote].fStateHndl = &HandleRemote;
   stateMachines[smRemote].fEventFct = &EventSm;
   stateMachines[smRemote].bState = smStateInit;
   stateMachines[smRemote].bEvent = 0;

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


static void HandleRemote( void )
{
   static byte bSend = 0;

   switch( stateMachines[smRemote].bState )
   {
      case smStateInit:
      {
         if( CS0() != 0 )
         {
            bSend = 0;
            Spi_Put(bSend);
            stateMachines[smRemote].bState = smStateWaitChipSelect;
         }
      }
      break;

      case smStateWaitChipSelect:
      {
         if( CS0() == 0 )
         {
            stateMachines[smRemote].bState = smStateChipSelected;
         }
         else
         {
            if( bSend != stateMachines[smRemote].bEvent )
            {
               bSend = stateMachines[smRemote].bEvent;
               Spi_Put(bSend);
            }
         }
      }
      break;

      case smStateChipSelected:
      {
         if( CS0() != 0 )
         {
            bSend = 0;
            Spi_Put(bSend);
            stateMachines[smRemote].bEvent = 0;
            stateMachines[smRemote].bState = smStateWaitChipSelect;
         }
      }
      break;

      default:
      break;
   }
}




