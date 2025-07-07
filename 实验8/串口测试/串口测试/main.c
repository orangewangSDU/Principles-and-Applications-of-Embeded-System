#include "stc15.h"

/* 数码管显示的数字 */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // 0-9，共阳极段码表

/* 存储数字 */
unsigned char DigitBuffer[4]; // 用于存储4位数字的每一位
unsigned char DigitReady = 0; // 是否准备好显示数字

/* 定时相关 */
unsigned char count_5ms = 0;
unsigned char count_20ms = 0;
bit task_5ms = 0;
bit task_20ms = 0;
unsigned char count_1s = 0;     // 用于1秒计数
unsigned char timer_running = 0; // 计时器运行状态
unsigned int seconds = 0;       // 总秒数（0-9999）

/* 串口发送相关 */
unsigned char TxBuff[4];        // 只存放4位数字的ASCII码
unsigned char TxIndex = 0;
unsigned char TxLen = 4;
unsigned char TxBusy = 0;    //防止重复发送

/* 展示位 */
unsigned char pos = 0;

// 按键定义（修改为P1.2-P1.4）
#define K1 (P1 & 0x04)  // P1.2
#define K2 (P1 & 0x08)  // P1.3 
#define K3 (P1 & 0x10)  // P1.4

//按键状态
bit key1_pressed = 0;
bit key2_pressed = 0;
bit key3_pressed = 0;

void timer0_init(void) // 5毫秒@11.0592MHz
{
    AUXR &= 0x7F; // 定时器时钟12T模式
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0x00;   // 设置定时初始值
    TH0 = 0xEE;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
    ET0 = 1;      // 允许定时器中断
}

void timer0_inter_handler(void) interrupt 1
{
    /* 5ms定时 用于刷新数码管*/
    task_5ms = 1;
    count_5ms++;
    
    /* 20ms定时 用于检测按键*/
    if(++count_20ms >= 4) // 4*5ms=20ms
    {
        count_20ms = 0;
        task_20ms = 1;
    }
    
    /* 1s定时 用于发送数据，只有正在计时的时候才开始计1s*/
    if(count_5ms >= 200 && timer_running) // 200*5ms=1s
    {
        count_5ms = 0;
        count_1s = 1;
    }
}

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
    switch (pos)
    {
    case 0: P41 = 0; break; // 第1位
    case 1: P42 = 0; break; // 第2位
    case 2: P43 = 0; break; // 第3位
    case 3: P44 = 0; break; // 第4位
    }

    // 准备下次显示下一位
    pos++;
    if (pos >= 4)
        pos = 0;
}

// 更新数码管显示和串口发送缓冲区
void UpdateTimeData()
{
    // 更新数码管显示数据
    DigitBuffer[0] = seconds / 1000 % 10; // 千位
    DigitBuffer[1] = seconds / 100 % 10;  // 百位
    DigitBuffer[2] = seconds / 10 % 10;   // 十位
    DigitBuffer[3] = seconds % 10;        // 个位
    DigitReady = 1;

    // 更新串口发送缓冲区（仅4位数字）
    TxBuff[0] = DigitBuffer[0] + '0'; // 千位
    TxBuff[1] = DigitBuffer[1] + '0'; // 百位
    TxBuff[2] = DigitBuffer[2] + '0'; // 十位
    TxBuff[3] = DigitBuffer[3] + '0'; // 个位
}

// 开始串口发送（发送4位数字）
void StartUartSend()
{
    if (!TxBusy)
    {
        TxIndex = 0;
        TxBusy = 1;
        SBUF = TxBuff[TxIndex++]; // 发送第一个数字
    }
}

void main()
{
    /* IO寄存器初始化 */
    P2M0 = 0xff;
    P2M1 = 0x00;
    P4M0 = 0xff; // 将P4口设置为推挽输出
    P4M1 = 0x00;
    P0M0 = 0xff; // 将P0口设置为推挽输出
    P0M1 = 0x00;
    
    // 设置P1.2-P1.4为输入模式（准双向口）
    P1M0 &= ~0x1C; // P1.2-P1.4
    P1M1 &= ~0x1C;

    /* 串口寄存器初始化 */
    SCON = 0x50;  // 8位数据,可变波特率
    AUXR |= 0x40; // 定时器时钟1T模式
    AUXR &= 0xFE; // 串口1选择定时器1为波特率发生器
    TMOD &= 0x0F; // 设置定时器模式
    TL1 = 0xE0;   // 设置定时初始值，9600bps
    TH1 = 0xFE;   // 设置定时初始值，f=11.0592MHz
    ET1 = 0;      // 禁止定时器1中断
    ES = 1;       // 允许串口1中断
    TR1 = 1;      // 定时器1开始计时

    timer0_init(); // 定时器0初始化

    EA = 1; // 开全局中断

    // 初始显示0000
    UpdateTimeData();

    while (1)
    {
        if (task_5ms)
        {
            task_5ms = 0;
            if (DigitReady)
            {
                DisplayDigits(); // 每5ms刷新一次数码管
            }
        }

        if (task_20ms)
        {
            task_20ms = 0;
            // 按键检测（20ms检测一次）
            if (K1 == 0) // P1.2按下（低电平有效）
            {
                if(!key1_pressed) key1_pressed=1;   //基本防抖
            }
            else
            {
                if(key1_pressed){
                    key1_pressed=0;
                    timer_running = 1; // 开始计时
                }
            }
            if (K2 == 0) // P1.3按下
            {
                if(!key2_pressed) key2_pressed=1;
            }
            else
            {
                if(key2_pressed){
                    key2_pressed=0;
                    timer_running = 0; // 暂停计时
                }
            }
            if (K3 == 0) // P1.4按下
            {
                if(!key3_pressed) key3_pressed=1;
            }
            else
            {
                if(key3_pressed){
                    key3_pressed=0;
                    seconds = 0;          // 清零
                    timer_running = 0;    // 暂停计时
                    UpdateTimeData();     // 更新显示数据
                    StartUartSend();      // 发送清零后的时间
                }
            }
        }

        if (count_1s && timer_running)
        {
            count_1s = 0;
            seconds++;             // 秒数增加
            if (seconds > 9999)    // 防止溢出
                seconds = 0;
            UpdateTimeData();       // 更新显示数据
            StartUartSend();       // 发送当前时间
        }
    }
}

void Uart_Int(void) interrupt 4
{
    if (RI)
    {
        RI = 0; // 清除接收中断标志
    }

    if (TI)
    {
        TI = 0; // 清除发送中断标志
        if (TxBusy && TxIndex < TxLen)
        {
            SBUF = TxBuff[TxIndex++]; // 发送下一个数字
        }
        else
        {
            TxBusy = 0; // 发送完成
        }
    }
}