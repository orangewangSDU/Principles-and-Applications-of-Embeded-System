#include <stc15.h>		//STC单片机头文件
#include <intrins.h>	//内含nop指令，用于延时
#include <absacc.h>		//绝对地址访问

#include "dataType.h"	//#include <>，只在系统指定文件夹搜索该头文件，系统头文件一般采用#include <>
						//#include ""，先在当前工程所在文件夹搜索该头文件，若找不到再到系统指定文件夹搜索。用户自定义头文件一般用#include ""

//将鼠标移动到头文件所在行，点下左键，然后鼠标右键，选择"Open document <xxx.h>"可打开该头文件
//头文件打开后，将鼠标箭头移动至该头文件选项卡，点鼠标右键，选择"Open Containing Folder"，可定位头文件所在文件夹
//注释容易出现乱码，点击“Edit”---》“Configuration”---》General Editor Settings：Encoding---》Chinese GB2312 (Simplified)

#define Tx_Array_Size	32
#define Rx_Array_Size	32

u8 Tx_Buffer[Tx_Array_Size];
u8 Tx_Ptr,Tx_Tail,Rx_Ptr;

extern void timer0_init(void);	//20毫秒@11.0592MHz


void main()//主程序
{

	P2M0 = 0xff;  //控制输出模式，软件生成
	P2M1 = 0x00;  //控制输出模式，软件生成
    timer0_init();
	EA=1;
	while(1);

}