#include "stc15.h"

/* 串口发送变量 */
unsigned char TxTail;
unsigned char TxPtr;
unsigned char TxBuff[5];

/* 接受hello字符串 */
unsigned char HelloBuf[5];     //接收缓冲区
unsigned char HelloIndex = 0;  //索引变量


void main()
{

	/* LED-IO寄存器初始化 */
	P2M0 = 0xff;
	P2M1 = 0x00;

	/* 变量初始化 */
	TxPtr=0;
	TxTail=0;
	
	/* 串口寄存器初始化 */
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器，波特率=溢出率/4
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE0;			//设置定时初始值，9600bps
	TH1 = 0xFE;			//设置定时初始值，f=11.0592MHz
	ET1 = 0;			//禁止定时器1中断
	ES = 1;				//允许串口1中断
	TR1 = 1;			//定时器1开始计时
	
	EA=1;				//开全局中断
	
	while(1) {}
}



void Uart_Int(void) interrupt 4
{
	unsigned char RxData;

	if(RI) 
	{
		RI = 0;
		RxData = SBUF;
		// 检查是否匹配 "hello"
		if ((RxData == "hello"[HelloIndex]) && HelloIndex < 5) //依次比较hello中的每个字符是否和RxData中的字符相等
		{
			HelloBuf[HelloIndex++] = RxData;
			if (HelloIndex == 5) 
			{
				// 匹配成功，准备发送 "world"
				TxBuff[0] = 'w';
				TxBuff[1] = 'o';
				TxBuff[2] = 'r';
				TxBuff[3] = 'l';
				TxBuff[4] = 'd';
				TxTail = 5;
				TxPtr = 0;
				SBUF = TxBuff[TxPtr];
				HelloIndex = 0; // 重新准备下一次接收
			}
		} 
		else 
		{
			HelloIndex = 0; // 匹配失败，重新开始
		}
	}
	if(TI) 
	{
		TI = 0;
		TxPtr ++ ;
		if(TxPtr < TxTail) 
		{
			SBUF = TxBuff[TxPtr];
		}
	}
}










