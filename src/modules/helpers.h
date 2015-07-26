
#ifndef MODULES_HELPERS
#define MODULES_HELPERS

template<typename T>
T set_masked(T src, T dest, T mask)
{
	return (src & (~mask)) | (dest & mask);
}

class error_t
{
public:
	error_t(unsigned error_code)
		:m_errorCode(error_code)
	{
	}
	
	bool is_ok() const
	{
		return (m_errorCode == 0);
	}
	
	static error_t ok()
	{
		return error_t(0);
	}
private:
	unsigned m_errorCode;
};

#endif
