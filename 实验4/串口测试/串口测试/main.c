#include "stc15.h"

/* �������ʾ������ */
unsigned char xdata nums[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};//0-9�������������

/* �洢���� */
unsigned char DigitBuffer[4]; // ���ڴ洢4λ���ֵ�ÿһλ
unsigned char DigitIndex = 0;
unsigned char DigitReady = 0; // �Ƿ�׼������ʾ����

unsigned char count_20ms,count20ms,task_20ms;

/* չʾλ */
unsigned char pos = 0;

void timer0_init(void)		//5����@11.0592MHz
{
	count_20ms = 0;

	AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x00;				//���ö�ʱ��ʼֵ
	TH0 = 0xEE;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ

	ET0 = 1;			//����ʱ���ж�
}

void timer0_inter_handler(void) interrupt 1
{
	count_20ms ++;
	count20ms=1;
	if(count_20ms>=25) 
	{
		count_20ms = 0;
	}
	task_20ms = 1;
}

//���չʾ����
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
    switch(pos)
    {
        case 0: P41 = 0; break; // ��1λ
        case 1: P42 = 0; break; // ��2λ
        case 2: P43 = 0; break; // ��3λ
        case 3: P44 = 0; break; // ��4λ
    }

    // ׼���´���ʾ��һλ
    pos++;
    if (pos >= 4) pos = 0;
}


void main()
{

	/* LED-IO�Ĵ�����ʼ�� */
	P2M0 = 0xff;
	P2M1 = 0x00;
	P4M0 = 0xff;  // ��P4������Ϊ�������
	P4M1 = 0x00;
	P0M0 = 0xff;  // ��P0������Ϊ�������
	P0M1 = 0x00;

	
	/* ���ڼĴ�����ʼ�� */
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�������������=�����/4
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xE0;			//���ö�ʱ��ʼֵ��9600bps
	TH1 = 0xFE;			//���ö�ʱ��ʼֵ��f=11.0592MHz
	ET1 = 0;			//��ֹ��ʱ��1�ж�
	ES = 1;				//������1�ж�
	TR1 = 1;			//��ʱ��1��ʼ��ʱ
	
	timer0_init();  //��ʱ��0��ʼ�������ڴ��ڳ�ʼ��֮��
	
	EA=1;				//��ȫ���ж�
	
	while (1) 
	{
		if (task_20ms) 
		{
			task_20ms = 0;
			if (DigitReady) 
			{
				DisplayDigits();  // ÿ20msˢ��һ�������
			}
		}
	}

}

void Uart_Int(void) interrupt 4
{
	unsigned char RxData;

	//�޸���������
	if (RI) 
	{
		RI = 0;
		RxData = SBUF;

		// �ж��Ƿ�Ϊ�����ַ� '0'~'9'
		if (RxData >= '0' && RxData <= '9') 
		{
			if (DigitIndex < 4)
			{
				DigitBuffer[DigitIndex] = RxData - '0'; // ת��Ϊ����
				DigitIndex++;
			}
			if (DigitIndex == 4) 
			{
				DigitReady = 1; // ��ʾ�����ѽ������
				DigitIndex = 0; // ׼���´ν���
			}
		} 
		else 
		{
			DigitIndex = 0; // ���������֣�������¿�ʼ
		}
	}
}










