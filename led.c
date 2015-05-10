#include "nimh.h"
#include <hic.h>

extern u8 gBatStateBuf[];

void LED_ON(u8 led)
{
	switch(led)
	{
		case 1:
			PA0=0;break;
		case 2:
			PA1=0;break;
		case 3:
			PC1=0;break;
		case 4:
			PC0=0;break;
	}
}

void LED_OFF(u8 led)
{
	switch(led)
	{
		case 1:
			PA0=1;break;
		case 2:
			PA1=1;break;
		case 3:
			PC1=1;break;
		case 4:
			PC0=1;break;
	}	
}

void ledHandler(void)
{
	u8 i;
	
	//charging state
	if(getSysTick() & SHOW_CHARGING_TICK)
	{
		for(i=1;i<5;i++)
		{	
			if(gBatStateBuf[i] & 0x3C)
				LED_ON(i);
		}
	}
	else
	{
		for(i=1;i<5;i++)
		{	
			if(gBatStateBuf[i] & 0x3C)
				LED_OFF(i);
		}		
	}



	//error state

	if(getSysTick() & 0x08)
	{
		for(i=1;i<5;i++)
		{	
			if((gBatStateBuf[i] & 0x40))
				LED_ON(i);
		}	
	}
	else
	{
		for(i=1;i<5;i++)
		{	
			if((gBatStateBuf[i] & 0x40))
				LED_OFF(i);
		}			
	}

	
}
