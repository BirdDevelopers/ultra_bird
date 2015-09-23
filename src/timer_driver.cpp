
#include "timer_driver.h"
#include <stm32f10x.h>

//timer_driver_stm32 * timer_driver_stm32::timers_table[12];// = {0, 0, 0, 0, 0, 0, 0, 0};
timer_driver_stm32 * timer_driver_stm32::pTimer2 = nullptr;

timer_driver_stm32::timer_driver_stm32()
{
	//timer_driver_stm32::timers_table[2] = this;
	pTimer2 = this;
	
	unsigned period_ms = 50000;
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef timerInitStructure;
	TIM_TimeBaseStructInit(&timerInitStructure);
	
	timerInitStructure.TIM_Prescaler = 127 - 1;//RCC_Clocks.HCLK_Frequency;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 2000 - 1;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	
	/*
*/
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
}

timer_driver_stm32::~timer_driver_stm32()
{
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);
	
	//timers_table[2] = nullptr;
	pTimer2 = nullptr;
}
/*
void timer_driver_stm32::event()
{
}*/

extern "C" void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		
		//timer_driver_stm32::timers_table[2]->event();
		timer_driver_stm32::pTimer2->event();
		//GPIOD->BRR = GPIO_Pin_13;
		//GPIOD->BSRR = GPIO_Pin_13;
	}
}
