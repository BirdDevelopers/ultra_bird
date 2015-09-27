
#include "led_control_task.h"
#include "../led_driver.h"
/*
int uav_rt_main()
{
	using led_t = led_driver<led_driver_stm32>;
	led_t l1;
	
	led_control_task<led_t> green_led_task(&l1);
	green_led_task.start();
	
	l1.set_mode(led_base::mode_t::blink);
	
	vTaskStartScheduler();
	return 0;
}*/
