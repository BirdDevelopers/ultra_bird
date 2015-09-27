

#ifndef UAV_LOGGER_H
#define UAV_LOGGER_H

#define LOGGER_LEVEL 0

template<typename S, typename C>
class logger
{
	enum level_t
	{
		DEBUG = 0,
		INFO = 1,
		WARNING = 2,
		ERROR = 3,
		SILENT = 4,
	} level;
public:
	using stream_t = S;
	using clock_source_t = C;

	logger(stream_t & strm, clock_source_t * p_cs)
		:stream(strm), clockSource(p_cs)
	{
	}
	
	template<typename T>
	friend logger & operator<<(logger & _logger, const T & val)
	{
		_logger.stream << val;
		return _logger;
	}
	
	template<typename ... ArgTypes>
	void debug(const ArgTypes& ... args)
	{
		log(level_t::DEBUG, args ...);
	}
	
	template<typename ... ArgTypes>
	void info(const ArgTypes& ... args)
	{
		log(level_t::INFO, args ...);
	}
	
	template<typename ... ArgTypes>
	void warning(const ArgTypes& ... args)
	{
		log(level_t::WARNING, args ...);
	}
	
	template<typename ... ArgTypes>
	void error(const ArgTypes& ... args)
	{
		log(level_t::ERROR, args ...);
	}
private:
	template<typename ... ArgTypes>
	void log(level_t level, const ArgTypes& ... args)
	{
		unsigned ts = clockSource->get_time();
		stream << "RTC:" << ts << " ";
		print_level(level);
		stream << " ";
		log_print(args ...);		
	}
	
	void log_print()
	{
		stream << serial_stream::endl;
	}
	
	template<typename Type, typename ... ArgTypes>
	void log_print(const Type & val, const ArgTypes& ... args)
	{
		stream << val;
		log_print(args ...);
	}
	
	void print_level(level_t level)
	{
		switch(level)
		{
			case level_t::DEBUG:
				stream << "DEBUG";
				break;
			case level_t::INFO:
				stream << "INFO";
				break;
			case level_t::WARNING:
				stream << "WARNING";
				break;
			case level_t::ERROR:
				stream << "ERROR";
				break;
			default:
				break;
		};
	}
	
private:
	stream_t & stream;
	clock_source_t * clockSource;
};


#endif
