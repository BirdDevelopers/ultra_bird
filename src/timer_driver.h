
#ifndef UAV_TIMER_H
#define UAV_TIMER_H


class timer_driver_stm32
{
public:
	timer_driver_stm32();
	~timer_driver_stm32();
	virtual void event() {};

	static const unsigned hw_timer_count = 12;
	//static timer_driver_stm32 * timers_table[hw_timer_count];
	static timer_driver_stm32 * pTimer2;

};
#endif
