#ifndef _CONTROL_H
#define _CONTROL_H

//#define DETECTOR_OFF_THRESHOLD  26 //1V
#define DETECTOR_OFF_THRESHOLD  8  //0.31V
#define DETECTOR_ON_THRESHOLD  90  //3.5V

void Control_Init( void );

void Control_Event( byte bVal );

void Control_Process( void );

#endif
