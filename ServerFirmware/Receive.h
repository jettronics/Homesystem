#ifndef _RECEIVE_H
#define _RECEIVE_H

#include "Types.h"

void Receive_Init( void );

void Receive_Register( void(*fctClbk)(byte) );

void Receive_Process( void );

#endif
