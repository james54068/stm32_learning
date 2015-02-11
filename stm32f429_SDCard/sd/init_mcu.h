#ifndef __INIT_MCU_H
#define __INIT_MCU_H

#include "stm32f4xx.h"

void GPIO_Configuration(void);
void USART1_Configuration(void);
void send_byte(uint8_t b);
int _write (int fd, char *ptr, int len);
void USART1_puts(char* s);




#endif