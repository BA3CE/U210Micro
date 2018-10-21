/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "stm32f10x_it.h"
#include "stm32f10x_lib.h"
#include "uart/usart_Inter.h"

/* Extern variables ------------------------------------------------------------*/
extern  OS_EVENT  *UartMsgQ;
OS_EVENT *WriteUart3Sem;
/* Private define ------------------------------------------------------------*/
/* Private function-----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*
*********************************************************************************************************
*                                              STM32_USART_Init()
*
* Description : Initialize the board's Uart.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*
* Note(s)     : David Wang.
*********************************************************************************************************
*/

void STM32_USART3_Init(void)
{
  	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  	/* Configure USART3 */
  	USART_Init(USART3, &USART_InitStructure);
	
  	/* Enable the USART Receive interrupt: this interrupt is generated when the 
       USART3 receive data register is not empty */
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART3, USART3_IRQHandler);
  	BSP_IntEn(BSP_INT_ID_USART3);
	
	USART_Cmd(USART3, ENABLE);
	
	WriteUart3Sem=OSSemCreate(1);
}

void STM32_USART_Interrupt(USART_TypeDef* USARTx)
{
	static unsigned char rxChar;
	
  	if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET){
	  	/* Read one byte from the receive data register */
	  	rxChar=USART_ReceiveData(USARTx);
		OSQPost(UartMsgQ,(void *)&rxChar);
	}  
}