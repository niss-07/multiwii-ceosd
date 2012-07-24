#ifndef __usart_h
#define __usart_h

#include <stm32f10x.h>

void Usart1Init(void);
void Usart2Init(void);
int SendChar(int c);
int  GetKey(void);

int _putchar(int port, int c);
int  _getchar(int port);
void _putstr(char *str);
int _rx_buffer(int port);

#endif /* __usart_h */
