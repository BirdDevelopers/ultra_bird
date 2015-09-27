
#ifndef UAV_LED_CONTROL_TASK_H
#define UAV_LED_CONTROL_TASK_H

#include "uav_task.h"
#include "led_driver.h"

template<typename L>
void led_control_task(void * param)
{
	auto p_led = static_cast<L*>(param);
	
	auto last_mode = p_led->mode();
		while(true)
		{
			last_mode = p_led->mode();
			switch(last_mode)
			{
				case led_base::mode_t::flash:
				{
					p_led->set();
					vTaskDelay(600);
					break;
				}
				case led_base::mode_t::blink:
				{
					p_led->set();
					vTaskDelay(50);
					p_led->reset();
					vTaskDelay(550);
					break;
				}
				case led_base::mode_t::slow_blink:
				{
					p_led->set();
					vTaskDelay(300);
					p_led->reset();
					vTaskDelay(300);
					break;
				}
				case led_base::mode_t::dark:
				{
					p_led->reset();
					vTaskDelay(600);
					break;
				}
			};		
		}
}
/*
template<typename T>
	class led_control_task : public uav_task
{
	using led_driver_t = T;
public:
	led_control_task(led_driver_t * p_driver)
		:pLEDDriver(p_driver)
	{
	}
	
	
	void start()
	{
		xTaskCreate(task_fun, "uav task", 256, this, tskIDLE_PRIORITY+3, &handle);
	}
	
	static void task_fun(void* parm)
	{
		static_cast<led_control_task<T>*>(parm)->run();
  }
	
	void run()
	{
		auto last_mode = pLEDDriver->mode();
		while(true)
		{
			last_mode = pLEDDriver->mode();
			switch(last_mode)
			{
				case led_base::mode_t::flash:
				{
					pLEDDriver->set();
					vTaskDelay(600);
					break;
				}
				case led_base::mode_t::blink:
				{
					pLEDDriver->set();
					vTaskDelay(50);
					pLEDDriver->reset();
					vTaskDelay(550);
				}
				case led_base::mode_t::slow_blink:
				{
					pLEDDriver->set();
					vTaskDelay(300);
					pLEDDriver->reset();
					vTaskDelay(300);
				}
				case led_base::mode_t::dark:
				{
					pLEDDriver->reset();
					vTaskDelay(600);
					break;
				}
			};		
		}
	}
private:
	led_driver_t * pLEDDriver;
};*/

#endif
