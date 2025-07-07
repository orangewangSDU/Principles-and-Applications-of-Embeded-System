#include "stc15.h"

/* ����ܶ��룬���������͵�ƽ���� */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                                0x92, 0x82, 0xF8, 0x80, 0x90};

/* ��ʾ������ */
unsigned char DigitBuffer[4];
unsigned char DigitReady = 1;

unsigned char count_20ms = 0;
unsigned char task_20ms = 0;

unsigned char pos = 0;
unsigned char now = 0;

/* �������� */
bit key4_pressed = 0;

/* ��ʱ��0��ʼ�� - 5ms */
void timer0_init(void)
{
    AUXR &= 0x7F;    // 12Tģʽ
    TMOD &= 0xF0;    // ���T0����λ
    TL0 = 0x00;      
    TH0 = 0xEE;      
    TF0 = 0;
    TR0 = 1;         // ����T0
    ET0 = 1;         // ����T0�ж�
}

/* ��̬ɨ����ʾ */
void DisplayDigits()
{
    // �ر�����λѡ
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // ��ѡ
    P0 = nums[DigitBuffer[pos]];

    // λѡ
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

/* ��ʱ��0�жϺ��� - ÿ5ms */
void timer0_inter_handler(void) interrupt 1
{
    count_20ms++;
    if (count_20ms >= 4) // �ܹ���20ms����
    {
        count_20ms = 0;
        task_20ms = 1;
    }

    // ����ܶ�̬ˢ�£�5msɨ��һ�Σ�
    if (DigitReady)
    {
        DisplayDigits();
    }
}

/* ������ */
void main()
{
    /* IO��ʼ�� */
    P2M0 = 0xff; P2M1 = 0x00;
    P4M0 = 0xff; P4M1 = 0x00;
    P0M0 = 0xff; P0M1 = 0x00;

    /* ����ܿ���IOĬ��ȫ�� */
    P41 = 0; 
	P42 = 0; 
	P43 = 0; 
	P44 = 0;

    timer0_init();  // ��ʱ����ʼ��
    EA = 1;         // �������ж�

    while (1)
    {
        if (task_20ms)
        {
            task_20ms = 0;

            // ���K4�Ƿ���
            if ((P1 & 0x20) == 0) // P1.5ΪK4���͵�ƽΪ����
            {
                if (!key4_pressed)
                {
                    key4_pressed = 1;
                }
            }
            else // ����
            {
                if (key4_pressed)
                {
                    key4_pressed = 0;
                    now++;
                    if (now > 15) now = 0;
                }
            }

            // �����������ʾ
            DigitBuffer[0] = now / 1000 % 10;
            DigitBuffer[1] = now / 100 % 10;
            DigitBuffer[2] = now / 10 % 10;
            DigitBuffer[3] = now % 10;
        }
    }
}
