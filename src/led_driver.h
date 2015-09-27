
#ifndef MODULES_LED_DRIVER
#define MODULES_LED_DRIVER

#include <vector>
#include <stdint.h>

class led_base
{
public:
	enum class mode_t {flash, blink, slow_blink, dark, invalid};

	led_base();
	mode_t mode() const;
	void set_mode(mode_t new_mode);
	
protected:
	mode_t m_mode;
};

class led_driver_stm32 : public led_base
{
public:
	led_driver_stm32();
	~led_driver_stm32();
	void set();
	void reset();
};


//Driver for A-ports
template<typename B>
class led_driver : public B, public led_base
{
	using _Base = B;
public:

	led_driver()
		:m_blinkCounter(0), m_loopFrameCount(32)
	{
		set_mode(mode_t::dark);
	}
	/*
	void set_mode(led_base::mode_t new_mode)
	{
		led_base::set_mode(new_mode);
		switch(new_mode)
		{
			case mode_t::flash:
				_Base::set();
				break;
			case mode_t::dark:
				_Base::reset();
				break;
			case mode_t::blink:
				m_blinkCounter = 0;
				break;
			default:
			break;
		}
	}*/
	
	/*
	Blinking is performed as:
	F----, where each frame is fired by timer
	and has equal length.
	*/
	void process_timer_signal()
	{
		if((m_mode == mode_t::blink) || (m_mode == mode_t::slow_blink))
		{
			m_blinkCounter = (++m_blinkCounter) % m_loopFrameCount;
		
			if(m_blinkCounter == 0)
				_Base::set();
			else if(m_blinkCounter == (m_mode == mode_t::blink ? m_loopFrameCount / 8 : m_loopFrameCount / 2))
				_Base::reset();
		}
	}
	
	
private:
	//uint8_t m_pinId;
	mode_t m_mode;
	
	unsigned m_blinkCounter;
	uint8_t m_loopFrameCount;
};


template<typename B, typename L, int Num>
class led_controller : public B
{
public:
	using _Base = B;
	using led_t = led_driver<L>;
	
	led_controller()
		:_Base()
	{}
		
	virtual void event() override
	{
		for(unsigned i = 0; i < Num; ++i)
			pLeds[i]->process_timer_signal();
	}
	
	void set_led(unsigned index, led_t * p_led)
	{
		pLeds[index] = p_led;
	}
	
	
private:
	led_t* pLeds[Num];
};

#endif
