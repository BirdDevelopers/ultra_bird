
#include "helpers.h"

void delay(unsigned val)
{
	volatile unsigned i;
	for (i=0; i != val; i++);
}
