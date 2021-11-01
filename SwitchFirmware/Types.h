#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char byte;

// Number of statemachines
typedef enum
{
   smSwitch = 0,
   smMax
}
sm_ind_type;

typedef enum
{
   smStateSwitchInit = 0,
   smStateSwitchOn,
   smStateSwitchOff,
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

typedef enum
{
  tmOff = 0,
  tmMax
}
tm_type;


#endif
