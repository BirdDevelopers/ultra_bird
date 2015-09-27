
#include "../serial_stream.h"
#include <tester.h>
#include "drivers/cc2500.h"
#include "spi_bus.h"

UAV_TEST(radio)
{
	serial_stream_stm32 uart0_stream;
	
	uart0_stream << "Testing radio..." << serial_stream::endl;
	
	spi_bus_stm32 spi_bus;
	cc2500_driver<spi_bus_stm32> driver;
}
