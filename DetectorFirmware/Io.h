#ifndef _IO_H
#define _IO_H

// Output
#define TP1_ON()    (PORTA |= 0x08)
#define TP1_OFF()   (PORTA &= (~0x08))
#define TP2_ON()    (PORTA |= 0x04)
#define TP2_OFF()   (PORTA &= (~0x04))
#define TP3_ON()    (PORTB |= 0x40)
#define TP3_OFF()   (PORTB &= (~0x40))
#define HF_ON()     (PORTB |= 0x08)
#define HF_OFF()    (PORTB &= (~0x08))
#define SOUND_ON()  (PORTB |= 0x10)
#define SOUND_OFF() (PORTB &= (~0x10))
#define POWER_ON()  (PORTB |= 0x20)
#define POWER_OFF() (PORTB &= (~0x20))

#define POWER_ENABLED() (PINB & 0x20)

// Input

// Config
// Bathroom = 1
// Kitchen = 2
// Toilet = 3
#define DEVICE()    (1)  // 1...15
#endif
