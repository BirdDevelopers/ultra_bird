
#ifndef UAV_EXPRESSION_H
#define UAV_EXPRESSION_H

#include "check.h"
#include "helpers.h"
#include <math.h>
#include <stdexcept>
#include <algorithm>

namespace expr{

	class node
	{
	public:
		virtual float evaluate(float * p_first_arg) const = 0;
	};

	class constant_node : public node
	{
	public:
		constant_node(float _val)
			:val(_val)
		{}

		virtual float evaluate(float * p_first_arg) const override
		{
			return val;
		}
	private:
		float val;
	};

	class arg_node : public node
	{
	public:
		arg_node(unsigned _arg_index)
			:arg_index(_arg_index)
		{}

		virtual float evaluate(float * p_first_arg) const override
		{
			return p_first_arg[arg_index];
		}
	private:
		unsigned arg_index;
	};

	class unary_fun_node : public node
	{
	public:
		enum class fun_t { minus, exp, sign, abs, invalid };
		unary_fun_node(fun_t _fun_type, node * p_child)
			:fun_type(_fun_type), pChild(p_child)
		{}

		~unary_fun_node()
		{
			delete pChild;
		}

		virtual float evaluate(float * p_first_arg) const override
		{
			float child_res = pChild->evaluate(p_first_arg);

			switch (fun_type)
			{
			case fun_t::minus:
				return -child_res;
			case fun_t::exp:
				return exp(child_res);
			case fun_t::sign:
				return sgn(child_res);
			case fun_t::abs:
				return abs(child_res);
			default:
				throw std::runtime_error("Invalid node");
				break;
			}
		}

	private:
		fun_t fun_type;
		node * pChild;
	};

	class binary_fun_node : public node
	{
	public:
		enum class fun_t { add, substract, multiply, divide, power, min, max, invalid };

		binary_fun_node(fun_t _fun_type, node * p_left, node * p_right)
			:fun_type(_fun_type), pLeft(p_left), pRight(p_right)
		{
		}

		~binary_fun_node()
		{
			delete pLeft;
			delete pRight;
		}

		virtual float evaluate(float * p_first_arg) const override
		{
			float left_res = pLeft->evaluate(p_first_arg), 
					right_res = pRight->evaluate(p_first_arg);

			switch (fun_type)
			{
			case fun_t::add:
				return left_res + right_res;
			case fun_t::substract:
				return left_res - right_res;
			case fun_t::multiply:
				return left_res * right_res;
			case fun_t::divide:
				return left_res / right_res;
			case fun_t::power:
				return pow(left_res, right_res);
			case fun_t::min:
				return std::min(left_res, right_res);
			case fun_t::max:
				return std::max(left_res, right_res);
			default:
				throw std::runtime_error("Invalid node");
				break;
			}
		}
	private:
		fun_t fun_type;
		node * pLeft, *pRight;
	};

	class less_condition
	{
	public:
		less_condition(unsigned arg_id, float _val)
			:argId(arg_id), val(_val)
		{
		}

		bool evaluate(float * p_first_arg) const
		{
			return p_first_arg[argId] < val;
		}
	private:
		unsigned argId;
		float val;
	};

	class conditional_node : public node
	{
	public:
		conditional_node(const less_condition & cond, node * p_left, node * p_right)
			:condition(cond), pLeft(p_left), pRight(p_right)
		{
		}

		virtual float evaluate(float * p_first_arg) const override
		{
			if (condition.evaluate(p_first_arg))
				return pLeft->evaluate(p_first_arg);
			else
				return pRight->evaluate(p_first_arg);
		}

		~conditional_node()
		{
			delete pLeft;
			delete pRight;
		}
	private:
		less_condition condition;
		node * pLeft, *pRight;
	};
};

class expression
{
public:
	expression(unsigned arg_count = 0)
		:pRoot(nullptr), argument_count(arg_count)
	{}

	~expression()
	{
		delete pRoot;
	}

	template<typename It>
	float evaluate(It first, It last) const
	{
		check_assertion(pRoot != nullptr);
		check_assertion(std::distance(first, last) == this->argument_count);
		return this->evaluate(&(*first));
	}

	float evaluate(float * p_args) const
	{
		return pRoot->evaluate(p_args);
	}

	void set_root(expr::node * new_root)
	{
		pRoot = new_root;
	}

private:
	expr::node * pRoot;
	unsigned argument_count;
};

#endif
