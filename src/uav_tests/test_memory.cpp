
#include "../serial_stream.h"
#include <stdlib.h>
#include <vector>
#include <tester.h>

UAV_TEST(memory)
{
	serial_stream_stm32 uart0_stream;
	
	uart0_stream << "Checking malloc...";
	char *p;
  p = (char*)malloc(1000); /* allocate 1000 bytes */
	
	//UAV_TEST_NEQ(p, (char*)0);
	
  if(p)
		uart0_stream << "success" << serial_stream::endl;
	else
		uart0_stream << "failed" << serial_stream::endl;
	free(p);
	
	
	uart0_stream << "Checking new operator...";
	int * p_int = new int;
	
	//UAV_TEST_NEQ(p_int, (int*)0);
	
	if(p_int)
		uart0_stream << "success" << serial_stream::endl;
	else
		uart0_stream << "failed" << serial_stream::endl;
	
	delete p_int;
	
	
	uart0_stream << "Checking vector...";
	
	std::vector<int> data;
	data.resize(2);
	/*for(int i = 0; i < 1000; ++i)
	{
		data[i] = (i + 3) / 5;
	}*/
}
