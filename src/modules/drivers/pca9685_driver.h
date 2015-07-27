
#ifndef PCA9685_driver
#define PCA9685_driver

#include "../helpers.h"

class pca9685_driver
{
public:
	error_t init(unsigned char addr, unsigned freq_hz);
	error_t set_frequency(unsigned freq_hz);
	error_t write_channel(unsigned channel, unsigned char led_on_l, unsigned char led_on_h, unsigned char led_off_l, unsigned char led_off_h);
private:
	unsigned char m_addr;
};

/*
Manipulates RC-hardware with 50Hz 1-2ms standard PWM signal.
*/
class pwm_driver
{
public:
	error_t init(unsigned char addr);
	error_t set_channel(unsigned channel, float val);
private:
	pca9685_driver m_ledDriver;
};

#endif