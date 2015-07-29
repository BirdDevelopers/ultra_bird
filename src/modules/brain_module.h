
#ifndef MODULES_BRAIN_MODULE
#define MODULES_BRAIN_MODULE

#include "led_driver.h"
#include <avr/interrupt.h>
#include <iterator>
#include <vector>

class timer_delegate
{
public:
	virtual void fire() {};
	
};

class timer_driver
{
public:
	timer_driver(timer_delegate * p_delegate)
		:m_pDelegate(p_delegate)
	{
		p_sharedTimer = this;
		TCNT0 = 0x00;	// set timer0 counter initial value to 0
		TCCR0 |= ((1 << CS00) | (1 << CS02));	// start timer0 with /1024 prescaler  
		TIMSK |= (1 << TOIE0) ; // setup timer 0 to interrupt on overflow		
	}
	
	void fire()
	{
		m_pDelegate->fire();
	}
	
	
	static timer_driver * p_sharedTimer;
private:
	timer_delegate * m_pDelegate;
};

class brain_module : public timer_delegate
{
public:
	enum class status_t {normal, error, critical_error, invalid};
	
	brain_module();
	void register_led(led_driver & ldriver);
	void run();
	virtual void fire() override;
	void set_status(status_t new_status);
	virtual void run_loop();
	
private:
	status_t m_status;
	led_driver m_ledGreen,	//A0 port
				m_ledRed;	//A1 port
	std::vector<led_driver*> m_leds;
};

#endif
