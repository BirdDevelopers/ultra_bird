
#ifndef MODULES_LED_DRIVER
#define MODULES_LED_DRIVER

#include "helpers.h"
#include <avr/io.h>

//Driver for A-ports
class led_driver
{
public:
	enum class mode_t {flash, blink, slow_blink, dark, invalid};

	led_driver(uint8_t pin_id, uint8_t loop_frame_count = 5);
	void set_mode(mode_t new_mode);
	mode_t mode() const;
	
	/*
	Blinking is performed as:
	F----, where each frame is fired by timer
	and has equal length.
	*/
	void process_timer_signal();
	
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
	uint8_t m_loopFrameCount;
};

#endif
