/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Header for main.c module
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define LED_ON		GPIOB->BSRR = GPIO_Pin_7	// GPIO_BSRR_BS7
#define LED_OFF		GPIOB->BRR = GPIO_Pin_7		// GPIO_BRR_BR7
#define LED_TOGGLE	GPIOB->ODR ^= GPIO_Pin_7	// GPIO_ODR_ODR7



/* Exported functions ------------------------------------------------------- */

void Delay(__IO uint32_t nTime);
uint32_t Ticks(uint32_t ticks);
uint32_t Timer_ms(uint32_t ms);

void Button_Init(void);
void Led_Init(void);
void ADC_DMA_Init(void);

/* Screen functions */
void print_video_voltage(void);
void print_main_voltage(void);
void print_main_current(void);
void print_main_consumed(void);
void print_rssi(void);
void print_version(int version);
void print_status(void);
void print_gps(void);
void print_attitude(void);
void print_altitude(void);

extern __IO uint16_t VideoAvgValue;
extern __IO uint16_t RSSIAvgValue;
extern __IO uint16_t MainAvgValue;
extern __IO uint16_t CurrentAvgValue;

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
