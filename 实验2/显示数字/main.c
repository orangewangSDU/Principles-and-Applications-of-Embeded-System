#include <stc15.h>		//STC单片机头文件
#include <intrins.h>	//内含nop指令，用于延时
#include <absacc.h>		//绝对地址访问

#include "dataType.h"	//#include <>，只在系统指定文件夹搜索该头文件，系统头文件一般采用#include <>
						//#include ""，先在当前工程所在文件夹搜索该头文件，若找不到再到系统指定文件夹搜索。用户自定义头文件一般用#include ""

//将鼠标移动到头文件所在行，点下左键，然后鼠标右键，选择"Open document <xxx.h>"可打开该头文件
//头文件打开后，将鼠标箭头移动至该头文件选项卡，点鼠标右键，选择"Open Containing Folder"，可定位头文件所在文件夹
//注释容易出现乱码，点击“Edit”---》“Configuration”---》General Editor Settings：Encoding---》Chinese GB2312 (Simplified)

#define Tx_Array_Size	32
#define Rx_Array_Size	32

u8 Tx_Buffer[Tx_Array_Size];
u8 Tx_Ptr,Tx_Tail,Rx_Ptr;

//float code PI=3.1415926;

u8 xdata nums[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};//其二进制指明了每个数字中应该点亮哪一段数码管
int idx=0;
int now=0;//索引到nums数组中的具体数

int mode = 0; //定义当前数码管的状态数字

u16 xdata array[4]={0x1122,0x3344,0x5566,0x7788};//数组赋初值，存储时数据高字节在低地址
u8 *ptr;
//u16 *ptr;

extern u8 task_500ms;
extern u8 task_20ms;
extern void timer0_init(void);	//20毫秒@11.0592MHz

extern void adc_init();
extern u16 adcRes[2];			//转换结果


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



	
void uart_init()//串口初始化
{
	Rx_Ptr = 0;			//变量初始化
						// SFR配置，9600bps
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器，波特率=溢出率/4
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE0;			//设置定时初始值，f=11.0592MHz
	TH1 = 0xFE;			//设置定时初始值
	ET1 = 0;			//禁止定时器1中断
	ES = 1;				//允许串口1中断
	TR1 = 1;			//定时器1开始计时
}


void main()//主程序
{

	P2M0 = 0xff;
	P2M1 = 0x00;
    
    P0M0 = 0xff;
    P0M1 = 0x00; 

    P4M0 = 0xff;
    P4M1 = 0x00; 
    
    P1M0 = 0xff;
    P1M1 = 0x00; 

	timer0_init();	//定时器0初始化
	uart_init();	//串口初始化
	adc_init();		//ADC初始化
	
    P41=0;//0表示亮，四个数码始终都亮，所以直接全置零即可
    P42=0;
    P43=0;
    P44=0;
	EA=1;//开全局中断
	
	while(1)
	{	
       //首先检测K1K2K3K4哪个按键被按下，方法是看P1的值
       if (P1==0xdf && mode==0)//11011111->11 0111 11->根据0111看出K4置为0，说明K4按下去了
       {
           Delay20ms();//去抖动
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
            
        
       if(task_20ms)//每隔20ms进行一次刷新
       {
           task_20ms = 0;
           P0=nums[now];
       }
    }
}