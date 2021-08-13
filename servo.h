void freeze(void);
#define	DEVICE_ADDR	0x40

#define ALLCALL		0x01
#define SUB1		0x01 << 3
#define SLEEP		0x01 << 4
#define AUTO_INCR	0x01 << 5
#define RESTART		0x01 << 7

//		Registers
#define MODE1		0
#define MODE2		1
#define	LED0		6		// 4 bytes for 12 bit data 
#define LED1        10      // 4 byte address from LED 0
#define PRESCALE	254		// Address NOT value

//50Hz =20mSec dived by 4096 = 4.8828125uSec/step
#define uSEC_PER_STEP	0.02 / 0.0000048828125
#define	MIN_VALUE	205/2	//0.001/000004.882812 or 4096 * 0.05 ( 1/20th of full range)
#define	MAX_VALUE	410*1.5	//0.002/000004.882812

#define ONE_MSEC  4096 * 0.03
#define TWO_MSEC  4096 * 0.13  - 1
#define MID_RANGE  (TWO_MSEC - ONE_MSEC) / 2 + ONE_MSEC