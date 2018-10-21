/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_AT24C64_H    
#define __STM32_AT24C64_H  

void AT24C64_Init(void);
unsigned char I2C_EE_BufferWrite(unsigned char *buff,unsigned short suba,unsigned char len);
unsigned char I2C_EE_BufferRead(unsigned char *buff,unsigned short suba,unsigned char len);
#endif