#ifndef  __STM32CAN_H__
#define  __STM32CAN_H__

typedef struct{
	unsigned short BaudRate;
	unsigned char  IDE;  	
}CANCONF;

typedef struct{
  u32 Id;
  u8  Data[8];
  u8  DLC;
} CANBUFF;


#define CANPORT		4
#define CanMsgNum	128

extern void STM32_CAN_Init(void);
extern void CAN_Interrupt(void);
extern unsigned char STM32_CAN_Send(CANBUFF *msg);
extern unsigned char VerifyCanBaudRate(unsigned short BaudRate);
#endif
