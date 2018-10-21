#ifndef _USART_INTER_H
#define _USART_INTER_H

enum COMPORT{
	COM1,
	COM2,
	COM3,
};

enum CMD{
	ALARMSET,
	ALARM,
	LINEBAD,
	ALARMPASS,
	UNALARM,
};

typedef struct{
	u32 Id;
	u8  Data[8];
	u8  DLC;
}SERIAL_PORT_BUFF;

//#define UartMsgNum		128

extern void USART3_Send(unsigned char *buff,unsigned short len);

#endif