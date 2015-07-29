
#include "brain_module.h"

timer_driver * timer_driver::p_sharedTimer = nullptr;

// timer0 overflow
ISR(TIMER0_OVF_vect)
{
	timer_driver::p_sharedTimer->fire();
}

brain_module::brain_module()
	:m_status(status_t::invalid), m_ledGreen(led_driver::port_t::C, 6), m_ledRed(led_driver::port_t::C, 7)
{
	register_led(m_ledGreen);
	register_led(m_ledRed);
	
	set_status(status_t::normal);
	//sei();
}

void brain_module::register_led(led_driver & ldriver)
{
	m_leds.push_back(&ldriver);
}

void brain_module::run()
{
	//led_driver * p_leds[] = {&m_ledGreen, &m_ledRed};
	
	timer_driver tmr(this);
	run_loop();
	
}

void brain_module::run_loop()
{
	while(true)
	{
	}
}

void brain_module::fire()
{
	//led_driver * p_leds[] = {&m_ledGreen, &m_ledRed};
	for(auto p_led : m_leds)
	{
		if((p_led->mode() == led_driver::mode_t::blink) || (p_led->mode() == led_driver::mode_t::slow_blink))
		p_led->process_timer_signal();
	}
}

void brain_module::set_status(status_t new_status)
{
	m_status = new_status;
	
	switch(new_status)
	{
		case status_t::normal:
			m_ledGreen.set_mode(led_driver::mode_t::flash);
			m_ledRed.set_mode(led_driver::mode_t::dark);
			break;
		case status_t::error:
			m_ledGreen.set_mode(led_driver::mode_t::flash);
			m_ledRed.set_mode(led_driver::mode_t::blink);
			break;
		case status_t::critical_error:
			m_ledGreen.set_mode(led_driver::mode_t::dark);
			m_ledRed.set_mode(led_driver::mode_t::flash);
			break;
	}
}
