#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


extern void usart_init(uint32_t baud);
extern void usart2_init(uint32_t baud);
extern void bt24_init(uint32_t baud);
extern void usart3_init(uint32_t baud);		//´®¿Ú3³õÊ¼»¯
extern void usart2_send_str(char *str);
extern void usart2_send_bytes(uint8_t *buf,uint32_t len);

#endif


