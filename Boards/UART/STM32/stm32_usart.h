#ifndef __STM32_USART_H
#define __STM32_USART_H


extern void STM32_USART1_Init(void);
extern void STM32_USART3_Init(void);
extern void STM32_USART_Interrupt(USART_TypeDef* USARTx);
#endif