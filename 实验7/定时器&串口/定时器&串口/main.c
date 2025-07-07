#include "stc15.h"

/* ����ܶ��룬���������͵�ƽ���� */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                                0x92, 0x82, 0xF8, 0x80, 0x90};

/* ��ʾ������ */
unsigned char DigitBuffer[4] = {0, 0, 0, 0};
unsigned char DigitReady = 1;

unsigned char count_20ms = 0;
unsigned char task_20ms = 0;

unsigned char pos = 0;
unsigned int timer_set = 0;    // ���õĶ�ʱʱ�䣨�룩
unsigned int timer_count = 0;  // ��ǰ��ʱֵ���룩
unsigned char flash_count = 0; // ��˸����
unsigned char flash_state = 1; // ��˸״̬��1��ʾ��ʾ��

/* ����״̬ */
bit key1_pressed = 0;
bit key2_pressed = 0;
bit key3_pressed = 0;
bit timer_running = 0;         // ��ʱ�����б�־
bit timer_finished = 0;        // ��ʱ��ɱ�־

/* ��̬ɨ����ʾ */
void DisplayDigits()
{
    // �ر�����λѡ
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // �����ʱ����Ҵ�����˸�ر�״̬������ʾ
    if(timer_finished && !flash_state)
    {
        P0 = 0xFF;  // �ر����ж�
    }
    else
    {
        // ����״̬��ʾ����
        if(timer_finished)
        {
            // ��ʾ8888
            P0 = nums[8];
        }
        else
        {
            // ������ʾ��ǰ����
            P0 = nums[DigitBuffer[pos]];
        }
    }

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

/* ��ʱ��0�жϺ��� - ÿ5ms ���ڼ�ⰴ��״̬*/
void timer0_inter_handler(void) interrupt 1
{
    TL0 = 0x00;      
    TH0 = 0xEE; 
    
    count_20ms++;
    if (count_20ms >= 4) // 20ms����
    {
        count_20ms = 0;
        task_20ms = 1;
    }

    // ����ܶ�̬ˢ�£�5msɨ��һ�Σ�
    DisplayDigits();
}

/* ��ʱ��1��ʼ�� - 50ms */
void timer1_init(void)
{
    AUXR &= 0xBF;    // 12Tģʽ
    TMOD &= 0x0F;    // ���T1����λ
    TL1 = 0xB0;      // 50ms��ʱ
    TH1 = 0x3C;      
    TF1 = 0;
    TR1 = 0;         // ��ʼ������
    ET1 = 1;         // ����T1�ж�
}

/* ��ʱ��1�жϺ��� - 50ms */
void timer1_inter_handler(void) interrupt 3
{
    TL1 = 0xB0;      
    TH1 = 0x3C; 
    
    if(timer_running)
    {
        static unsigned char count_1s = 0;
        count_1s++;
        
        if(count_1s >= 20)  // 1�뵽��20��50ms����1s
        {
            count_1s = 0;
            if(timer_count > 0)
            {
                timer_count--;
                
                // ������ʾ
                DigitBuffer[0] = timer_count / 1000 % 10;
                DigitBuffer[1] = timer_count / 100 % 10;
                DigitBuffer[2] = timer_count / 10 % 10;
                DigitBuffer[3] = timer_count % 10;
            }
            else
            {
                timer_running = 0;
                timer_finished = 1;
                TR1 = 1;  // ��������������˸
            }
        }
    }
    else if(timer_finished)
    {
        flash_count++;
        if(flash_count >= 10)  // 0.5s�л�һ�Σ�10*50ms��
        {
            flash_count = 0;
            flash_state = !flash_state;
        }
    }
}

/* ������ʾ������ */
void UpdateDisplay(void)
{
    DigitBuffer[0] = timer_count / 1000 % 10;
    DigitBuffer[1] = timer_count / 100 % 10;
    DigitBuffer[2] = timer_count / 10 % 10;
    DigitBuffer[3] = timer_count % 10;
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

    timer0_init();  // ��ʱ��0��ʼ�� - 5ms
    timer1_init();  // ��ʱ��1��ʼ�� - 50ms
    EA = 1;         // �������ж�

    // ��ʼ��ʾ0000
    UpdateDisplay();

    while (1)
    {
        if (task_20ms)
        {
            task_20ms = 0;

            // ���K1�Ƿ��� (P1.2)
            if ((P1 & 0x04) == 0)
            {
                if (!key1_pressed)
                {
                    key1_pressed = 1;
                }
            }
            else // ����
            {
                if (key1_pressed)
                {
                    key1_pressed = 0;
                    if(!timer_running && !timer_finished)  // ֻ���ڷǼ�ʱ״̬���ܵ���ʱ��
                    {
                        timer_set++;
                        if(timer_set > 9999) timer_set = 9999; // ��ʱ������9999
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                }
            }

            // ���K2�Ƿ��� (P1.3)
            if ((P1 & 0x08) == 0)
            {
                if (!key2_pressed)
                {
                    key2_pressed = 1;
                }
            }
            else // ����
            {
                if (key2_pressed)
                {
                    key2_pressed = 0;
                    if(!timer_running && !timer_finished)  // ֻ���ڷǼ�ʱ״̬���ܵ���ʱ��
                    {
                        if(timer_set > 0) timer_set--;
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                }
            }

            // ���K3�Ƿ��� (P1.4)
            if ((P1 & 0x10) == 0)
            {
                if (!key3_pressed)
                {
                    key3_pressed = 1;
                }
            }
            else // ����
            {
                if (key3_pressed)
                {
                    key3_pressed = 0;
                    if(timer_finished)
                    {
                        // �����ʱ��ɣ��ٴΰ���K3���¿�ʼ
                        timer_finished = 0;
                        flash_state = 1;
                        timer_count = timer_set;
                        UpdateDisplay();
                    }
                    
                    if(timer_count > 0 && !timer_running)
                    {
                        timer_running = 1;
                        TR1 = 1;  // ������ʱ��1
                    }
                }
            }
        }
    }
}