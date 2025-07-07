#include "stc15.h"

/* 数码管段码，共阳极（低电平亮） */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                                0x92, 0x82, 0xF8, 0x80, 0x90};

/* 显示缓冲区 */
unsigned char DigitBuffer[4] = {0, 0, 0, 0};
unsigned char DigitReady = 1;

unsigned char count_20ms = 0;
unsigned char task_20ms = 0;

unsigned char pos = 0;
unsigned int timer_set = 0;    // 设置的定时时间（秒）
unsigned int timer_count = 0;  // 当前计时值（秒）
unsigned char flash_count = 0; // 闪烁计数
unsigned char flash_state = 1; // 闪烁状态（1表示显示）

/* 按键状态 */
bit key1_pressed = 0;
bit key2_pressed = 0;
bit key3_pressed = 0;
bit timer_running = 0;         // 计时器运行标志
bit timer_finished = 0;        // 计时完成标志

/* 动态扫描显示 */
void DisplayDigits()
{
    // 关闭所有位选
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // 如果计时完成且处于闪烁关闭状态，不显示
    if(timer_finished && !flash_state)
    {
        P0 = 0xFF;  // 关闭所有段
    }
    else
    {
        // 根据状态显示内容
        if(timer_finished)
        {
            // 显示8888
            P0 = nums[8];
        }
        else
        {
            // 正常显示当前数字
            P0 = nums[DigitBuffer[pos]];
        }
    }

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

/* 定时器0中断函数 - 每5ms 用于检测按键状态*/
void timer0_inter_handler(void) interrupt 1
{
    TL0 = 0x00;      
    TH0 = 0xEE; 
    
    count_20ms++;
    if (count_20ms >= 4) // 20ms任务
    {
        count_20ms = 0;
        task_20ms = 1;
    }

    // 数码管动态刷新（5ms扫描一次）
    DisplayDigits();
}

/* 定时器1初始化 - 50ms */
void timer1_init(void)
{
    AUXR &= 0xBF;    // 12T模式
    TMOD &= 0x0F;    // 清除T1控制位
    TL1 = 0xB0;      // 50ms定时
    TH1 = 0x3C;      
    TF1 = 0;
    TR1 = 0;         // 初始不启动
    ET1 = 1;         // 允许T1中断
}

/* 定时器1中断函数 - 50ms */
void timer1_inter_handler(void) interrupt 3
{
    TL1 = 0xB0;      
    TH1 = 0x3C; 
    
    if(timer_running)
    {
        static unsigned char count_1s = 0;
        count_1s++;
        
        if(count_1s >= 20)  // 1秒到，20个50ms就是1s
        {
            count_1s = 0;
            if(timer_count > 0)
            {
                timer_count--;
                
                // 更新显示
                DigitBuffer[0] = timer_count / 1000 % 10;
                DigitBuffer[1] = timer_count / 100 % 10;
                DigitBuffer[2] = timer_count / 10 % 10;
                DigitBuffer[3] = timer_count % 10;
            }
            else
            {
                timer_running = 0;
                timer_finished = 1;
                TR1 = 1;  // 继续运行用于闪烁
            }
        }
    }
    else if(timer_finished)
    {
        flash_count++;
        if(flash_count >= 10)  // 0.5s切换一次（10*50ms）
        {
            flash_count = 0;
            flash_state = !flash_state;
        }
    }
}

/* 更新显示缓冲区 */
void UpdateDisplay(void)
{
    DigitBuffer[0] = timer_count / 1000 % 10;
    DigitBuffer[1] = timer_count / 100 % 10;
    DigitBuffer[2] = timer_count / 10 % 10;
    DigitBuffer[3] = timer_count % 10;
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

    timer0_init();  // 定时器0初始化 - 5ms
    timer1_init();  // 定时器1初始化 - 50ms
    EA = 1;         // 开启总中断

    // 初始显示0000
    UpdateDisplay();

    while (1)
    {
        if (task_20ms)
        {
            task_20ms = 0;

            // 检测K1是否按下 (P1.2)
            if ((P1 & 0x04) == 0)
            {
                if (!key1_pressed)
                {
                    key1_pressed = 1;
                }
            }
            else // 松手
            {
                if (key1_pressed)
                {
                    key1_pressed = 0;
                    if(!timer_running && !timer_finished)  // 只有在非计时状态才能调整时间
                    {
                        timer_set++;
                        if(timer_set > 9999) timer_set = 9999; // 计时器上限9999
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                }
            }

            // 检测K2是否按下 (P1.3)
            if ((P1 & 0x08) == 0)
            {
                if (!key2_pressed)
                {
                    key2_pressed = 1;
                }
            }
            else // 松手
            {
                if (key2_pressed)
                {
                    key2_pressed = 0;
                    if(!timer_running && !timer_finished)  // 只有在非计时状态才能调整时间
                    {
                        if(timer_set > 0) timer_set--;
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                }
            }

            // 检测K3是否按下 (P1.4)
            if ((P1 & 0x10) == 0)
            {
                if (!key3_pressed)
                {
                    key3_pressed = 1;
                }
            }
            else // 松手
            {
                if (key3_pressed)
                {
                    key3_pressed = 0;
                    if(timer_finished)
                    {
                        // 如果计时完成，再次按下K3重新开始
                        timer_finished = 0;
                        flash_state = 1;
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                    
                    if(timer_count > 0 && !timer_running)
                    {
                        timer_running = 1;
                        TR1 = 1;  // 启动定时器1
                    }
                }
            }
        }
    }
}