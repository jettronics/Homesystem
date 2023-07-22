#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char byte;

// Number of statemachines
typedef enum
{
   smRemCtrl = 0,
   smMax
}
sm_ind_type;

typedef enum
{
   smStateInit = 0,
   smStateSend,
   smStatePause,
   smStatePowerOff,
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
  tmSend = 0,
  tmSound,
  tmOff,
  tmMax
}
tm_type;

typedef enum
{
   rcPower = 0,
   rcNext,
   rcVolup,
   rcVoldn
}
rc_type;


#endif
