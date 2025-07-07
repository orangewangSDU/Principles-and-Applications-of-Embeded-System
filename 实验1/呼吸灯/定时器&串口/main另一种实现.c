#include<stc15.h>
unsigned char pwmDuty = 0;	// ռ�ձ�
unsigned char pwmCounter = 0;
unsigned char direction = 0; // �������ӻ��Ǽ���

void Timer0_Init(void)		//10΢��@11.0592MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x91;				//���ö�ʱ��ʼֵ
	TH0 = 0xFF;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

void Timer1_Init(void)		//10����@11.0592MHz
{
	AUXR &= 0xBF;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;			//���ö�ʱ��ģʽ
	TL1 = 0x00;				//���ö�ʱ��ʼֵ
	TH1 = 0xDC;				//���ö�ʱ��ʼֵ
	TF1 = 0;				//���TF1��־
	TR1 = 1;				//��ʱ��1��ʼ��ʱ
	ET1 = 1;
}


void Timer0_Isr(void) interrupt 1
{
	if(pwmCounter < pwmDuty)
		P2 = 0xff;
	else
		P2 = 0x00;
	pwmCounter++;
	pwmCounter %= 100;
}

void Timer1_Isr(void) interrupt 3
{
	if(direction == 0){
		pwmDuty++;
	}
	else
		pwmDuty--;
	if(pwmDuty >= 100){
		direction = 1;	// 1˵����ʼ����
	}
	if(pwmDuty == 0)
		direction = 0;	// 0˵����ʼ����
}

void main(){
	P2M0 = 0x00; P2M1 = 0x00; 

	P2 = 0x00;
	Timer0_Init();
	Timer1_Init();
	EA = 1;			// ��ȫ���ж�
	while(1){}
}