
#ifndef MODULES_PWM_READER
#define MODULES_PWM_READER

#include "helpers.h"
/*
Reads PWM through internal A-port (6 channels).
*/
class pwm_reader
{
public:
	pwm_reader();
	float channel_value(unsigned ch_idx) const;
	error_t init();
	
	static pwm_reader * p_sharedPwmReader;
private:
	float m_data[6];
};

#endif
