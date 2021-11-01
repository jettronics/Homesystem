#ifndef _IO_H
#define _IO_H

// Output
#define TP1_ON()     (PORTA |= 0x08)
#define TP1_OFF()    (PORTA &= (~0x08))
#define TP2_ON()     (PORTA |= 0x04)
#define TP2_OFF()    (PORTA &= (~0x04))
#define TP3_ON()     (PORTB |= 0x40)
#define TP3_OFF()    (PORTB &= (~0x40))
#define SWITCH_ON()  (PORTB |= 0x20)
#define SWITCH_OFF() (PORTB &= (~0x20))

// Input

// Config
#define DEVICE()    (10)  // Detector: 1...9; Switch: 10...15
#endif
