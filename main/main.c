#include "main.h"
#include "SPI_MAX7456.h"
#include "USART.h"
#include "multiwii21.h"

#include <stdio.h>

uint8_t wait_responce = 0;

int main(void)
{

	SystemInit();
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock / 500)) // 1 ms
	{ 
		/* Capture error */ 
		while (1);
	}

	Led_Init();
	Button_Init();
	Usart1Init();
	Usart2Init();
	ADC_DMA_Init();

	Delay(1000);
	MAX7456_SYS_Init();
	Ticks(1);
	while (1)
	{
		if(!wait_responce || !Timer_ms(0))
		{
			wait_responce = request();
			Timer_ms(100);
		}
		if(parse_responce())
		{
			wait_responce--;
		}
		while((MAX7456_Read_Reg(STAT) & 0x10) != 0x00);	// wait for vsync
		MAX7456_ClearScreen();
		
		print_video_voltage();
		print_main_voltage();
		print_main_current();
		print_main_consumed();
		print_rssi();
		print_altitude();
		print_attitude();
		print_gps();
		MAX7456_WriteScreen();
		Ticks(1);
	}
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
