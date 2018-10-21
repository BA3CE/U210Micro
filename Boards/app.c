/*
*********************************************************************************************************
*                                       GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stdio.h>

#include <string.h>
#include "stm32f10x_it.h"
#include "bsp.h"


#define WDEG_EN     1




/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/* ----------------- APPLICATION GLOBALS ------------------ */

/* -------- uC/OS-II APPLICATION TASK STACKS -------------- */
static  OS_STK         App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                      GLOBAL VARIABLES
*********************************************************************************************************
*/


/* ---------- uC/OS-II APPLICATION EVENTS ----------------- */


/*
*********************************************************************************************************
*                                      FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void            App_TaskStart               (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
#if (OS_TASK_NAME_SIZE > 5)
    CPU_INT08U  err;
#endif

    BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */
	
    OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_SIZE > 5)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
static  void  App_TaskStart (void *p_arg)
{
    (void)p_arg;                                                /* See Note #1                                              */
	


    BSP_Init();                                                 /* Initialize BSP functions                                 */
    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                              */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif
#if (WDEG_EN >0)
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	IWDG_SetPrescaler(IWDG_Prescaler_64);//64分频 一个周期1.6ms
 	IWDG_SetReload(800);//最长12位 [0,4096] 800*1.6=1.28S 
 	/* Reload IWDG counter */
 	IWDG_ReloadCounter();
 	IWDG_Enable();// Enable IWDG (the LSI oscillator will be enabled by hardware)

#endif
          
	//GPIO_SetBits(GPIOA, CS);
    
    while (DEF_TRUE){                                          /* Task body, always written as an infinite loop.            */       
        GPIO_SetBits(GPIOA, LED);
#if (WDEG_EN >0)        
		IWDG_ReloadCounter();
#endif 		
        OSTimeDlyHMSM(0, 0, 0, 500);
        GPIO_ResetBits(GPIOA, LED);
#if (WDEG_EN >0)        
		IWDG_ReloadCounter();
#endif 		
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}

