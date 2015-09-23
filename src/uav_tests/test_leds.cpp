
#include <stm32f10x.h>
#include "../timer_driver.h"
#include "../led_driver.h"
#include "../serial_stream.h"
#include "helpers.h"
#include <tester.h>
	
UAV_TEST(leds)
{
	serial_stream_stm32 uart0_stream;
	
	uart0_stream << "Initializing LEDs..." << serial_stream::endl;
	led_driver<led_driver_stm32> l1;
	
	uart0_stream << "Checking LED on..." << serial_stream::endl;
	l1.set();
	delay(1000000);
	
	uart0_stream << "Checking LED off..." << serial_stream::endl;
	l1.reset();
	delay(1000000);
	
	uart0_stream << "Initializing LED controller..." << serial_stream::endl;
		
	//timer_driver_stm32 tmr;
	led_controller<timer_driver_stm32, led_driver_stm32, 1> led_ctrl;
	led_ctrl.set_led(0, &l1);
	
	uart0_stream << "Checking always on mode..." << serial_stream::endl;
	l1.set_mode(led_base::mode_t::flash);
	delay(1000000);
	
	uart0_stream << "Checking always off mode..." << serial_stream::endl;
	l1.set_mode(led_base::mode_t::dark);
	delay(1000000);
	
	uart0_stream << "Checking slow blink mode..." << serial_stream::endl;
	l1.set_mode(led_base::mode_t::slow_blink);
	delay(4000000);
	
	uart0_stream << "Checking fast blink mode..." << serial_stream::endl;
	l1.set_mode(led_base::mode_t::blink);
	delay(4000000);
	
	
	
/*	while(1);
	{
		GPIOD->BSRR = GPIO_Pin_13;
		for(dly = 0; dly < 100000; dly++);
		GPIOD->BRR = GPIO_Pin_13;
		for(dly = 0; dly < 100000; dly++);
	}*/
	
	//GPIOD->BSRRH = GPIO_Pin_13;
	
	
}

