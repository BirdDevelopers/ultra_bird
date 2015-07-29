
#include "led_driver.h"

volatile uint8_t * port_mem(led_driver::port_t port_id)
{
	switch(port_id)
	{
		case led_driver::port_t::A:
			return &PORTA;
		case led_driver::port_t::B:
			return &PORTB;
		case led_driver::port_t::C:
			return &PORTC;
		case led_driver::port_t::D:
			return &PORTD;
	}
	
	return nullptr;
}


volatile uint8_t * ddr_mem(led_driver::port_t port_id)
{
	switch(port_id)
	{
		case led_driver::port_t::A:
			return &DDRA;
		case led_driver::port_t::B:
			return &DDRB;
		case led_driver::port_t::C:
			return &DDRC;
		case led_driver::port_t::D:
			return &DDRD;
	}
	
	return nullptr;
}

led_driver::led_driver(port_t _port, uint8_t pin_id, uint8_t loop_frame_count)
	:m_port(_port), m_pinId(pin_id), m_mode(mode_t::invalid), m_blinkCounter(0), m_loopFrameCount(loop_frame_count)
{
	auto p_ddr = ddr_mem(m_port);
	
	*p_ddr = set_masked<uint8_t>(*p_ddr, 0xff, 1 << pin_id);
	set_mode(mode_t::dark);
}

void led_driver::set_mode(mode_t new_mode)
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

led_driver::mode_t led_driver::mode() const
{
	return m_mode;
}

void led_driver::process_timer_signal()
{
	m_blinkCounter = (++m_blinkCounter) % m_loopFrameCount;
	
	if(m_blinkCounter == 0)
		set();
	else if(m_blinkCounter == (m_mode == mode_t::blink ? m_loopFrameCount / 4 : m_loopFrameCount / 2))
		reset();
}

void led_driver::set()
{
	volatile uint8_t * p_port = port_mem(m_port);
	*p_port = set_masked<uint8_t>(*p_port, 0xff, 1 << m_pinId);
}

void led_driver::reset()
{
	volatile uint8_t * p_port = port_mem(m_port);
	*p_port = set_masked<uint8_t>(*p_port, 0x00, 1 << m_pinId);
}