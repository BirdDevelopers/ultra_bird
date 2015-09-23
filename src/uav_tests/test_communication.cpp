
#include "../serial_stream.h"
#include "../logger.h"
#include <tester.h>
#include <stm32f10x.h>

UAV_TEST(uart_stream)
{
	serial_stream_stm32 uart0_stream;
	
	
	//Simple uint
	uart0_stream << (uint8_t)0x2f;
	
	//Newline
	uart0_stream << serial_stream::endl;
	//Simple chars
	uart0_stream << (uint8_t)'X' << serial_stream::endl;
	uart0_stream << (uint8_t)'Y' << serial_stream::endl;
	uart0_stream << (uint8_t)'Z' << serial_stream::endl;
	
	//Number
	uart0_stream << 18446744 << serial_stream::endl;
	
	//String
	uart0_stream << "Hi, Logger!" << serial_stream::endl;
	
	//String + new line
	uart0_stream << "String will end" << serial_stream::endl << "and new will start" << serial_stream::endl;
	
	//Some complex string
	uart0_stream << "5+2=" << 5+2 << serial_stream::endl;
	
	uint16_t val = 0;
	uart0_stream << "Input some number: ";
	uart0_stream >> val;
	uart0_stream << "Yoo entered " << val << serial_stream::endl;
}

struct clock_source
{
	unsigned get_time() const
	{
		return RTC->CNTL;
	}
};

UAV_TEST(logger)
{
	serial_stream_stm32 uart0_stream;
	
	clock_source cs;
	
	logger<serial_stream_stm32, clock_source> logger_obj(uart0_stream, &cs);
	logger_obj.debug("test debug message");
	logger_obj.info("test info message");
	logger_obj.warning("test warning message");
	logger_obj.error("test error message");
	
	logger_obj.info("Complex info mwssage with numbers:", 3, "+", 12, "=", 3+12);
	
}
