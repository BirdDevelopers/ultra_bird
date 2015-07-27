
#include "led_driver.h"

led_driver::led_driver(uint8_t pin_id, uint8_t loop_frame_count)
	:m_pinId(pin_id), m_mode(mode_t::invalid), m_blinkCounter(0), m_loopFrameCount(loop_frame_count)
{
	DDRA = set_masked<uint8_t>(DDRA, 0xff, 1 << pin_id);
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
