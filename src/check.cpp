
#include "check.h"
#include <stdexcept>

void check_assertion(bool val)
{
	if (!val)
		throw std::runtime_error("Assertion failed");
}
