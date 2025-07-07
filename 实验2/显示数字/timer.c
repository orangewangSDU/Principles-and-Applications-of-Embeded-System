#include <stc15.h>		//STC��Ƭ��ͷ�ļ�
#include "dataType.h"	//#include <>��ֻ��ϵͳָ���ļ���������ͷ�ļ���ϵͳͷ�ļ�һ�����#include <>

u8 count_20ms,count20ms,task_500ms,task_20ms;

void timer0_init(void)		//5����@11.0592MHz
{
	task_500ms = 0;
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
	if(count_20ms>=25) {
		count_20ms = 0;
		task_500ms = 1;
	}
	task_20ms = 1;

}