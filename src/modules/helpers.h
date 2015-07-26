
#ifndef MODULES_HELPERS
#define MODULES_HELPERS

template<typename T>
T set_masked(T src, T dest, T mask)
{
	return (src & (~mask)) | (dest & mask);
}

#endif
