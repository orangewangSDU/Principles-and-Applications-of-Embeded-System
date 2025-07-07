#include <stc15.h>		//STC��Ƭ��ͷ�ļ�
#include <intrins.h>	//�ں�nopָ�������ʱ
#include <absacc.h>		//���Ե�ַ����

#include "dataType.h"	//#include <>��ֻ��ϵͳָ���ļ���������ͷ�ļ���ϵͳͷ�ļ�һ�����#include <>
						//#include ""�����ڵ�ǰ���������ļ���������ͷ�ļ������Ҳ����ٵ�ϵͳָ���ļ����������û��Զ���ͷ�ļ�һ����#include ""

//������ƶ���ͷ�ļ������У����������Ȼ������Ҽ���ѡ��"Open document <xxx.h>"�ɴ򿪸�ͷ�ļ�
//ͷ�ļ��򿪺󣬽�����ͷ�ƶ�����ͷ�ļ�ѡ���������Ҽ���ѡ��"Open Containing Folder"���ɶ�λͷ�ļ������ļ���
//ע�����׳������룬�����Edit��---����Configuration��---��General Editor Settings��Encoding---��Chinese GB2312 (Simplified)

#define Tx_Array_Size	32
#define Rx_Array_Size	32

u8 Tx_Buffer[Tx_Array_Size];
u8 Tx_Ptr,Tx_Tail,Rx_Ptr;

//float code PI=3.1415926;

u8 xdata nums[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};//�������ָ����ÿ��������Ӧ�õ�����һ�������
int idx=0;
int now=0;//������nums�����еľ�����

int mode = 0; //���嵱ǰ����ܵ�״̬����

u16 xdata array[4]={0x1122,0x3344,0x5566,0x7788};//���鸳��ֵ���洢ʱ���ݸ��ֽ��ڵ͵�ַ
u8 *ptr;
//u16 *ptr;

extern u8 task_500ms;
extern u8 task_20ms;
extern void timer0_init(void);	//20����@11.0592MHz

extern void adc_init();
extern u16 adcRes[2];			//ת�����


void Delay20ms(void)	//@11.0592MHz
{
	unsigned char data i, j;

	i = 216;
	j = 37;
	do
	{
		while (--j);
	} while (--i);
}



	
void uart_init()//���ڳ�ʼ��
{
	Rx_Ptr = 0;			//������ʼ��
						// SFR���ã�9600bps
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�������������=�����/4
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xE0;			//���ö�ʱ��ʼֵ��f=11.0592MHz
	TH1 = 0xFE;			//���ö�ʱ��ʼֵ
	ET1 = 0;			//��ֹ��ʱ��1�ж�
	ES = 1;				//������1�ж�
	TR1 = 1;			//��ʱ��1��ʼ��ʱ
}


void main()//������
{

	P2M0 = 0xff;
	P2M1 = 0x00;
    
    P0M0 = 0xff;
    P0M1 = 0x00; 

    P4M0 = 0xff;
    P4M1 = 0x00; 
    
    P1M0 = 0xff;
    P1M1 = 0x00; 

	timer0_init();	//��ʱ��0��ʼ��
	uart_init();	//���ڳ�ʼ��
	adc_init();		//ADC��ʼ��
	
    P41=0;//0��ʾ�����ĸ�����ʼ�ն���������ֱ��ȫ���㼴��
    P42=0;
    P43=0;
    P44=0;
	EA=1;//��ȫ���ж�
	
	while(1)
	{	
       //���ȼ��K1K2K3K4�ĸ����������£������ǿ�P1��ֵ
       if (P1==0xdf && mode==0)//11011111->11 0111 11->����0111����K4��Ϊ0��˵��K4����ȥ��
       {
           Delay20ms();//ȥ����
           if(P1==0xdf)
           {
               mode=4;
               now=4;
           }
       }
            
       if (P1==0xef && mode==0)
       {
           Delay20ms();
           if(P1==0xef)
           {
               mode=3;
			   now=3;
           }
       }
       if (P1==0xf7 && mode==0)
       {
           Delay20ms();
           if(P1==0xf7)
           {
               mode=2;
			   now=2;
           }
       }
	   if (P1==0xfb && mode==0)
       {
           Delay20ms();
           if(P1==0xfb)
           {
               mode=1;
               now=1;
           }
       }
	   if (P1==0xff && mode!=0)
       {
           Delay20ms();
           if(P1==0xff)
           {
               mode=0;
           }
       }
            
        
       if(task_20ms)//ÿ��20ms����һ��ˢ��
       {
           task_20ms = 0;
           P0=nums[now];
       }
    }
}