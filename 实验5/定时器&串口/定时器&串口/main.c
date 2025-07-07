#include <stc15.h>
#include "dataType.h"

/* 数码管段码，共阳极 */
unsigned char code SEG_TAB[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

unsigned char DigitBuffer[4];
unsigned char pos = 0;

unsigned int adcAvg = 0;
unsigned int adcSum = 0;
unsigned int mv;
unsigned char adcCount = 0;

bit adc_ready = 0;

extern unsigned int adcRes[2];  // 从 adc 模块获取 adcRes[1]（P1.1）
extern void adc_init();


// ================= 显示函数 =================
void DisplayDigits()
{
	// 关闭所有位选
    P41 = 1; 
	P42 = 1; 
	P43 = 1; 
	P44 = 1;  

    P0 = SEG_TAB[DigitBuffer[pos]];     // 段码输出

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

// ================ 定时器初始化 ================
void timer0_init()   //5毫秒@11.0592MHz

{
    AUXR &= 0x7F;    // 12T模式
    TMOD &= 0xF0;    // 清除T0控制位
    TL0 = 0x00;
    TH0 = 0xEE;
    TF0 = 0;
    TR0 = 1;         // 启动T0
    ET0 = 1;         // 允许T0中断
}

// ================ 定时器中断（5ms） ================
void timer0_inter_handler(void) interrupt 1
{
    static unsigned char tick = 0;

    tick++;
    if (tick >= 13) // ≈ 65ms ≈ 每秒16次
    {
        tick = 0;

        // 每65ms一次采样（16次后处理）
        adcSum += adcRes[1];  // 通道1 -> P1.1
        adcCount++;

        if (adcCount >= 16)
        {
            adcAvg = adcSum / 16;
            adcSum = 0;
            adcCount = 0;
            adc_ready = 1;
        }
    }

    DisplayDigits();  // 数码管刷新
}

void main()
{
    P2M0 = 0xFF; P2M1 = 0x00;
    P4M0 = 0xFF; P4M1 = 0x00;
    P0M0 = 0xFF; P0M1 = 0x00;

    P41 = 0;
	P42 = 0;
	P43 = 0;
	P44 = 0;

    adc_init();     // 初始化ADC模块
    timer0_init();  // 初始化定时器

    EA = 1;         // 开启总中断

    while (1)
    {
        if (adc_ready)
        {
            adc_ready = 0;

            // 假设参考电压为5V，ADC最大1023
            mv = (adcAvg * 5000UL) / 1023;

            DigitBuffer[0] = mv / 1000 % 10;
            DigitBuffer[1] = mv / 100 % 10;
            DigitBuffer[2] = mv / 10 % 10;
            DigitBuffer[3] = mv % 10;
        }
    }
}
