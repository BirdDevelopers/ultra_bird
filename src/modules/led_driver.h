
#ifndef MODULES_LED_DRIVER
#define MODULES_LED_DRIVER

#include "helpers.h"
#include <avr/io.h>

//Driver for A-ports
class led_driver
{
public:
	enum class mode_t {flash, blink, slow_blink, dark, invalid};
	enum class port_t {A, B, C, D};

	led_driver(port_t _port, uint8_t pin_id, uint8_t loop_frame_count = 30);
	void set_mode(mode_t new_mode);
	mode_t mode() const;
	
	/*
	Blinking is performed as:
	F----, where each frame is fired by timer
	and has equal length.
	*/
	void process_timer_signal();
	
private:
	void set();
	void reset();
	
private:
	uint8_t m_pinId;
	mode_t m_mode;
	port_t m_port;
	
	unsigned m_blinkCounter;
	uint8_t m_loopFrameCount;
};

#endif
