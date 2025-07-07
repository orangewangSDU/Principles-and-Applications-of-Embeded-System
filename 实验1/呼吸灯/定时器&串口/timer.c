#include <stc15.h>        //STC��Ƭ��ͷ�ļ�
#include "dataType.h"    //#include <>��ֻ��ϵͳָ���ļ���������ͷ�ļ���ϵͳͷ�ļ�һ�����#include <>

int flag=0;
int time=0,cnt=0,dir=0;

void timer0_init(void)        //100us @11.0592MHz
{
    AUXR |= 0x80;            //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;            //���ö�ʱ��ģʽ
    TL0 = 0xAE;                //���ö�ʱ��ʼֵ
    TH0 = 0xFB;                //���ö�ʱ��ʼֵ
    TF0 = 0;                //���TF0��־
    TR0 = 1;                //��ʱ��0��ʼ��ʱ

    ET0 = 1;                //����ʱ���ж�
}


void timer0_inter_handler(void) interrupt 1   //�жϴ���
{
    cnt++;
    cnt%=100;
    
    if(time>=99)
    {
        time=0;
        cnt=0;
        dir=!dir;
    }
    
    if(dir==0)
    {
        if(cnt>time)    
            P2=0xff;
        if(cnt<time)
            P2=0;
        if(cnt>=99)
            time++;
    }
    
    if(dir==1)
    {
        if(cnt>time)
            P2=0;
        if(cnt<time)
            P2=0xff;
        if(cnt>=99)
            time++;
    }
}