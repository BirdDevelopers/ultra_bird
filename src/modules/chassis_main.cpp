/*
 * chassis_module.cpp
 *
 * Created: 22.07.2015 21:40:36
 *  Author: ultrablox
 */ 

#define F_CPU 16000000UL

#include "led_driver.h"
#include "brain_module.h"
#include "drivers/pca9685_driver.h"
#include "pwm_reader.h"
#include <avr/io.h>
#include "chassis.pb.h"
#include "global.h"

extern "C" {
	#include <i2c.h>
}


class chassis_module : public brain_module
{
	enum class mode_t {normal, direct, combined, invalid};
public:
	chassis_module()
		:m_ledMode(led_driver::port_t::B, 1)
	{
		register_led(m_ledMode);		
		set_mode(mode_t::direct);
	}
	
	error_t init()
	{
		sei();
		i2cInit();
		
		//Initialize PWM driver		
		auto res = m_pwmDriver.init(0x80);
		if(!res.is_ok())
		{
			set_status(brain_module::status_t::critical_error);
			return error_t(1);
		}
		
		//Initialize ADC
		
		set_status(brain_module::status_t::normal);
		
		m_pwmDriver.set_channel(0, 1.0f);
		m_pwmDriver.set_channel(1, 1.0f);
		
		return error_t::ok();
	}
	
	void set_mode(mode_t new_mode)
	{
		m_mode = new_mode;
		
		switch(m_mode)
		{
			case mode_t::normal:
				m_ledMode.set_mode(led_driver::mode_t::flash);
				break;
			case mode_t::direct:
				m_ledMode.set_mode(led_driver::mode_t::blink);
				break;
			case mode_t::combined:
				m_ledMode.set_mode(led_driver::mode_t::slow_blink);
				break;
			default:
				break;
		}
	}
	
	/*
	0 - OK
	1 - unknown command
	2+ - command specific
	*/
	response_m process_message(const request_m & msg)
	{
		response_m res;
		
		switch(msg.cmd_id)
		{
			case command_id_SET_SINGLE_OUTPUT:
			{
				//auto err = m_pwmDriver.set_channel(msg.out_vals())
				res.error_code = 1;
				break;
			}
			case command_id_SET_ALL_OUTPUTS:
				res.error_code = 1;
				break;
			case command_id_GET_SINGLE_INPUT:
				res.error_code = 1;
				break;
			case command_id_GET_ALL_INPUTS:
				res.error_code = 1;
				break;
			default:
				res.error_code = 1;
				break;
		}
		
		return res;
	}
	
	virtual void run_loop() override
	{
		while(true)
		{
			switch(m_mode)
			{
				case mode_t::combined:
				{
					if(m_pwmReader.channel_value(0) < 0.5f)
						break;
				}
				case mode_t::direct:
				{
					for(unsigned i = 0; i < 6; ++i)
						m_pwmDriver.set_channel(i, m_pwmReader.channel_value(i));
					break;
				}
				default:
					break;
			}
		}
	}
	
private:
	led_driver m_ledMode;
	mode_t m_mode;
	pwm_driver m_pwmDriver;
	pwm_reader m_pwmReader;
};
	
int main(void)
{
	chassis_module chs;
	chs.init();
	
	chs.run();
}
