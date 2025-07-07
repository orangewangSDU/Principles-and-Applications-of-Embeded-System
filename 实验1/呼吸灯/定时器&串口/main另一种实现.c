#include<stc15.h>
unsigned char pwmDuty = 0;	// 占空比
unsigned char pwmCounter = 0;
unsigned char direction = 0; // 控制增加还是减少

void Timer0_Init(void)		//10微秒@11.0592MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x91;				//设置定时初始值
	TH0 = 0xFF;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}

void Timer1_Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x00;				//设置定时初始值
	TH1 = 0xDC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;
}


void Timer0_Isr(void) interrupt 1
{
	if(pwmCounter < pwmDuty)
		P2 = 0xff;
	else
		P2 = 0x00;
	pwmCounter++;
	pwmCounter %= 100;
}

void Timer1_Isr(void) interrupt 3
{
	if(direction == 0){
		pwmDuty++;
	}
	else
		pwmDuty--;
	if(pwmDuty >= 100){
		direction = 1;	// 1说明开始减少
	}
	if(pwmDuty == 0)
		direction = 0;	// 0说明开始增加
}

void main(){
	P2M0 = 0x00; P2M1 = 0x00; 

	P2 = 0x00;
	Timer0_Init();
	Timer1_Init();
	EA = 1;			// 开全局中断
	while(1){}
}