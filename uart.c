#include "nimh.h"
#include "hic.h"

#if 0
const char digits[] = "0123456789abcdef";
void send(u16 sData)
{
	signed char i;

	for(i=3; i>=0; i--)
	{
        while(!TRMT);           //�ȴ�������λ�Ĵ���TXR��
        TXB  =  digits[(sData>>(i*4))&0xf];   //�����ַ���
        while(!TXIF);           //�ȴ������жϱ�־λ 
        TXIF = 0;               //�巢���жϱ�־λ 
	}

	  while(!TRMT);           //�ȴ�������λ�Ĵ���TXR��
        TXB  =  ' ';   //�����ַ���
        while(!TXIF);           //�ȴ������жϱ�־λ 
        TXIF = 0;               //�巢���жϱ�־λ 

}

void sendStr(char str[])
{
	u8 i;

	for(i=0;i<5;i++)
	{
        while(!TRMT);           //�ȴ�������λ�Ĵ���TXR��
        TXB  =  str[i];   //�����ַ���
        while(!TXIF);           //�ȴ������жϱ�־λ 
        TXIF = 0;               //�巢���жϱ�־λ 	
	}

	while(!TRMT);           //�ȴ�������λ�Ĵ���TXR��
        TXB  =  ' ';   //�����ַ���
        while(!TXIF);           //�ȴ������жϱ�־λ 
        TXIF = 0;               //�巢���жϱ�־λ 
}
#endif
