#include <stc15.h>        //STC单片机头文件
#include "dataType.h"    //#include <>，只在系统指定文件夹搜索该头文件，系统头文件一般采用#include <>

int flag=0;
int time=0,cnt=0,dir=0;

void timer0_init(void)        //100us @11.0592MHz
{
    AUXR |= 0x80;            //定时器时钟1T模式
    TMOD &= 0xF0;            //设置定时器模式
    TL0 = 0xAE;                //设置定时初始值
    TH0 = 0xFB;                //设置定时初始值
    TF0 = 0;                //清除TF0标志
    TR0 = 1;                //定时器0开始计时

    ET0 = 1;                //允许定时器中断
}


void timer0_inter_handler(void) interrupt 1   //中断处理
{
    cnt++;
    cnt%=100;
    
    if(time>=99)
    {
        time=0;
        cnt=0;
        dir=!dir;
    }
    
    if(dir==0)
    {
        if(cnt>time)    
            P2=0xff;
        if(cnt<time)
            P2=0;
        if(cnt>=99)
            time++;
    }
    
    if(dir==1)
    {
        if(cnt>time)
            P2=0;
        if(cnt<time)
            P2=0xff;
        if(cnt>=99)
            time++;
    }
}