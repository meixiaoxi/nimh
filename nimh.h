#ifndef __NIMH_H__
#define __NIMH_H__


#define u8	unsigned char
#define u16	unsigned int
#define u32	unsigned long

#define	LED1	PA0
#define	LED2	PA1
#define	LED3	PC1
#define	LED4	PC0


#define	BAT_MAX_LABEL 		4

#define	BAT_ADD			0
#define	BAT_REMOVE		1


#define HAS_BATTERY		0x01   

#define CHARGE_STATE_ALL		0x3C

#define BAT_TYPE_ERROR						(1<<7)
#define CHARGE_STATE_ERROR				(1<<6)
#define CHARGE_STATE_PRE					(1<<5)
#define CHARGE_STATE_FAST					(1<<4)
#define CHARGE_STATE_SUP					(1<<3)
#define CHARGE_STATE_TRICK					(1<<2)
#define BAT_DETECT_BIT						(1<<1)   //电池测试
#define BAT_CHECK_BIT						(1<<0)   //有无电池

#define BAT_MIN_VOLT_OPEN	496 			//(0.4/3.3)*4096
#define BAT_MAX_VOLT_OPEN	2110		//(1.7/3.3)*4096

#define BAT_MAX_VOLT_CLOSE 2234		//(1.8/3.3)*4096	


#define BAT_CHARGING_PULSE_TIME	6   //100ms  100/16.384
#define BAT_CHARGING_DETECT_TIME	0

#define FAST_SKIP_COUNT	0
#define SUP_SKIP_COUNT		1
#define PRE_SKIP_COUNT		1
#define TRI_SKIP_COUNT		1
#define DUMMY_SKIP_COPUNT	0xFF

#define DisWatchdog()	RCEN=0
#define EnWatchdog()		RCEN=1

#define t8n_start()	T8NEN=1
#define t8n_stop()	T8NEN=0

#define t8p1_start()	T8P1E=1
#define t8p1_stop()	T8P1E=0

#define CHANNEL_VBAT_1	3
#define CHANNEL_VBAT_2 4
#define CHANNEL_VBAT_3	12
#define CHANNEL_VABT_4	2

#define CHANNEL_20_RES	13

#define BAT_VALID_VALUE	787

#define SHOW_CHARGING_TICK	0x20


u32 getSysTick();
u32 getDiffTickFromNow(u32 ttick);
void ledHandler(void);
void delay_us(u16);
u32 getBatTick();
	
u16 getVbatAdc(u8 channel);
u16 getAverage(u8 channel);

void send(u16 sData);
void sendStr(char str[]);

#endif
