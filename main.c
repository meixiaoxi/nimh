#include <hic.h>
#include "nimh.h"

u32 tick=0;
u32 shortTick=0;
u8 gIsChargingBatPos=1;   //此刻正在充电电池的标记    


u32 ChargingTimeTick = 0;
u16 gChargeCurrent;

//type_error charge_error  pre  fast sup  trick  charging(on_off)  is_detect(电池检测) bat_state(valid)
//the first byte is a dummy						
u8 gBatStateBuf[5] = {0x0,0x10,0x00,0x00,0x10};

u16 gBatVoltArray[4][6] = {	{0,0,0,0,0,0},
						{0,0,0,0,0,0},
						{0,0,0,0,0,0},
						{0,0,0,0,0,0},
					  };

u8 gChargeSkipCount[] = {0,0,0,0};    //控制PWM周期

u8 gBatNumNow = 0;


		// 1/2/3/4号电池
u8 gBatNowBuf[5]={0,0,0,0,0};  //存放充电器上电池的标号

void isr(void) interrupt
{
	if(T8NIF)
	{
		T8NIF = 0;
		tick++;
	}
	if(T8P1IF)
	{
		T8P1IF=0;
		shortTick++;
	}
}



void BatPreCharge(u8 batLabel)
{
	if((PB & batLabel) == 0)  //not charging
	{
		if(gChargeSkipCount[batLabel] >= FAST_SKIP_COUNT)
		{
			PB |= batLabel;
			gChargeSkipCount[batLabel] =0;
		}
	}
}

void BatFastCharge(u8 batLabel)
{
	if((PB & batLabel) == 0)  //not charging
	{
		if(gChargeSkipCount[batLabel] >= FAST_SKIP_COUNT)
		{
			PB |= batLabel;
			gChargeSkipCount[batLabel] =0;
		}
	}
}

void BatSupCharge(u8 batLabel)
{
	if((PB & batLabel) == 0)  //not charging
	{
		if(gChargeSkipCount[batLabel] >= FAST_SKIP_COUNT)
		{
			PB |= batLabel;
			gChargeSkipCount[batLabel] =0;
		}
	}
}

void BatTrickCharge(u8 batLabel)
{
	if((PB & batLabel) == 0)  //not charging
	{
		if(gChargeSkipCount[batLabel] >= FAST_SKIP_COUNT)
		{
			PB |= batLabel;
			gChargeSkipCount[batLabel] =0;
		}
	}	
}


void removeBat(u8 toChangeBatPos)
{
	u8 i;

	gChargeSkipCount[gBatNowBuf[toChangeBatPos] -1 ] = 0;
	gBatStateBuf[gBatNowBuf[toChangeBatPos]] =0;

	
	for(i=toChangeBatPos;i<gBatNumNow;i++)
	{
		gBatNowBuf[i] = gBatNowBuf[i+1];
	}
	gBatNowBuf[gBatNumNow] = 0;
	gBatNumNow--;
}

void chargeHandler(void)
{
	u32 ticknow = getBatTick();
	u16 tempV;

	static u8 skipCount = 0;
	
	//close all pwm
	//PB &= 0xF0;

	if(gBatNumNow ==0)
		return;

	if(ChargingTimeTick == 0)   
	{
		switch(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]]& 0x3C)	
		{
			case CHARGE_STATE_FAST:
					skipCount = FAST_SKIP_COUNT; break; //BatFastCharge(ticknow%4);break;
			case CHARGE_STATE_SUP:
					skipCount = SUP_SKIP_COUNT;break;// BatSupCharge(ticknow%4);break;
			case CHARGE_STATE_PRE:
					skipCount = PRE_SKIP_COUNT;break;//BatPreCharge(ticknow%4);break;
			case CHARGE_STATE_TRICK:
					skipCount = TRI_SKIP_COUNT;break;//BatTrickCharge(ticknow%4);break;
			default:
					break;
		}

		ChargingTimeTick = getSysTick();
		if(ChargingTimeTick == 0)
			ChargingTimeTick =1;
		if(gIsChargingBatPos !=0)    //0 pos is a dummy
		{
			//电池类型错误    充电错误
			if((gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & BAT_TYPE_ERROR) || gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & CHARGE_STATE_ERROR)
				return;	
	
			if(gChargeSkipCount[gBatNowBuf[gIsChargingBatPos]-1] >=skipCount)   //ok, it's pulse now
			{
				PB &= 0xF0;   //close all pwm
				PB |= (gBatNowBuf[gIsChargingBatPos]);
			}
			else
				gChargeSkipCount[gBatNowBuf[gIsChargingBatPos] - 1]++;
		}
	}
	else
	{
		if(getDiffTickFromNow(ChargingTimeTick)  > BAT_CHARGING_PULSE_TIME)   //change to next channel
		{
			PB &= ~(gBatNowBuf[gIsChargingBatPos]);   //close current pwm channel
			ChargingTimeTick = 0;
			if(gIsChargingBatPos >= gBatNumNow)
			{
				if(gBatNumNow%2)
				{
					gIsChargingBatPos =0;
				}
				else
					gIsChargingBatPos =1;
			}
			else
				gIsChargingBatPos++;
		}
		else if(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & BAT_DETECT_BIT)   //电池检测
		{
			if(getDiffTickFromNow(ChargingTimeTick)>BAT_CHARGING_DETECT_TIME)
			{
				tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);
				if(tempV<BAT_MIN_VOLT_OPEN)   //电池被拔出
				{
					ChargingTimeTick = 0;
					removeBat(gIsChargingBatPos);
				}
				else if(tempV>BAT_MAX_VOLT_CLOSE  || (gChargeCurrent<<3) > tempV-gBatVoltArray[gBatNowBuf[gIsChargingBatPos]-1][0])  //
				{
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= ~(BAT_DETECT_BIT |CHARGE_STATE_ALL);
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= BAT_TYPE_ERROR;
					ChargingTimeTick = 0;
				}
				else
				{
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] |= HAS_BATTERY;
					gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] &= ~ BAT_CHECK_BIT;
				}
  					
  			}
		}
		else if(gIsChargingBatPos !=0)
		{ 
			tempV = getVbatAdc(gBatNowBuf[gIsChargingBatPos]);
			if(tempV < BAT_MIN_VOLT_OPEN)
			{
					ChargingTimeTick = 0;
					removeBat(gIsChargingBatPos);
			}
			if(gBatStateBuf[gBatNowBuf[gIsChargingBatPos]] & CHARGE_STATE_ERROR)
			{
				//温度检测
			}
			
		}
	}
}

void InitConfig()
{
	DisWatchdog();
	
	//IO led1~4  输出低
	PAT 	&=	0xFC;
	PCT	&=	0xFC;
	PA	|= 0x3;
	PC   |= 0x3;

	//pwm
	PBT &= 0xF0;
	PB  &= 0xF0;

	//timerN
	//bit    000(1:128) 	 1(en)	  NE	      0(timer mode)       1(时钟源为WDT)      0( dis)
	//	T8NPRS	    T8NPRE	T8NEG  T8NM	                T8NCLK		                      T8EN
	T8NC =   0x4E;
	T8N = 0;
	T8NIF = 0;
	T8NIE=1;

	//t8p1
	T8P1=0; //celar count
	T8P1P=0xFF; //计数周期
	T8P1C= 0x7B; ///定时器模式，预分频比1:16, 后分频比1:16
	T8P1IF = 0;
	T8P1IE = 1;	

	
	//ADC	
	ANSL = 0x03;   //AIN2~AIN6
	ANSH = 0x9F;	 //AIN12 AIN13

	#if 1
	ADCTST = 0x00;   //AD转换速度为低速
	ADCCL	= 0x05;  //硬件控制采样
	ADCCH = 0x4C;   //ADC采样时间为16个ADC时钟，采样值高位对齐，A/D转换时钟频率为Fosc/16，参考电压为内部2.6V
	#else
	ADCTST = 0x00;   //AD转换速度为低速
	ADCCL	= 0x05;  //硬件控制采样
	ADCCH = 0x4D;
	VRC1 = 0x80;      // internal 2.6V enbale
	#endif
	//uart
    PC = 0;         //设置PC端口输出低电平 
    PCT1 = 0;       //TX方向输出
    PCT0 = 1;       //RX方向输入
    BRGH = 1;       //波特率高速模式，Fosc/(16*(BRR+1))
    BRR = 51;       //设置波特率9600，BRR=8MHz/9600/16-1
    TXM = 0;        //发送8位数据格式
    TXEN = 1;       //UART发送使能   
// RXM = 0;        //接收8位数据格式
   //RXEN = 1;       //UART接收使能

	
}
u16 vTemp;
		extern void LED_ON(u8 led);
		extern void LED_OFF(u8 led);
		extern void delay_ms(u16);
void main() 
{
	
	u8 cur_detect_pos=1;
	u16 tempVoltClose,tempVoltOpen;
	
	InitConfig();

	RCEN=1;
	GIE=1;

	t8n_start();
	t8p1_start();
	
		LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
		//LED_ON(1);delay_us(600);


	while(1)
	{	
		#if 1
		if(gBatStateBuf[cur_detect_pos] & HAS_BATTERY)
		{
			//getVbatAdc(cur_detect_pos);
		}
		else
		{
		
			if((gBatStateBuf[cur_detect_pos] & BAT_DETECT_BIT) == 0)
			{	
						LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
			tempVoltClose =  getVbatAdc(cur_detect_pos);	//gBatVoltArray[cur_detect_pos-1][0]= getVbatAdc(cur_detect_pos);
							LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
				if(gBatVoltArray[cur_detect_pos-1][0] >= BAT_MIN_VOLT_OPEN)
				{
					if(gBatVoltArray[cur_detect_pos-1][0] >= BAT_MAX_VOLT_OPEN)
					{
						gBatStateBuf[cur_detect_pos] |= BAT_TYPE_ERROR;
					}
					else
					{
						gBatStateBuf[cur_detect_pos] |= (CHARGE_STATE_PRE|BAT_DETECT_BIT);
					}
					gBatNumNow++;					
					gBatNowBuf[gBatNumNow] = cur_detect_pos;
				}
			}	
		}
		#endif
		LED_ON(1);delay_ms(600);
		LED_OFF(1);delay_ms(600);
		LED_ON(1);delay_ms(600);
		chargeHandler();
		LED_OFF(1);delay_ms(600);
		//ledHandler();
		cur_detect_pos++;
		if(cur_detect_pos%4 == 0)
			cur_detect_pos=1;
		else
			cur_detect_pos = cur_detect_pos%4;

	}
		
	
	
}

