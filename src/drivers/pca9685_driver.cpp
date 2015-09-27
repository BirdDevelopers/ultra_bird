
#include "pca9685_driver.h"
extern "C" {
	#include <i2c.h>
}

#define MODE1				0x00
#define MODE1_ALLCALL	0
#define MODE1_SUB3		1
#define MODE1_SUB2		2
#define MODE1_SUB1		3
#define MODE1_SLEEP		4
#define MODE1_AI		5
#define MODE1_EXTCLK	6
#define MODE1_RESTART	7

#define MODE2				0x01
#define MODE2_OUTNE0	0
#define MODE2_OUTNE1	1
#define MODE2_OUTDRV	2
#define MODE2_OCH		3
#define MODE2_INVRT		4

#define SUBADR1				0x02
#define SUBADR2				0x03
#define SUBADR3				0x04
#define ALLCALLADR			0x05

#define LED0				0x06 // location for start of LED0 registers
#define LEDn(n)				(LED0 + (n)*4)
#define LEDn_ON_L(n)		(0x06 + (n)*4)
#define LEDn_ON_H(n)		(0x07 + (n)*4)
#define LEDn_OFF_L(n)		(0x08 + (n)*4)
#define LEDn_OFF_H(n)		(0x09 + (n)*4)

#define ALL_LED_ON_L		0xFA
#define ALL_LED_ON_H		0xFB
#define ALL_LED_OFF_L		0xFC
#define ALL_LED_OFF_H		0xFD

#define PRE_SCALE			0xFE

#define MAX_OUTPUT_INDEX	15
#define MAX_OUTPUT_VALUE	0xFFF

typedef enum
{
	PCA9685_NotInvOutputs =	0,
	PCA9685_InvOutputs =	1
} PCA9685_InvOutputs_TypeDef;
#define IS_PCA9685_INV_OUTPUTS(INVRT)	(((INVRT) == PCA9685_NotInvOutputs) || ((INVRT) == PCA9685_InvOutputs))

typedef enum
{
	PCA9685_OutputDriver_OpenDrain =	0,
	PCA9685_OutputDriver_TotemPole =	1
} PCA9685_OutputDriver_TypeDef;
#define IS_PCA9685_OUTPUT_DRIVER(OUTPUT_DRIVER)	(((OUTPUT_DRIVER) == PCA9685_OutputDriver_OpenDrain) || ((OUTPUT_DRIVER) == PCA9685_OutputDriver_TotemPole))

typedef enum
{
	PCA9685_OutputNotEn_0 =			0,
	PCA9685_OutputNotEn_OUTDRV =	1,
	PCA9685_OutputNotEn_High_Z1 =	2,
	PCA9685_OutputNotEn_High_Z2 =	3
} PCA9685_OutputNotEn_TypeDef;
#define IS_PCA9685_OUTPUT_NOT_EN(OUTNE)	(((OUTNE) >= PCA9685_OutputNotEn_0) && ((OUTNE) <= PCA9685_OutputNotEn_High_Z2))


/**
 * @brief	PCA9685 Frequency
 *			Set by prescale = round(25 MHz / (4096 * freq)) - 1
 */
typedef enum
{
	PCA9685_Frequency_200Hz =	30,
	PCA9685_Frequency_100Hz =	60,
	PCA9685_Frequency_60Hz =	100,
	PCA9685_Frequency_50Hz =	121
} PCA9685_Frequency;
#define IS_PCA9685_FREQUENCY(FREQ) ((FREQ) == PCA9685_Frequency_200Hz || \
									(FREQ) == PCA9685_Frequency_100Hz || \
									(FREQ) == PCA9685_Frequency_60Hz || \
									(FREQ) == PCA9685_Frequency_50Hz)
									
error_t i2c_write_register(unsigned char device_addr, unsigned char register_addr, unsigned char register_val)
{
	unsigned char data[2] = {register_addr, register_val};
	return i2cMasterSendNI(device_addr, sizeof(data), data) == I2C_OK ? error_t::ok() : error_t(1);
}
/*
error_t i2c_send(unsigned char device_addr, std::initializer_list<unsigned char> & data)
{
	return error_t::ok();
}*/

//#define INIT_SEND_I2C(addr, data)

error_t pca9685_driver::init(unsigned char addr, unsigned freq_hz)
{
	m_addr = addr;
	
	//return error_t::ok();
	i2cSetLocalDeviceAddr(0x0A, TRUE);
	
	/* MODE1 Register:
		 * Internal clock
		 * Register Auto-Increment enabled
		 * Normal mode
		 * Does not respond to subaddresses
		 * Responds to All Call I2C-bus address
	*/
	unsigned char mode1 = (1 << MODE1_AI) | (0 << MODE1_ALLCALL);
	
	auto r = i2c_write_register(m_addr, MODE1, mode1);
	if(!r.is_ok())
		return r;
		
	r = i2c_write_register(m_addr, MODE1, (mode1 & 0x7F) | 0x10);
	if(!r.is_ok())
		return r;
		
	set_frequency(freq_hz);
	
	r = i2c_write_register(m_addr, MODE1, mode1);
	if(!r.is_ok())
		return r;
		
	/* MODE2 Register:
		 * Outputs change on STOP command
		 */
		
	r = i2c_write_register(m_addr, MODE2, (PCA9685_InvOutputs_TypeDef::PCA9685_NotInvOutputs << MODE2_INVRT) | 
												(PCA9685_OutputDriver_TypeDef::PCA9685_OutputDriver_TotemPole << MODE2_OUTDRV) | 
												(PCA9685_OutputNotEn_TypeDef::PCA9685_OutputNotEn_0 << MODE2_OUTNE0));
	if(!r.is_ok())
		return r;
	
	/*
	if(!r.is_ok())
		return r;*/
		
	//unsigned char data[] = {LEDn(0), 0x00, 0x00, 0xFF, 0x07};
	for(unsigned char i = 0; i < 16; ++i)
	{
		unsigned char data[] = {LEDn(i), 0x00, 0x00, 0x00, 0x10};
		i2cMasterSendNI(m_addr, sizeof(data), data);	
	}
	
	//unsigned char data[] = {LEDn(8), 0x10, 0x00, 0x00, 0x00};
	//i2cMasterSendNI(m_addr, sizeof(data), data);
	//i2c_send(m_addr, {LEDn(8), 0x10, 0x00, 0x00, 0x00});
	
	return error_t::ok();
}

error_t pca9685_driver::set_frequency(unsigned freq_hz)
{
	/*
	PWM frame size is 20ms, so frequency = 50HZ. Internal oscillator freq=25MHz.
	So, prescale = round(25MHz / (4096 * 50Hz)) - 1 = 121.07 ~= 121 = 0x79
	*/
	unsigned presc_val = (25000000U / 4096) / freq_hz - 1;
	//if(presc_val > 0xff)
	//	return error_t(1);
		
	return i2c_write_register(m_addr, PRE_SCALE, presc_val & 0xFF);
	//return i2c_write_register(m_addr, PRE_SCALE, PCA9685_Frequency::PCA9685_Frequency_50Hz);
}

error_t pca9685_driver::write_channel(unsigned channel, unsigned char led_on_l, unsigned char led_on_h, unsigned char led_off_l, unsigned char led_off_h)
{
	unsigned char data[] = {LEDn(channel), led_on_l, led_on_h, led_off_l, led_off_h};
	return i2cMasterSendNI(m_addr, sizeof(data), data) == I2C_OK;
}

//===============PWM driver======================

error_t pwm_driver::init(unsigned char addr)
{
	auto r = m_ledDriver.init(addr, 50);
	if(!r.is_ok())
		return r;
	
	//Set PWM
	//r = m_ledDriver.set_frequency();
	
	return r;
}

error_t pwm_driver::set_channel(unsigned channel, float val)
{
	if((val < 0.0f) || (val > 1.0f))
		return error_t(1);
	/*
	Duty cycle is 20ms, min val = 1ms, max - 2ms. 20ms - 4096. Let's
	start from the beginning.
	Delay time - 0
	
	*/
	
	float pulse_width_ratio = (1.0f + val) / 20.0f;
	//float pulse_width_ratio = val;
	
	unsigned pulse_width = 4096 * pulse_width_ratio - 1;
	return m_ledDriver.write_channel(channel,
											0x00, 0x00, //0-delay time
											//0x01, 0x5C);	
											pulse_width & 0xff, (pulse_width >> 8) & 0xff);	//
}
