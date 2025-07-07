#include <stc15.h>
#include "dataType.h"


								//ADC�Ĵ���
#define ADC_POWER		0x80
#define ADC_SPEED(x)	(x<<5)	//ADת���ٶȣ�0-3
#define ADC_START		0x08	//����ADת��
#define ADC_FLAG		0x10	//ת��������־

u8	adc_ch;				//��ǰת����ADCͨ��
u16 adcRes[2];			//ת�����

void adc_init()
{
	adc_ch = 0;
	adcRes[0] = 0;
	adcRes[1] = 0;
	
	P1ASF = 0x03;	//P1.0��P1.1��ΪA/Dʹ��
	ADC_RES = 0;	//ת�������8λ����λֵΪ0���ɲ�����
	ADC_RESL = 0;	//ת�������8λ����λֵΪ0���ɲ�����
	ADC_CONTR = ADC_POWER|ADC_SPEED(0)|ADC_START|adc_ch;
	EADC = 1;
}

void adc_handle() interrupt 5
{
	ADC_CONTR &= ~ADC_FLAG;	//��ADC_FLAGλ����
	adcRes[adc_ch] = ADC_RES;//ת�������8λ
	adcRes[adc_ch] <<= 2;
	adcRes[adc_ch] |= ADC_RESL&0x03;
	adc_ch ++ ;
	if(adc_ch>=2) {
		adc_ch = 0;
	}
	ADC_CONTR = ADC_POWER|ADC_SPEED(1)|ADC_START|adc_ch;
}