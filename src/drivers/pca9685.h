
#ifndef PCA9685_driver
#define PCA9685_driver

#include "../helpers.h"
#include <functional>
#include <stdint.h>

template<typename B>
class i2c_device
{	
public:
	using i2c_bus_t = B;

	i2c_device(i2c_bus_t * i2c_accessor, uint8_t addr)
		:pI2CAcessor(i2c_accessor), address(addr)
	{
	}
	
	void write_register(uint8_t reg_addr, uint8_t val)
	{
		pI2CAcessor->start_transmission(i2c_bus_t::direction_t::Out, address);
		pI2CAcessor->write(reg_addr);
		pI2CAcessor->write(val);
		pI2CAcessor->stop_transmission();
	}
	
	void write_data(uint8_t start_addr, const uint8_t * data, unsigned size)
	{
		pI2CAcessor->start_transmission(i2c_bus_t::direction_t::Out, address);
		pI2CAcessor->write(start_addr);
		for(;size > 0; --size)
			pI2CAcessor->write(*data++);
		pI2CAcessor->stop_transmission();
	}
	
private:
	i2c_bus_t * pI2CAcessor;
	uint8_t address;
};

class pca9685_base
{
public:
	typedef enum
	{
		MODE1 = 0x00,
		MODE2 = 0x01,
		PRE_SCALE = 0xFE
	} Registers;
	
	typedef enum
	{
		ALLCALL = 0,
		SUB3 = 1,
		SUB2 = 2,
		SUB1 = 3,
		SLEEP = 4,
		AI = 5,
		EXTCLK = 6,
		RESTART = 7
	} MODE1_BITS;
	
	typedef enum
	{
		OUTNE0 = 0,
		OUTNE1 = 1,
		OUTDRV = 2,
		OCH = 3,
		INVRT = 4
	} MODE2_BITS;
	
	typedef enum
	{
		NotInvOutputs =	0,
		InvOutputs =	1
	} OutputsInv;

	typedef enum
	{
		OpenDrain =	0,
		TotemPole =	1
	} OutputDriver;

	typedef enum
	{
		OutputNotEn_0 =			0,
		OutputNotEn_OUTDRV =	1,
		OutputNotEn_High_Z1 =	2,
		OutputNotEn_High_Z2 =	3
	} OutputNotEn;

	typedef enum
	{
		F200Hz =	30,
		F100Hz =	60,
		F60Hz =	100,
		F50Hz =	121
	} Frequency;
	
	static uint8_t get_i2c_address(bool a0, bool a1, bool a2, bool a3, bool a4, bool a5)
	{
		return (1 << 7) | ((uint8_t)a5 << 6) | ((uint8_t)a4 << 5) | ((uint8_t)a3 << 4) | ((uint8_t)a2 << 3) | ((uint8_t)a1 << 2);
	}
};

template<typename B>
class pca9685_driver : public i2c_device<B>, public pca9685_base
{

public:
	using _Base = i2c_device<B>;
	using i2c_bus_t = typename _Base::i2c_bus_t;

	pca9685_driver(i2c_bus_t * i2c_accessor, uint8_t addr)
		:_Base(i2c_accessor, addr)
	{
	}
	
	void set_frequency(unsigned freq_hz)
	{
		/*
		PWM frame size is 20ms, so frequency = 50HZ. Internal oscillator freq=25MHz.
		So, prescale = round(25MHz / (4096 * 50Hz)) - 1 = 121.07 ~= 121 = 0x79
		*/
		unsigned presc_val = (25000000U / 4096) / freq_hz - 1;
		//if(presc_val > 0xff)
		//	return error_t(1);
			
		_Base::write_register(PRE_SCALE, presc_val & 0xFF);
	}
	
	error_t init(unsigned freq_hz)
	{
		uint8_t mode1 = (1 << MODE1_BITS::AI) | (0 << MODE1_BITS::ALLCALL);
		
		_Base::write_register(Registers::MODE1, mode1);
		_Base::write_register(Registers::MODE1, (mode1 & 0x7F) | 0x10);
		set_frequency(freq_hz);
		_Base::write_register(Registers::MODE1, mode1);
		
		_Base::write_register(Registers::MODE2, (OutputsInv::NotInvOutputs << MODE2_BITS::INVRT) | 
												(OutputDriver::TotemPole << MODE2_BITS::OUTDRV) | 
												(OutputNotEn::OutputNotEn_0 << MODE2_BITS::OUTNE0));
		
		return error_t::ok();
	}
	
	void write_channel(uint8_t channel, uint8_t led_on_l, uint8_t led_on_h, uint8_t led_off_l, uint8_t led_off_h)
	{
		uint8_t data[] = {led_on_l, led_on_h, led_off_l, led_off_h};
		_Base::write_data(get_led_register(channel), data, sizeof(data));
	}
	
	void set_channel_pulse_width(uint8_t channel, float ratio)
	{
		unsigned pulse_width = 4096 * ratio - 1;
		write_channel(channel,
						0x00, 0x00, //0-delay time
						//0x01, 0x5C);	
						pulse_width & 0xff, (pulse_width >> 8) & 0xff);
	}

private:
	uint8_t get_led_register(uint8_t channel) const
	{
		return 0x06 + 4 * channel;
	}
};

/*
Manipulates RC-hardware with 50Hz 1-2ms standard PWM signal.
*/
template<typename B>
class pwm_driver : public pca9685_driver<B>
{
public:
	using _Base = pca9685_driver<B>;
	using i2c_bus_t = typename _Base::i2c_bus_t;

	pwm_driver(i2c_bus_t * i2c_accessor, uint8_t addr)
		:_Base(i2c_accessor, addr)
	{
	}
	
	error_t init()
	{
		return _Base::init(50);
	}
	//error_t init(unsigned char addr);
	error_t set_channel(unsigned channel, float val)
	{
		if((val < 0.0f) || (val > 1.0f))
			return error_t(1);
		
		float pulse_width_ratio = (1.0f + val) / 20.0f;
		_Base::set_channel_pulse_width(channel, pulse_width_ratio);
		
		return error_t::ok();
	}
	
};

#endif
