/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*                                 IAR STM32F103ZE-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include "bsp.h"
#include "stm32f10x_it.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_RCC_TO_VAL                  0x00000FFF             /* Max Timeout for RCC register                             */

/*
*********************************************************************************************************
*                                           EXTERNAL FUNCTIONS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void  BSP_GPIO_Init(void);
void  BSP_RCC_Init(void);
void  BSP_EXT_Init(void);
void  BSP_SPI_Init(void);
void  BSP_DMA_Init(void);
void  BSP_TIM2_Init(void);
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
**********************************************************************************************************
**********************************************************************************************************
**                                         GLOBAL FUNCTIONS
**********************************************************************************************************
**********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_Init()
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_Init (void)
{
    BSP_RCC_Init();                                             /* Initialize the Reset and Control (RCC) module             */
    BSP_DMA_Init();
    BSP_GPIO_Init();
    BSP_EXT_Init();
	BSP_TIM2_Init();
	BSP_SPI_Init();
}

/*
*********************************************************************************************************
*                                       BSP_CPU_ClkFreq()
*
* Description : This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    static  RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;


    freq = BSP_CPU_ClkFreq();
    return (freq);
}

void BSP_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = LED|RX_STCP|RX_SHCP|RX_DS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MOSI|SCK|nCS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Pin = TX_STCP|TX_SHCP|TX_DS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*
*********************************************************************************************************
*                                         BSP_RCC_Init()
*
* Description : Initializes the RCC module. Set the FLASH memmory timing and the system clock dividers
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_RCC_Init(void)
{
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
    /* Enable peripheral clocks --------------------------------------------------*/
    /*AFIO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
    /*GPIOA clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /*GPIOC clock enable */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* Enable ADC1 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    /* Enable DAC clocks */
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /*TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  }
}
void BSP_SPI_Init(void)
{
    /* SPI1 Config -------------------------------------------------------------*/
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
    
    SPI_Cmd(SPI1, ENABLE); 
    
    //BSP_IntVectSet(BSP_INT_ID_SPI1, SPI1_IRQHandler);
  	//BSP_IntEn(BSP_INT_ID_SPI1);
}
void BSP_EXT_Init(void)
{
  	EXTI_InitTypeDef EXTI_InitStructure;

  //设置io口  
  /* Connect EXTI Line to W5100 GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);  //PA4 为EXTI nCS

  /* Configure EXTI Line to generate an interrupt on rising edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(BSP_INT_ID_EXTI4);
	
	BSP_IntVectSet(BSP_INT_ID_EXTI4, EXTI4_IRQHandler);
  	BSP_IntEn(BSP_INT_ID_EXTI4);
}

#define SPI1_DR_Address  (&(SPI1->DR)) 

vu8 SPI1_Buffer_Rx[BufferSize];

void BSP_DMA_Init(void)
{
    DMA_InitTypeDef  DMA_InitStructure;
  
    /* DMA1 channel2 SPI1 RX configuration ---------------------------------------------*/
    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_Buffer_Rx;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    

    DMA_ClearFlag(DMA1_FLAG_TC2);
     /* Disable DMA1 Channel4 */
    DMA_Cmd(DMA1_Channel2, ENABLE);
    

}

void BSP_TIM2_Init(void)
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//((1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+35999)/72M)*(1+9)=5毫秒
	
	
	TIM_TimeBaseStructure.TIM_Period = 9; //设置自动装载寄存器       
	TIM_TimeBaseStructure.TIM_Prescaler = 35999; //分频计数    
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //选择向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_Cmd(TIM2, DISABLE);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//设置中断向量
  	BSP_IntVectSet(BSP_INT_ID_TIM2, TIM2_IRQHandler);
  	BSP_IntEn(BSP_INT_ID_TIM2);	

}


