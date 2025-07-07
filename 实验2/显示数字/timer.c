#include <stc15.h>		//STC单片机头文件
#include "dataType.h"	//#include <>，只在系统指定文件夹搜索该头文件，系统头文件一般采用#include <>

u8 count_20ms,count20ms,task_500ms,task_20ms;

void timer0_init(void)		//5毫秒@11.0592MHz
{
	task_500ms = 0;
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
	if(count_20ms>=25) {
		count_20ms = 0;
		task_500ms = 1;
	}
	task_20ms = 1;

}