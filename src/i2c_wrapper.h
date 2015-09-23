
#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include <stdint.h>

class i2c_wrapper
{
public:
	enum class direction_t {In, Out};
		
	i2c_wrapper(uint8_t addr = 0x00);
	~i2c_wrapper();
	void start_transmission(direction_t dir, uint8_t slave_addr);
	void stop_transmission();
	void write(uint8_t val);
};

#endif
