
#ifndef UAV_TESTER_H
#define UAV_TESTER_H

#include "singleton.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

template<typename T>
void test_check_eq(const std::string & msg, const T & checked_val, const T & correct_val)
{
	if (checked_val != correct_val)
		std::cout << "Test failed: " << msg << std::endl;
}

struct test_invoker_base
{
	virtual void run() = 0;
};

class uav_tester
{
public:
	using test_func_t = void(*)(void);

	uav_tester()
		:errorCount(0)
	{}
	
	int exec()
	{
		//std::cout << "Running " << invokers.size() << " test cases" << std::endl;

		for (auto inv : invokers)
			inv->run();

		if (errorCount)
		{
			//std::cout << "Test failed: total " << errorCount << " errors" << std::endl;
		}
		else
		{
			//std::cout << "All tests passed" << std::endl;
		}
		return 0;
	}

	template<typename T>
	void check_eq(const char * file_name, unsigned line_number, const T & checked_val, const T & correct_val)
	{
		if (checked_val != correct_val)
		{
			/*std::stringstream ss;
			ss << checked_val << " != " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "not equal");
		}
	}
	
	template<typename T>
	void check_neq(const char * file_name, unsigned line_number, const T & checked_val, const T & correct_val)
	{
		if (checked_val == correct_val)
		{
			/*std::stringstream ss;
			ss << checked_val << " == " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "equal");
		}
	}

private:
	void submit_error(const char * file_name, unsigned line_number, const char * error_text)
	{
		++errorCount;
		std::cout << "Error (" << file_name << ':' << line_number << "): " << error_text << std::endl;
	}
public:
	std::vector<test_invoker_base*> invokers;
private:
	unsigned errorCount;
};

typedef autocreate_singleton<uav_tester> tester_singleton;

#define UAV_TEST_NEQ(exp1, exp2) CREATE_UAV_TEST_EQ(__FILE__, __LINE__, exp1, exp2)
#define CREATE_UAV_TEST_NEQ(filename, linenum, exp1, exp2) tester_singleton::get_singleton()->check_neq(filename, linenum, exp1, exp2)

#define UAV_TEST_EQ(exp1, exp2) CREATE_UAV_TEST_EQ(__FILE__, __LINE__, exp1, exp2)
#define CREATE_UAV_TEST_EQ(filename, linenum, exp1, exp2) tester_singleton::get_singleton()->check_eq(filename, linenum, exp1, exp2)

#define UAV_TEST(test_name) CREATE_UAV_TEST(uav_test_##test_name)
#define CREATE_UAV_TEST(struct_name) struct struct_name : public test_invoker_base{\
						struct_name(){\
							tester_singleton::get_singleton()->invokers.push_back(this);\
						}\
						virtual void run() override;\
						static struct_name invoker_instance;\
                   };\
				   struct_name struct_name::invoker_instance;\
				void struct_name::run()


#endif

