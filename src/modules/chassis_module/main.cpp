/*
 * chassis_module.cpp
 *
 * Created: 22.07.2015 21:40:36
 *  Author: ultrablox
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

template<typename T>
T set_masked(T src, T dest, T mask)
{
	return (src & (~mask)) | (dest & mask);
}

//Driver for A-ports
class led_driver
{
public:
	enum class mode_t {flash, blink, dark, invalid};
		
	led_driver(uint8_t pin_id)
		:m_pinId(pin_id), m_mode(mode_t::invalid), m_blinkCounter(0)
	{
		DDRA = set_masked<uint8_t>(DDRA, 0xff, 1 << pin_id);
		set_mode(mode_t::dark);
	}
	
	void set_mode(mode_t new_mode)
	{
		m_mode = new_mode;
		switch(m_mode)
		{
			case mode_t::flash:
				set();
				break;
			case mode_t::dark:
				reset();
				break;
			case mode_t::blink:
				m_blinkCounter = 0;
				break;
			default:
				break;
		}
	}
	
	mode_t mode() const
	{
		return m_mode;
	}
	
	/*
	Blinking is performed as:
	F----, where each frame is fired by timer
	and has equal length.
	*/
	void process_timer_signal()
	{
		m_blinkCounter = (++m_blinkCounter) % 5;
		
		switch(m_blinkCounter)
		{
			case 0:
				set();
				break;
			case 1:
				reset();
				break;
			default:
				break;
		}
	}
	
private:
	void set()
	{
		PORTA = set_masked<uint8_t>(PORTA, 0xff, 1 << m_pinId);
	}
	
	void reset()
	{
		PORTA = set_masked<uint8_t>(PORTA, 0x00, 1 << m_pinId);
	}
	
private:
	uint8_t m_pinId;
	mode_t m_mode;
	
	unsigned m_blinkCounter;
};

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

timer_driver * timer_driver::p_sharedTimer = nullptr;

// timer0 overflow
ISR(TIMER0_OVF_vect)
{
	timer_driver::p_sharedTimer->fire();
}

class brain_module : public timer_delegate
{
public:
	enum class status_t {normal, error, critical_error, invalid};
	
	brain_module()
		:m_status(status_t::invalid), m_ledGreen(0), m_ledRed(1)
	{
		set_status(status_t::normal);
		//sei();
	}
	
	void run()
	{
		led_driver * p_leds[] = {&m_ledGreen, &m_ledRed};
		
		timer_driver tmr(this);
			
		while(true)
		{
			//for(unsigned i = 0; i < 2; ++i)
			//	p_leds[i]->tick();
		}
	}
	
	virtual void fire() override
	{
		led_driver * p_leds[] = {&m_ledGreen, &m_ledRed};
		for(unsigned i = 0; i < 2; ++i)
		{
			if(p_leds[i]->mode() == led_driver::mode_t::blink)
				p_leds[i]->process_timer_signal();
		}
	}
	
	void set_status(status_t new_status)
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
	
private:
	status_t m_status;
	led_driver m_ledGreen,	//A0 port
				m_ledRed;	//A1 port
};

class chassis_module : public brain_module
{
public:
	chassis_module()
	{
		//Initialize PWM driver
		
		//Initialize ADC
	}
};
	
int main(void)
{
	sei();
	
	chassis_module chs;
	chs.set_status(brain_module::status_t::error);
	chs.run();
	
}