#include "stc15.h"

/* ���ڷ��ͱ��� */
unsigned char TxTail;
unsigned char TxPtr;
unsigned char TxBuff[5];

/* ����hello�ַ��� */
unsigned char HelloBuf[5];     //���ջ�����
unsigned char HelloIndex = 0;  //��������


void main()
{

	/* LED-IO�Ĵ�����ʼ�� */
	P2M0 = 0xff;
	P2M1 = 0x00;

	/* ������ʼ�� */
	TxPtr=0;
	TxTail=0;
	
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
	
	EA=1;				//��ȫ���ж�
	
	while(1) {}
}



void Uart_Int(void) interrupt 4
{
	unsigned char RxData;

	if(RI) 
	{
		RI = 0;
		RxData = SBUF;
		// ����Ƿ�ƥ�� "hello"
		if ((RxData == "hello"[HelloIndex]) && HelloIndex < 5) //���αȽ�hello�е�ÿ���ַ��Ƿ��RxData�е��ַ����
		{
			HelloBuf[HelloIndex++] = RxData;
			if (HelloIndex == 5) 
			{
				// ƥ��ɹ���׼������ "world"
				TxBuff[0] = 'w';
				TxBuff[1] = 'o';
				TxBuff[2] = 'r';
				TxBuff[3] = 'l';
				TxBuff[4] = 'd';
				TxTail = 5;
				TxPtr = 0;
				SBUF = TxBuff[TxPtr];
				HelloIndex = 0; // ����׼����һ�ν���
			}
		} 
		else 
		{
			HelloIndex = 0; // ƥ��ʧ�ܣ����¿�ʼ
		}
	}
	if(TI) 
	{
		TI = 0;
		TxPtr ++ ;
		if(TxPtr < TxTail) 
		{
			SBUF = TxBuff[TxPtr];
		}
	}
}










