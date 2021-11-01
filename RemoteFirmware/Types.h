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

typedef enum
{
   SEND_IDLE = 0,
   SEND_START,
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
   SEND_PARITY_BIT_1,
   SEND_PARITY_BIT_0,
   SEND_STOP
}
transmit_type;


#endif
