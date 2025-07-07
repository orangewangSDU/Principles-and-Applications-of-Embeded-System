#include <stc15.h>		//STC��Ƭ��ͷ�ļ�
#include "dataType.h"	//�Զ���

extern u8 Tx_Buffer[];
extern u8 Tx_Ptr,Tx_Tail,Rx_Ptr;

void uart_handler(void) interrupt 4//�����жϷ������
{
	u8 rx_data;

	if(RI) {
		RI = 0;
		rx_data = SBUF;
	}

	if(TI) {
		TI = 0;
		Tx_Ptr ++ ;
		if(Tx_Ptr <= Tx_Tail) {
			SBUF = Tx_Buffer[Tx_Ptr];
		}
	}
}
