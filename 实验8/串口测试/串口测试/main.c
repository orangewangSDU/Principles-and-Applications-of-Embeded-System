#include "stc15.h"

/* �������ʾ������ */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // 0-9�������������

/* �洢���� */
unsigned char DigitBuffer[4]; // ���ڴ洢4λ���ֵ�ÿһλ
unsigned char DigitReady = 0; // �Ƿ�׼������ʾ����

/* ��ʱ��� */
unsigned char count_5ms = 0;
unsigned char count_20ms = 0;
bit task_5ms = 0;
bit task_20ms = 0;
unsigned char count_1s = 0;     // ����1�����
unsigned char timer_running = 0; // ��ʱ������״̬
unsigned int seconds = 0;       // ��������0-9999��

/* ���ڷ������ */
unsigned char TxBuff[4];        // ֻ���4λ���ֵ�ASCII��
unsigned char TxIndex = 0;
unsigned char TxLen = 4;
unsigned char TxBusy = 0;    //��ֹ�ظ�����

/* չʾλ */
unsigned char pos = 0;

// �������壨�޸�ΪP1.2-P1.4��
#define K1 (P1 & 0x04)  // P1.2
#define K2 (P1 & 0x08)  // P1.3 
#define K3 (P1 & 0x10)  // P1.4

//����״̬
bit key1_pressed = 0;
bit key2_pressed = 0;
bit key3_pressed = 0;

void timer0_init(void) // 5����@11.0592MHz
{
    AUXR &= 0x7F; // ��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0; // ���ö�ʱ��ģʽ
    TL0 = 0x00;   // ���ö�ʱ��ʼֵ
    TH0 = 0xEE;   // ���ö�ʱ��ʼֵ
    TF0 = 0;      // ���TF0��־
    TR0 = 1;      // ��ʱ��0��ʼ��ʱ
    ET0 = 1;      // ����ʱ���ж�
}

void timer0_inter_handler(void) interrupt 1
{
    /* 5ms��ʱ ����ˢ�������*/
    task_5ms = 1;
    count_5ms++;
    
    /* 20ms��ʱ ���ڼ�ⰴ��*/
    if(++count_20ms >= 4) // 4*5ms=20ms
    {
        count_20ms = 0;
        task_20ms = 1;
    }
    
    /* 1s��ʱ ���ڷ������ݣ�ֻ�����ڼ�ʱ��ʱ��ſ�ʼ��1s*/
    if(count_5ms >= 200 && timer_running) // 200*5ms=1s
    {
        count_5ms = 0;
        count_1s = 1;
    }
}

void DisplayDigits()
{
    // ��ѡ - �ͳ����������루�͵�ƽ����
    P0 = nums[DigitBuffer[pos]];

    // �������λѡ��Ϩ����������ܣ�
    P41 = 1;
    P42 = 1;
    P43 = 1;
    P44 = 1;

    // λѡ���ĸ��������ʾ��
    switch (pos)
    {
    case 0: P41 = 0; break; // ��1λ
    case 1: P42 = 0; break; // ��2λ
    case 2: P43 = 0; break; // ��3λ
    case 3: P44 = 0; break; // ��4λ
    }

    // ׼���´���ʾ��һλ
    pos++;
    if (pos >= 4)
        pos = 0;
}

// �����������ʾ�ʹ��ڷ��ͻ�����
void UpdateTimeData()
{
    // �����������ʾ����
    DigitBuffer[0] = seconds / 1000 % 10; // ǧλ
    DigitBuffer[1] = seconds / 100 % 10;  // ��λ
    DigitBuffer[2] = seconds / 10 % 10;   // ʮλ
    DigitBuffer[3] = seconds % 10;        // ��λ
    DigitReady = 1;

    // ���´��ڷ��ͻ���������4λ���֣�
    TxBuff[0] = DigitBuffer[0] + '0'; // ǧλ
    TxBuff[1] = DigitBuffer[1] + '0'; // ��λ
    TxBuff[2] = DigitBuffer[2] + '0'; // ʮλ
    TxBuff[3] = DigitBuffer[3] + '0'; // ��λ
}

// ��ʼ���ڷ��ͣ�����4λ���֣�
void StartUartSend()
{
    if (!TxBusy)
    {
        TxIndex = 0;
        TxBusy = 1;
        SBUF = TxBuff[TxIndex++]; // ���͵�һ������
    }
}

void main()
{
    /* IO�Ĵ�����ʼ�� */
    P2M0 = 0xff;
    P2M1 = 0x00;
    P4M0 = 0xff; // ��P4������Ϊ�������
    P4M1 = 0x00;
    P0M0 = 0xff; // ��P0������Ϊ�������
    P0M1 = 0x00;
    
    // ����P1.2-P1.4Ϊ����ģʽ��׼˫��ڣ�
    P1M0 &= ~0x1C; // P1.2-P1.4
    P1M1 &= ~0x1C;

    /* ���ڼĴ�����ʼ�� */
    SCON = 0x50;  // 8λ����,�ɱ䲨����
    AUXR |= 0x40; // ��ʱ��ʱ��1Tģʽ
    AUXR &= 0xFE; // ����1ѡ��ʱ��1Ϊ�����ʷ�����
    TMOD &= 0x0F; // ���ö�ʱ��ģʽ
    TL1 = 0xE0;   // ���ö�ʱ��ʼֵ��9600bps
    TH1 = 0xFE;   // ���ö�ʱ��ʼֵ��f=11.0592MHz
    ET1 = 0;      // ��ֹ��ʱ��1�ж�
    ES = 1;       // ������1�ж�
    TR1 = 1;      // ��ʱ��1��ʼ��ʱ

    timer0_init(); // ��ʱ��0��ʼ��

    EA = 1; // ��ȫ���ж�

    // ��ʼ��ʾ0000
    UpdateTimeData();

    while (1)
    {
        if (task_5ms)
        {
            task_5ms = 0;
            if (DigitReady)
            {
                DisplayDigits(); // ÿ5msˢ��һ�������
            }
        }

        if (task_20ms)
        {
            task_20ms = 0;
            // ������⣨20ms���һ�Σ�
            if (K1 == 0) // P1.2���£��͵�ƽ��Ч��
            {
                if(!key1_pressed) key1_pressed=1;   //��������
            }
            else
            {
                if(key1_pressed){
                    key1_pressed=0;
                    timer_running = 1; // ��ʼ��ʱ
                }
            }
            if (K2 == 0) // P1.3����
            {
                if(!key2_pressed) key2_pressed=1;
            }
            else
            {
                if(key2_pressed){
                    key2_pressed=0;
                    timer_running = 0; // ��ͣ��ʱ
                }
            }
            if (K3 == 0) // P1.4����
            {
                if(!key3_pressed) key3_pressed=1;
            }
            else
            {
                if(key3_pressed){
                    key3_pressed=0;
                    seconds = 0;          // ����
                    timer_running = 0;    // ��ͣ��ʱ
                    UpdateTimeData();     // ������ʾ����
                    StartUartSend();      // ����������ʱ��
                }
            }
        }

        if (count_1s && timer_running)
        {
            count_1s = 0;
            seconds++;             // ��������
            if (seconds > 9999)    // ��ֹ���
                seconds = 0;
            UpdateTimeData();       // ������ʾ����
            StartUartSend();       // ���͵�ǰʱ��
        }
    }
}

void Uart_Int(void) interrupt 4
{
    if (RI)
    {
        RI = 0; // ��������жϱ�־
    }

    if (TI)
    {
        TI = 0; // ��������жϱ�־
        if (TxBusy && TxIndex < TxLen)
        {
            SBUF = TxBuff[TxIndex++]; // ������һ������
        }
        else
        {
            TxBusy = 0; // �������
        }
    }
}