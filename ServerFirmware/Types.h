#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char byte;

// Number of statemachines
typedef enum
{
   smRemote = 0,
   smMax
}
sm_ind_type;

typedef enum
{
   smStateInit = 0,
   smStateWaitChipSelect,
   smStateChipSelected,
   smStateMax
}
sm_state_type;

typedef struct
{
   byte bEvent;
   sm_state_type bState;
   void (*fStateHndl)(void);
   void (*fEventFct)(byte, sm_ind_type);
}
sm_type;


#endif
