
#ifndef PCA9685_driver
#define PCA9685_driver

#include "../helpers.h"

class pca9685_driver
{
public:
	error_t init(unsigned char addr);
private:
	unsigned char m_addr;
};

#endif