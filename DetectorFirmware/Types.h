#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char byte;

// Number of statemachines
typedef enum
{
   smDetection = 0,
   smMax
}
sm_ind_type;

typedef enum
{
   smStateInit = 0,
   smStateSend,
   smStateSound,
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
  adDetector = 0,
  adBattery,
  adMax
}
ad_type;

typedef enum
{
  ctNormalBatt = 0,
  ctLowBatt,
  ctVeryLowBatt,
  ctCriticalBatt,
  ctMax
}
ct_batt_type;

#endif
