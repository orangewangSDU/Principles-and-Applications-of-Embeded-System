#include "stc15.h"

/* 数码管显示的数字 */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};//0-9，共阳极段码表

/* 存储数字 */
unsigned char DigitBuffer[4]; // 用于存储4位数字的每一位
unsigned char DigitIndex = 0;
unsigned char DigitReady = 0; // 是否准备好显示数字

unsigned char count_20ms,count20ms,task_20ms;

/* 展示位 */
unsigned char pos = 0;

void timer0_init(void)		//5毫秒@11.0592MHz
{
	count_20ms = 0;

	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0xEE;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

	ET0 = 1;			//允许定时器中断
}

void timer0_inter_handler(void) interrupt 1
{
	count_20ms ++;
	count20ms=1;
	if(count_20ms>=25) 
	{
		count_20ms = 0;
	}
	task_20ms = 1;
}

//添加展示函数
void DisplayDigits()
{
    // 段选 - 送出共阳极段码（低电平亮）
    P0 = nums[DigitBuffer[pos]];

	
    // 清除所有位选（熄灭所有数码管）
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // 位选（哪个数码管显示）
    switch(pos)
    {
        case 0: P41 = 0; break; // 第1位
        case 1: P42 = 0; break; // 第2位
        case 2: P43 = 0; break; // 第3位
        case 3: P44 = 0; break; // 第4位
    }

    // 准备下次显示下一位
    pos++;
    if (pos >= 4) pos = 0;
}


void main()
{

	/* LED-IO寄存器初始化 */
	P2M0 = 0xff;
	P2M1 = 0x00;
	P4M0 = 0xff;  // 将P4口设置为推挽输出
	P4M1 = 0x00;
	P0M0 = 0xff;  // 将P0口设置为推挽输出
	P0M1 = 0x00;

	
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
	
	timer0_init();  //定时器0初始化，加在串口初始化之后
	
	EA=1;				//开全局中断
	
	while (1) 
	{
		if (task_20ms) 
		{
			task_20ms = 0;
			if (DigitReady) 
			{
				DisplayDigits();  // 每20ms刷新一次数码管
			}
		}
	}

}

void Uart_Int(void) interrupt 4
{
	unsigned char RxData;

	//修改以下内容
	if (RI) 
	{
		RI = 0;
		RxData = SBUF;

		// 判断是否为数字字符 '0'~'9'
		if (RxData >= '0' && RxData <= '9') 
		{
			if (DigitIndex < 4)
			{
				DigitBuffer[DigitIndex] = RxData - '0'; // 转换为数字
				DigitIndex++;
			}
			if (DigitIndex == 4) 
			{
				DigitReady = 1; // 表示数字已接收完成
				DigitIndex = 0; // 准备下次接收
			}
		} 
		else 
		{
			DigitIndex = 0; // 若不是数字，清空重新开始
		}
	}
}










