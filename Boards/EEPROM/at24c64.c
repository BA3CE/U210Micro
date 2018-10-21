/* Includes ------------------------------------------------------------------*/
#include <intrinsics.h>

#include "bsp.h"
/* Private value -----------------------------------------------------------*/
OS_EVENT *ReadWriteAT24C64Sem;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EEPROM_SLAVE_ADDRESS   0xA0

#define I2C_DELAY()	__no_operation();	\
					__no_operation();	\
					__no_operation(); 	\
					__no_operation();	\
                    __no_operation();	\
					__no_operation()

u8 ack;

void AT24C64_Init(void)
{
  	ReadWriteAT24C64Sem=OSSemCreate(1);
}

/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  
����:       ����I2C����,������I2C��ʼ����.
  
********************************************************************/
static void Start_I2c(void)
{
  	GPIO_SetBits(GPIOC, SDA);		/*������ʼ�����������ź�*/
 	I2C_DELAY();
  	GPIO_SetBits(GPIOC, SCL);
	I2C_DELAY();    				/*��ʼ��������ʱ�����4.7us,��ʱ*/
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY(); 
	GPIO_ResetBits(GPIOC, SDA); 	/*������ʼ�ź�*/ 
  	I2C_DELAY();		   			/* ��ʼ��������ʱ�����4��s*/
  	I2C_DELAY();
  	I2C_DELAY();
  	I2C_DELAY();
  	I2C_DELAY();      
  	GPIO_ResetBits(GPIOC, SCL);   	/*ǯסI2C���ߣ�׼�����ͻ�������� */
  	I2C_DELAY();
  	I2C_DELAY();
  	I2C_DELAY();
	I2C_DELAY();
  	I2C_DELAY();   
}
/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  
����:       ����I2C����,������I2C��������.
  
********************************************************************/
static void Stop_I2c(void)
{
	GPIO_ResetBits(GPIOC, SDA);   /*���ͽ��������������ź�*/
  	I2C_DELAY();				  /*���ͽ���������ʱ���ź�*/
  	GPIO_SetBits(GPIOC, SCL);;    /*������������ʱ�����4��s*/
  	I2C_DELAY();
  	I2C_DELAY();
  	I2C_DELAY();
	I2C_DELAY();
  	I2C_DELAY();
  	GPIO_SetBits(GPIOC, SDA);	 /*����I2C���߽����ź�*/
  	I2C_DELAY();
  	I2C_DELAY();
  	I2C_DELAY();
	I2C_DELAY();
  	I2C_DELAY();
}
/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: void  SendByte(uchar c);
����:  ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
     ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0 ��)     
     ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
static void SendByte(u8 c)
{
	u8 BitCnt;
 
 	for(BitCnt=0;BitCnt<8;BitCnt++){  /*Ҫ���͵����ݳ���Ϊ8λ*/
	    if((c<<BitCnt)&0x80)	 /*�жϷ���λ*/
			GPIO_SetBits(GPIOC, SDA);  
       	else  
		  	GPIO_ResetBits(GPIOC, SDA);                
     	I2C_DELAY();
     	GPIO_SetBits(GPIOC, SCL);               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
      	I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();         
     	GPIO_ResetBits(GPIOC, SCL); 
    }
   	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();  
    GPIO_SetBits(GPIOC, SDA);               /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
    I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();
	I2C_DELAY();   
    GPIO_SetBits(GPIOC, SCL);
    I2C_DELAY(); 
	
    if(GPIO_ReadInputDataBit(GPIOC, SDA)==1)
		ack=0;     
    else 
	  	ack=1;        /*�ж��Ƿ���յ�Ӧ���ź�*/
    GPIO_ResetBits(GPIOC, SCL); 
    I2C_DELAY(); 
    I2C_DELAY(); 
}
/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: uchar  RcvByte();
����:  �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
     ���������Ӧ������  
********************************************************************/	
static u8 RcvByte(void)
{
	u8 retc=0;
  	u8 BitCnt;
  
  	GPIO_SetBits(GPIOC, SDA);               /*��������Ϊ���뷽ʽ*/
  	for(BitCnt=0;BitCnt<8;BitCnt++){
		I2C_DELAY();
        GPIO_ResetBits(GPIOC, SCL);        /*��ʱ����Ϊ�ͣ�׼����������λ*/
        I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();
		I2C_DELAY();   
        GPIO_SetBits(GPIOC, SCL);        /*��ʱ����Ϊ��ʹ��������������Ч*/
        I2C_DELAY();
		I2C_DELAY(); 
        retc=retc<<1;
        if(GPIO_ReadInputDataBit(GPIOC, SDA)==1)
			retc=retc+1; /*������λ,���յ�����λ����retc�� */
        I2C_DELAY();
		I2C_DELAY(); 
    }
  	GPIO_ResetBits(GPIOC, SCL);  
  	I2C_DELAY();
	I2C_DELAY(); 
  	return(retc);
}

/********************************************************************
                     Ӧ���Ӻ���
ԭ��:  void Ack_I2c(bit a);
 
����:����������Ӧ���ź�,(������Ӧ����Ӧ���ź�)
********************************************************************/
static void Ack_I2c(u8 a)
{
  
	if(a==0)
		GPIO_ResetBits(GPIOC, SDA);     /*�ڴ˷���Ӧ����Ӧ���ź� */
    else 
	  	GPIO_SetBits(GPIOC, SDA);
  	I2C_DELAY();
	I2C_DELAY(); 
	I2C_DELAY(); 
  	GPIO_SetBits(GPIOC, SCL);
    I2C_DELAY();
	I2C_DELAY(); 
	I2C_DELAY(); 
	I2C_DELAY();
	I2C_DELAY(); 
 	GPIO_ResetBits(GPIOC, SCL);         /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
    I2C_DELAY(); 
	I2C_DELAY();  
}
u8 I2C_EE_BufferWrite(u8 *buff,u16 suba,u8 len)
{
	u8 i;
   	u8 suba_h;
   	u8 suba_l;
   	suba_h=suba>>8;
   	suba_l=suba;
   	Start_I2c();               /*��������*/
   	SendByte(EEPROM_SLAVE_ADDRESS);            /*����������ַ*/
    if(ack==0)
		return	0;
   	SendByte(suba_h);            /*���������ӵ�ַ*/
    if(ack==0)
		return	0;
   	SendByte(suba_l);            /*���������ӵ�ַ*/
    if(ack==0)
		return	0;  
	for(i=0;i<len;i++){
    	SendByte(*buff);               /*��������*/
       	if(ack==0)
			return 0;
     	buff++;
    } 
 	Stop_I2c();                 /*��������*/ 
  	return	1;
}
u8 I2C_EE_BufferRead(u8 *buff,unsigned suba,u8 len)
{
	u8 i;
   	u8 suba_h;
   	u8 suba_l;
   	suba_h=suba>>8;
   	suba_l=suba;
   	Start_I2c();                 	/*��������*/
   	SendByte(EEPROM_SLAVE_ADDRESS);	/*����������ַ*/
    if(ack==0)
		return 0;
   	SendByte(suba_h);            /*���������ӵ�ַ*/
    if(ack==0)
		return 0;
   	SendByte(suba_l);            /*���������ӵ�ַ*/
   	if(ack==0)
		return 0;  

   	Start_I2c();
   	SendByte(EEPROM_SLAVE_ADDRESS+1);
    if(ack==0)
		return	0;

   	for(i=0;i<len-1;i++){
    	*buff=RcvByte();               /*��������*/
      	Ack_I2c(0);                	/*���;ʹ�λ*/  
     	buff++;
    } 
   	*buff=RcvByte();
    Ack_I2c(1);                 	/*���ͷ�Ӧλ*/
 	Stop_I2c();                    	/*��������*/ 
  	return 1;
}
