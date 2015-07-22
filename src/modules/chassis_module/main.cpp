/*
 * chassis_module.cpp
 *
 * Created: 22.07.2015 21:40:36
 *  Author: ultrablox
 */ 


#include <avr/io.h>

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
		:m_pinId(pin_id), m_mode(mode_t::invalid)
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
			default:
				break;
		}
	}
	
	void tick()
	{
		if(m_mode != mode_t::blink)
			return;
		
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
};

class brain_module
{
public:
	enum class status_t {normal, error, critical_error, invalid};
	
	brain_module()
		:m_status(status_t::invalid), m_ledGreen(0), m_ledRed(1)
	{
		set_status(status_t::normal);
	}
	
	void run()
	{
		led_driver * p_leds[] = {&m_ledGreen, &m_ledRed};
			
		while(true)
		{
			for(unsigned i = 0; i < 2; ++i)
				p_leds[i]->tick();
		}
	}
	
	void set_status(status_t new_status)
	{
		m_status = new_status;
		
		switch(new_status)
		{
			case status_t::normal:
				m_ledGreen.set_mode(led_driver::mode_t::flash);
				m_ledGreen.set_mode(led_driver::mode_t::dark);
				break;
			case status_t::error:
				m_ledGreen.set_mode(led_driver::mode_t::flash);
				m_ledGreen.set_mode(led_driver::mode_t::blink);
				break;
			case status_t::critical_error:
				m_ledGreen.set_mode(led_driver::mode_t::dark);
				m_ledGreen.set_mode(led_driver::mode_t::flash);
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
	chassis_module chs;
	chs.run();
	
}