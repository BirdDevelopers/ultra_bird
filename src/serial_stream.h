
#ifndef UAV_SERIAL_STREAM_H
#define UAV_SERIAL_STREAM_H

#include <stdint.h>
#include <string>
#include <stdio.h>

class serial_stream
{
public:
	struct endl_t
	{};
		
	static const endl_t endl;
};

class serial_stream_stm32 : public serial_stream
{
public:
	serial_stream_stm32();
	~serial_stream_stm32();
	void append_buffer(uint8_t val);
	void clear_buffer();
	void write(uint8_t c);
	void read(uint16_t & val);

	void write(const char * p_str);
	void write(const serial_stream::endl_t &);

	template<typename T>
	void write_basetype(T val)
	{
		char buffer[32];
		unsigned n = sprintf(buffer, "%d", val);
		for(unsigned i = 0; i < n; ++i)
		{
			write((uint8_t)buffer[i]);
		}
	}
	
	void write(int val);
	void write(unsigned val);

	/*template<typename T>
	void write(const T & val)
	{
	}*/	

	template<typename T>
	friend serial_stream_stm32 & operator<<(serial_stream_stm32 & ss, const T & val)
	{
		ss.write(val);
		return ss;
	}
	
	template<typename T>
	friend serial_stream_stm32 & operator>>(serial_stream_stm32 & ss, T & val)
	{
		ss.read(val);
		return ss;
	}
	
	bool is_ready() const
	{
		return (curPos > 0) && (inBuffer[curPos-1] == 0x0D);
	}
	
	const char * buffer() const
	{
		return inBuffer;
	}
	
	static serial_stream_stm32 * pSS;
	
private:
	char inBuffer[4096];
	unsigned curPos;
};

#endif
