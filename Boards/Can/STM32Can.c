/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "bsp.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_it.h"

#include "can/stm32can.h"

/* Extern variables ------------------------------------------------------------*/
extern OS_EVENT	*CanMsgQ;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//static CANCONF	CanConf;
static CANBUFF  CanBuffer[CanMsgNum];
static INT8U CanMsgCount=0;
/* Private functions ---------------------------------------------------------*/


void STM32_CAN_Init(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
  	/* CAN register init */
  	CAN_DeInit();

  	/* CAN cell init */
  	CAN_InitStructure.CAN_TTCM=DISABLE;
  	CAN_InitStructure.CAN_ABOM=DISABLE;
  	CAN_InitStructure.CAN_AWUM=DISABLE;
  	CAN_InitStructure.CAN_NART=DISABLE;
  	CAN_InitStructure.CAN_RFLM=DISABLE;
  	CAN_InitStructure.CAN_TXFP=DISABLE;
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;
/*
  can主频=pclk1/CAN_Prescaler=36M/225 = 160K
  can主频=pclk1/CAN_Prescaler=8M/50 = 160K
  baund = can主频 / (sjw+bs1+bs2) = 160k /16 = 10k
  24M - >10k , 1,7,8,150
*/  
	//get_canbus(&CanConf);
	
	//CanConf.IDE=CAN_ID_EXT;
	

	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;    
  	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;    
  	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	//CAN_InitStructure.CAN_Prescaler=225;	//10K
    CAN_InitStructure.CAN_Prescaler=50;	//10K
	
  	CAN_Init(&CAN_InitStructure);

  	/* CAN filter init */
  	CAN_FilterInitStructure.CAN_FilterNumber=0;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* CAN FIFO0 message pending interrupt enable */ 
  	CAN_ITConfig(CAN_IT_FMP0, ENABLE);
	
	//设置中断向量
  	BSP_IntVectSet(BSP_INT_ID_USB_LP_CAN_RX0, USB_LP_CAN_RX0_IRQHandler);
  	BSP_IntEn(BSP_INT_ID_USB_LP_CAN_RX0);

}

INT8U STM32_CAN_Send(CANBUFF *msg)
{
  	CanTxMsg TxMessage;
  	INT8U TransmitMailbox = 0;
  	/* transmit */

  	TxMessage.ExtId=msg->Id;
	
  	TxMessage.RTR=CAN_RTR_DATA;
  	TxMessage.IDE=CAN_ID_EXT;
	
	if(msg->DLC>8)
		TxMessage.DLC=8;
	else 
	  	TxMessage.DLC=msg->DLC;
	
	memcpy(TxMessage.Data,msg->Data,TxMessage.DLC);

  	TransmitMailbox=CAN_Transmit(&TxMessage);
	
	return CAN_TransmitStatus(TransmitMailbox);
}

void CAN_Interrupt(void)
{
  	CanRxMsg RxMessage;
#if 0	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	    
#endif
	//OS_ENTER_CRITICAL();		
  	CAN_Receive(CAN_FIFO0, &RxMessage);
	//OS_EXIT_CRITICAL();
	if(RxMessage.IDE==CAN_ID_STD)
		CanBuffer[CanMsgCount].Id=RxMessage.StdId;
	else
	  	CanBuffer[CanMsgCount].Id=RxMessage.ExtId;
	
	CanBuffer[CanMsgCount].DLC=RxMessage.DLC;
	memcpy(CanBuffer[CanMsgCount].Data,RxMessage.Data,RxMessage.DLC);
	
	OSQPost(CanMsgQ,&CanBuffer[CanMsgCount]);
	
	CanMsgCount++;
	if(CanMsgCount>=CanMsgNum)
		CanMsgCount=0;  	
}
unsigned char VerifyCanBaudRate(unsigned short BaudRate)
{
	switch(BaudRate){
		case 5:
		case 10:
		case 20:
		case 50:
		  	return 1;
	default:
		  	return 0;
	}
}