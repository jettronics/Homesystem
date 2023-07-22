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

// Input
#define REMCTRL_POWER() (0x80)
#define REMCTRL_NEXT()  (0x40)
#define REMCTRL_VOLUP() (0x20)
#define REMCTRL_VOLDN() (0x10)
#define REMCTRL_KEY_PRESSED() (PINA & 0xF0)


// Config
#define DEVICE()    (11)  // 11...15
#define DEVOFFSET() (10)
#endif
