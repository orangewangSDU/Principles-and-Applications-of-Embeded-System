#include "stc15.h"

/* 数码管段码，共阳极（低电平亮） */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                                0x92, 0x82, 0xF8, 0x80, 0x90};

/* 显示缓冲区 */
unsigned char DigitBuffer[4];
unsigned char DigitReady = 1;

unsigned char count_20ms = 0;
unsigned char task_20ms = 0;

unsigned char pos = 0;
unsigned char now = 0;

/* 按键处理 */
bit key4_pressed = 0;

/* 定时器0初始化 - 5ms */
void timer0_init(void)
{
    AUXR &= 0x7F;    // 12T模式
    TMOD &= 0xF0;    // 清除T0控制位
    TL0 = 0x00;      
    TH0 = 0xEE;      
    TF0 = 0;
    TR0 = 1;         // 启动T0
    ET0 = 1;         // 允许T0中断
}

/* 动态扫描显示 */
void DisplayDigits()
{
    // 关闭所有位选
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // 段选
    P0 = nums[DigitBuffer[pos]];

    // 位选
    switch(pos)
    {
        case 0: P41 = 0; break;
        case 1: P42 = 0; break;
        case 2: P43 = 0; break;
        case 3: P44 = 0; break;
    }

    pos++;
    if (pos >= 4) pos = 0;
}

/* 定时器0中断函数 - 每5ms */
void timer0_inter_handler(void) interrupt 1
{
    count_20ms++;
    if (count_20ms >= 4) // 攒够了20ms任务
    {
        count_20ms = 0;
        task_20ms = 1;
    }

    // 数码管动态刷新（5ms扫描一次）
    if (DigitReady)
    {
        DisplayDigits();
    }
}

/* 主函数 */
void main()
{
    /* IO初始化 */
    P2M0 = 0xff; P2M1 = 0x00;
    P4M0 = 0xff; P4M1 = 0x00;
    P0M0 = 0xff; P0M1 = 0x00;

    /* 数码管控制IO默认全亮 */
    P41 = 0; 
	P42 = 0; 
	P43 = 0; 
	P44 = 0;

    timer0_init();  // 定时器初始化
    EA = 1;         // 开启总中断

    while (1)
    {
        if (task_20ms)
        {
            task_20ms = 0;

            // 检测K4是否按下
            if ((P1 & 0x20) == 0) // P1.5为K4，低电平为按下
            {
                if (!key4_pressed)
                {
                    key4_pressed = 1;
                }
            }
            else // 松手
            {
                if (key4_pressed)
                {
                    key4_pressed = 0;
                    now++;
                    if (now > 15) now = 0;
                }
            }

            // 更新数码管显示
            DigitBuffer[0] = now / 1000 % 10;
            DigitBuffer[1] = now / 100 % 10;
            DigitBuffer[2] = now / 10 % 10;
            DigitBuffer[3] = now % 10;
        }
    }
}
