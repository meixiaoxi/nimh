#include "nimh.h"
#include "hic.h"

u8 tempStr1[] = "temp1:";
u8 tempStr2[] = "temp2:";

extern u16 gChargeCurrent;

extern void LED_ON(u8 led);
extern void LED_OFF(u8 led);
extern void delay_ms(u16);
u16 getAdcValue(u8 channel)
{
	
	u16 temp;
	u8 tempH,tempL;


	ADCCL = 5| channel;


	ADTRG=1;
				LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
	while(ADTRG) {;}
			LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
	tempL = ADCRL;	
	tempH = ADCRH;
	

	
	temp = ((u16)tempH)<<4;
	temp += (tempL>>4);

	ADEN=0;
	
	return temp;
}

u16 getAverage(u8 channel)
{
	u8 i;
	u16 temp,max,min,ret;


	temp = getAdcValue(channel<<4);
	ret= temp;
	max =temp;
	min = temp;
	for(i=0;i<9;i++)
	{
		delay_us(200);
		 temp = getAdcValue(channel<<4);
	 	if(temp > max)
	 	{
			max = temp;
	 	}

		 if(temp < min)
		{
			min = temp;
	 	}
	 	ret += temp;
	}	

	return (ret - max - min)>>3;
}
u16 getVbatAdc(u8 channel)
{
	u16 temp1,temp2;
					LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);

	switch(channel)
	{
		case 1:
			channel = CHANNEL_VBAT_1;break;
		case 2:
			channel = CHANNEL_VBAT_2;break;
		case 3:
			channel = CHANNEL_VBAT_3;break;
		case 4:
			channel = CHANNEL_VABT_4;break;
		default:
			break;
	}

	
					LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
	gChargeCurrent= getAverage(CHANNEL_20_RES);
	temp2 = getAverage(channel);

//	sendStr(tempStr1);
//	send(gChargeCurrent);
//	sendStr(tempStr2);
//	send(temp2);
	
	return (temp2-gChargeCurrent);
}
