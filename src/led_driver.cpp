
#include "led_driver.h"
#include <stm32f10x.h>

led_driver_stm32::led_driver_stm32()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_13;
	gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);
}

led_driver_stm32::~led_driver_stm32()
{
}

void led_driver_stm32::set()
{
	GPIOD->BSRR = GPIO_Pin_13;
}

void led_driver_stm32::reset()
{
	GPIOD->BRR = GPIO_Pin_13;
}


led_base::led_base()
	:m_mode(mode_t::invalid)
{
}
	
led_base::mode_t led_base::mode() const
{
	return m_mode;
}
	
void led_base::set_mode(mode_t new_mode)
{
	m_mode = new_mode;
}
