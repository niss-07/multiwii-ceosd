#include "main.h"

static __IO uint32_t 	TimingDelay;
static __IO uint32_t 	TickCounter;
static __IO uint32_t  TimerCounter;


void Delay(__IO uint32_t nTime)
{ 
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

uint32_t Ticks(uint32_t ticks)
{
	if(ticks)
		TickCounter = ticks;
	return TickCounter;
}

uint32_t Timer_ms(uint32_t ms)
{
	if(ms)
		TimerCounter = ms;
	return TimerCounter;
}

void SysTick_Handler(void)
{
  if (TimingDelay)
		TimingDelay--;

	if(TimerCounter)
		TimerCounter--;
	
	TickCounter++;
}
