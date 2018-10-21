/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "bsp.h"
#include "stm32f10x_lib.h"
#include "Usart_Inter.h"
#include "stm32/stm32_usart.h"

/* Extern variables ------------------------------------------------------------*/
extern unsigned char SendBuffer[1024];
extern OS_EVENT *WriteUart3Sem;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void USART3_Send(unsigned char *buff,unsigned short len)
{
	int TxCounter=0;
	
	OSSemPend(WriteUart3Sem,0,NULL);
	
	for(TxCounter=0;TxCounter<len;TxCounter++){
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	    /* Write one byte in the USART1 Transmit Data Register */
    	USART_SendData(USART3, buff[TxCounter]);
	}
	
	OSSemPost(WriteUart3Sem);
}
