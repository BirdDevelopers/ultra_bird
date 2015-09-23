
#include <stm32f10x.h>
#include <tester.h>

int stdout_putchar (int ch)
{
	return 0;	
}

int main(void)
{
	tester_singleton::get_singleton()->exec();

	
	/*auto lamb = [](int x){
		int y = x + 3;
	};*/
	
	while (1);
}

