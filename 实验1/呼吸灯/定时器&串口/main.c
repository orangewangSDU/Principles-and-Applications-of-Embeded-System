#include <stc15.h>		//STC��Ƭ��ͷ�ļ�
#include <intrins.h>	//�ں�nopָ�������ʱ
#include <absacc.h>		//���Ե�ַ����

#include "dataType.h"	//#include <>��ֻ��ϵͳָ���ļ���������ͷ�ļ���ϵͳͷ�ļ�һ�����#include <>
						//#include ""�����ڵ�ǰ���������ļ���������ͷ�ļ������Ҳ����ٵ�ϵͳָ���ļ����������û��Զ���ͷ�ļ�һ����#include ""

//������ƶ���ͷ�ļ������У����������Ȼ������Ҽ���ѡ��"Open document <xxx.h>"�ɴ򿪸�ͷ�ļ�
//ͷ�ļ��򿪺󣬽�����ͷ�ƶ�����ͷ�ļ�ѡ���������Ҽ���ѡ��"Open Containing Folder"���ɶ�λͷ�ļ������ļ���
//ע�����׳������룬�����Edit��---����Configuration��---��General Editor Settings��Encoding---��Chinese GB2312 (Simplified)

#define Tx_Array_Size	32
#define Rx_Array_Size	32

u8 Tx_Buffer[Tx_Array_Size];
u8 Tx_Ptr,Tx_Tail,Rx_Ptr;

extern void timer0_init(void);	//20����@11.0592MHz


void main()//������
{

	P2M0 = 0xff;  //�������ģʽ���������
	P2M1 = 0x00;  //�������ģʽ���������
    timer0_init();
	EA=1;
	while(1);

}