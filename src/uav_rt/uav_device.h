
#ifndef UAV_DEVICE_H
#define UAV_DEVICE_H

#include "../led_driver.h"

class uav_led_table
{
public:
	uav_led_table()
	{
	}
private:
	led_driver_stm32 leds[5];
};

class uav_device
{
public:
	uav_device()
	{
	}
private:
	uav_led_table ledTable;
};

#endif
