#include "usart.h"

#define USART_BRR(_fclk_,_baud_) 	((uint32_t)((uint32_t)_fclk_ * 0xF /  _baud_ / 16))
#define USART_BUFSIZE 256

volatile static uint8_t rxbuf[2][USART_BUFSIZE];
volatile static uint8_t *volatile rxhead[2], *volatile rxtail[2];

volatile static uint8_t txbuf[2][USART_BUFSIZE];
volatile static uint8_t *volatile txhead[2], *volatile txtail[2]; 

typedef struct
{
	USART_TypeDef *usart;
	DMA_Channel_TypeDef * dma;
} USART_Def;

USART_Def ports[] = {{USART1, DMA1_Channel5}, {USART2, DMA1_Channel6}};

void USART_IRQHandler (int port)
{
	uint8_t 	*dmahead;
	int 			delta_recv, free_buf;

	if (( ports[port].usart->SR & USART_SR_TXE) && (ports[port].usart->CR1 & USART_CR1_TXEIE))
	{
		if ( txhead[port] == txtail[port])
		{
			ports[port].usart->CR1 &= ~USART_CR1_TXEIE; // «апретим прерывани€ если нечего передавать
		}
		else
		{
			ports[port].usart->DR =  *(txtail[port]++);
			if (txtail[port] == (txbuf[port] + USART_BUFSIZE)) txtail[port] = txbuf[port];	
		}
	}

	dmahead =  rxbuf[port] + (USART_BUFSIZE - ports[port].dma->CNDTR);
	// ќпреде€л€ем количество прин€тых байт по DMA
	if ( rxhead[port] <= dmahead )
	{
		delta_recv = dmahead - rxhead[port];
	}
	else
	{
		delta_recv = USART_BUFSIZE - (rxhead[port] - dmahead);
	}
	
	if ( delta_recv > 0 )
	{
		// ќпредел€ем количество свободного места в буфере
		if ( rxhead[port] >= rxtail[port] )
		{
			free_buf = USART_BUFSIZE - (rxhead[port] - rxtail[port]) - 1;
		}
		else
		{
			free_buf =  rxtail[port] - rxhead[port] - 1;
		}
		// checking for free space
		if ( free_buf < delta_recv )
		{
			// ≈сли свободного места меньше чем было прин€то по DMA, 
			// то смещаем хвост чтобы он оставалс€ перед головой  и тер€ем данные в хвосте 
			rxtail[port] = (rxhead[port] + 1);
		}
		rxhead[port] = dmahead;
	}
}

void USART1_IRQHandler (void)
{
	USART_IRQHandler (0);
}

void USART2_IRQHandler (void)
{
	USART_IRQHandler (1);
}

void DMA1_Channel5_IRQHandler()
{
	DMA1->IFCR |= DMA_IFCR_CGIF5;
}

void DMA1_Channel6_IRQHandler()
{
	DMA1->IFCR |= DMA_IFCR_CGIF6;
}

void Init_DMA_UART1_Mem(void)
{
  if ((RCC->AHBENR & RCC_AHBENR_DMA1EN) != RCC_AHBENR_DMA1EN) RCC->AHBENR |=RCC_AHBENR_DMA1EN;

  DMA1_Channel5->CPAR  = (uint32_t)&USART1->DR;
  DMA1_Channel5->CMAR  = (uint32_t)&rxbuf[0][0];
  DMA1_Channel5->CNDTR = USART_BUFSIZE; 
  DMA1_Channel5->CCR   =   DMA_CCR1_MINC | DMA_CCR1_CIRC | DMA_CCR1_TCIE | DMA_CCR1_EN;
	
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	NVIC_SetPriority(DMA1_Channel5_IRQn, 11);
}

void Init_DMA_UART2_Mem(void)
{
  if ((RCC->AHBENR & RCC_AHBENR_DMA1EN) != RCC_AHBENR_DMA1EN) RCC->AHBENR |=RCC_AHBENR_DMA1EN;

  DMA1_Channel6->CPAR  = (uint32_t)&USART2->DR;
  DMA1_Channel6->CMAR  = (uint32_t)&rxbuf[1][0];
  DMA1_Channel6->CNDTR = USART_BUFSIZE; 
  DMA1_Channel6->CCR   =   DMA_CCR1_MINC | DMA_CCR1_CIRC | DMA_CCR1_TCIE | DMA_CCR1_EN;
	
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	NVIC_SetPriority(DMA1_Channel6_IRQn, 11);
}

void Usart1Init(void)
{
	uint32_t int_part, tmp, fract_part;

	rxhead[0] = rxtail[0] = rxbuf[0];
	txhead[0] = txtail[0] = txbuf[0]; 

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;    // enable clock for USART1
	RCC->APB2ENR|= RCC_APB2ENR_AFIOEN;   //Alternate Function I/O clock enable
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRH &= ~GPIO_CRH_MODE9;
  GPIOA->CRH &= ~GPIO_CRH_CNF9;
  GPIOA->CRH &= ~GPIO_CRH_MODE10;
  GPIOA->CRH &= ~GPIO_CRH_CNF10;

  GPIOA->CRH |= GPIO_CRH_MODE9_1;
  GPIOA->CRH |= GPIO_CRH_CNF9_1;

  GPIOA->CRH |= GPIO_CRH_CNF10_0;

	int_part = ((0x19 * SystemCoreClock * 2) / (0x04 * 115200));
	tmp = (int_part / 0x64) << 0x04;
	fract_part = int_part - (0x64 * (tmp >> 0x04));
	tmp |= ((((fract_part * 0x10) + 0x32) / 0x64)) & ((u8)0x0F);
	USART1->BRR = tmp;

  //USART Enable; RXNE Interrupt Enable; Transmitter Enable; Receiver Enable
	USART1->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	USART1->CR3 |= USART_CR3_DMAR;
	USART1->DR = 0x00;

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 12);

	Init_DMA_UART1_Mem();
}

void Usart2Init(void)
{
	uint32_t int_part, tmp, fract_part;

	rxhead[1] = rxtail[1] = rxbuf[1];
	txhead[1] = txtail[1] = txbuf[1]; 

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;    // enable clock for USART1
//	RCC->APB1ENR|= RCC_APB1ENR_AFIOEN;   //Alternate Function I/O clock enable
//	RCC->APB1ENR |= RCC_APB1ENR_IOPAEN;

	GPIOA->CRL &= ~GPIO_CRL_MODE2;
  GPIOA->CRL &= ~GPIO_CRL_CNF2;
  GPIOA->CRL &= ~GPIO_CRL_MODE3;
  GPIOA->CRL &= ~GPIO_CRL_CNF3;

  GPIOA->CRL |= GPIO_CRL_MODE2_1;
  GPIOA->CRL |= GPIO_CRL_CNF2_1;

  GPIOA->CRL |= GPIO_CRL_CNF3_0;

	int_part = ((0x19 * SystemCoreClock * 2) / (0x04 * 115200));
	tmp = (int_part / 0x64) << 0x04;
	fract_part = int_part - (0x64 * (tmp >> 0x04));
	tmp |= ((((fract_part * 0x10) + 0x32) / 0x64)) & ((u8)0x0F);
	USART2->BRR = tmp;
   
	//USART Enable; RXNE Interrupt Enable; Transmitter Enable; Receiver Enable
	USART2->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	USART2->CR3 |= USART_CR3_DMAR;
	USART2->DR = 0x00;

	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 12);

	Init_DMA_UART2_Mem();
}

int _putchar(int port, int c)
{
	int diff = txhead[port] - txtail[port];
	if (diff < 0) diff += USART_BUFSIZE;
	if (diff < USART_BUFSIZE -1)
	{
		*(txhead[port]++) = c;
		if(port == 0)
		{
			USART1->CR1 |= USART_CR1_TXEIE;
		}
		else
		{
			USART2->CR1 |= USART_CR1_TXEIE;
		}
		//++txhead[port];
		if (txhead[port] == (txbuf[port] + USART_BUFSIZE)) txhead[port] = txbuf[port];
	};
	return c;
}

int _rx_buffer(int port)
{
		if ( rxhead[port] == rxtail[port])
			return 0;
		
		if ( rxhead[port] > rxtail[port] )
		{
			return rxhead[port] - rxtail[port];
		}
		else
		{
			return USART_BUFSIZE - (rxtail[port] - rxhead[port]);
		}
}

int  _getchar(int port)
{
	int ret = -1;
	if ( rxhead[port] != rxtail[port])
	{
		ret =  *(rxtail[port]++);
		if (rxtail[port] == (rxbuf[port] + USART_BUFSIZE)) rxtail[port] = rxbuf[port];	
	}
	return ret;
}

int SendChar(int c)
{
	return _putchar(1, c);
}

int  GetKey(void)
{
	return _getchar(1);
}


void _putstr(char *str)
{
		while(*str)
		{
				SendChar(*str++);
		}
}
