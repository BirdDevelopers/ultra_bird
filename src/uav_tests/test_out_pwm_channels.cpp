
#include <drivers/pca9685.h>
#include <i2c_wrapper.h>
#include "uav_rt/config.h"
#include <tester.h>

template<typename PCA>
void test_pca9685(PCA & pca)
{
	//Rise
	float val = 0.0f;
	for(; val < 1.0; val += 0.001f)
	{		
		for(unsigned i = 0; i < 15; ++i)
			pca.set_channel_pulse_width(i, val);
	}
	
	//Fall
	for(; val > 0.0; val -= 0.001f)
		for(unsigned i = 0; i < 15; ++i)
			pca.set_channel_pulse_width(i, val);
}

template<typename DRV>
void test_out_pwm_channels(DRV & drv)
{
	for(unsigned i = 0; i < 4; ++i)
	{
		float val = 0.0f;
		for(; val < 1.0; val += 0.001f)
		{
			for(unsigned c = 0; c < 15; ++c)
				drv.set_channel(c, val);
		}
		
		for(; val > 0.0; val -= 0.001f)
		{
			for(unsigned c = 0; c < 15; ++c)
				drv.set_channel(c, val);
		}
	}
}

UAV_TEST(LD1)
{
	i2c_wrapper bus;
	
	uint8_t addr = pca9685_base::get_i2c_address(false, false, false, false, false, false);
	pwm_driver<i2c_wrapper> drv(&bus, addr);
	auto res = drv.init();
	
	//Connect LEDs to test
	//test_pca9685(drv);
	test_out_pwm_channels(drv);
}
