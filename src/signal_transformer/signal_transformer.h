
#ifndef SIGNAL_TRANSFORMER_H
#define SIGNAL_TRANSFORMER_H

#include <check.h>
#include <vector>
#include <expression.h>

class signal_transform
{
public:
	signal_transform(unsigned _in_count, unsigned _out_count)
		:in_count(_in_count), out_count(_out_count), compiled_exprs(out_count, nullptr)
	{}

	virtual ~signal_transform()
	{
		for (auto exp : compiled_exprs)
			delete exp;
	}

	unsigned input_channel_count() const
	{
		return in_count;
	}

	template<typename InIt, typename OutIt>
	void compute(InIt first_in, OutIt first_out) const
	{
		for (unsigned i = 0; i < out_count; ++i)
		{
			check_assertion(compiled_exprs[i] != nullptr);
			*first_out++ = compiled_exprs[i]->evaluate(&(*first_in));
		}
	}
	
	void compile()
	{
		for (unsigned i = 0; i < this->out_count; ++i)
		{
			delete this->compiled_exprs[i];
			this->compiled_exprs[i] = compile_channel(i);
		}
	}

	virtual expression * compile_channel(unsigned channel_id) const = 0;
protected:
	template<typename T>
	void check_range(T val, T min_val, T max_val) const
	{
		if ((val > max_val) || (val < min_val))
		{
			throw std::runtime_error("Out of range");
		}
	}
protected:
	unsigned in_count, out_count;
	std::vector<expression*> compiled_exprs;
};
/*
Maps one channel to another with zero/positive/negative sign.
One input channel can be mapped to many output channels.
*/
class direct_transform : public signal_transform
{
public:
	enum class mapping_t {direct, reverse, zero};

	struct mapping_channel_t
	{
		mapping_channel_t()
			:input_channel_id(-1), mapping(mapping_t::zero)
		{
		}

		unsigned input_channel_id;
		mapping_t mapping;
	};

	direct_transform(unsigned in_count, unsigned out_count)
		:signal_transform(in_count, out_count), mappings(out_count)
	{}
	void set_mapping(unsigned out_index, unsigned in_index, mapping_t map_type)
	{
		mappings[out_index].input_channel_id = in_index;
		mappings[out_index].mapping = map_type;
	}

	void reset_mapping(unsigned out_index)
	{
		mappings[out_index] = mapping_channel_t();
	}

	virtual expression * compile_channel(unsigned channel_id) const override
	{
		auto new_exp = new expression(this->in_count);
		if (mappings[channel_id].mapping == mapping_t::zero)
			new_exp->set_root(new expr::constant_node(0.0f));
		else
		{
			if (mappings[channel_id].mapping == mapping_t::direct)
				new_exp->set_root(new expr::arg_node(mappings[channel_id].input_channel_id));
			else
				new_exp->set_root(new expr::unary_fun_node(expr::unary_fun_node::fun_t::minus, new expr::arg_node(mappings[channel_id].input_channel_id)));
		}

		return new_exp;
	}

private:
	std::vector<mapping_channel_t> mappings;
};

/*
Is determined by min, zero and max points.
*/

//Creates node for K*x+B
expr::node * create_linear_node(unsigned arg_id, float k, float b)
{
	return new expr::binary_fun_node(expr::binary_fun_node::fun_t::add,
					new expr::binary_fun_node(expr::binary_fun_node::fun_t::multiply, new expr::constant_node(k), new expr::arg_node(arg_id)),
					new expr::constant_node(b));
}

class hw_limits_transform : public signal_transform
{
	struct channel_props_t
	{
		float min, zero, max;
	};
public:
	hw_limits_transform(unsigned channel_count)
		:signal_transform(channel_count, channel_count), props(channel_count)
	{}

	virtual expression * compile_channel(unsigned channel_id) const override
	{
		const channel_props_t & prop = props[channel_id];

		auto * root = new expr::conditional_node(expr::less_condition(channel_id, 0.0f), 
			create_linear_node(channel_id, prop.zero - prop.min, prop.zero),
			create_linear_node(channel_id, prop.max - prop.zero, prop.zero));

		auto new_exp = new expression(this->in_count);
		new_exp->set_root(root);
		return new_exp;
	}

	void set_prop(unsigned channel_index, float min = -1.0f, float zero = 0.0f, float max = 1.0f)
	{
		auto & prop = props[channel_index];
		prop.max = max;
		prop.min = min;
		prop.zero = zero;
	}
private:
	std::vector<channel_props_t> props;
};

/*
Is determined by dependency matrix.
*/
class mix_transform : public signal_transform
{

};

/*
Dual rates, exponent and trim.
*/
class adjust_transform : public signal_transform
{
	struct channel_props_t
	{
		float exp,//Range 0.0-1.0
			rates,	//Rates 0.0 - 1.0
			minTrim, maxTrim;
	};

public:
	adjust_transform(unsigned channel_count)
		:signal_transform(channel_count, channel_count), props(channel_count)
	{

	}

	void set_props(unsigned idx, float exp = 1.0f, float dual_rates = 1.0f, float min_trim = -1.0f, float max_trim = 1.0f)
	{
		check_range(exp, 0.0f, 1.0f);
		props[idx].exp = exp;

		check_range(dual_rates, 0.0f, 1.0f);
		props[idx].rates = dual_rates;

		check_range(min_trim, -1.0f, 1.0f);
		props[idx].minTrim = min_trim;

		check_range(max_trim, -1.0f, 1.0f);
		props[idx].maxTrim = max_trim;
	}

	virtual expression * compile_channel(unsigned channel_id) const override
	{
		const channel_props_t & prop = props[channel_id];

		expr::node * cur_node = new expr::unary_fun_node(expr::unary_fun_node::fun_t::abs, new expr::arg_node(channel_id));

		if (prop.exp != 1.0f)
		{
			cur_node = new expr::binary_fun_node(expr::binary_fun_node::fun_t::multiply,
				new expr::binary_fun_node(expr::binary_fun_node::fun_t::multiply,
				cur_node,
					new expr::constant_node(prop.exp)),
				new expr::binary_fun_node(expr::binary_fun_node::fun_t::power,
					new expr::constant_node(1.0f / prop.exp),
					cur_node));
		}
		
		if (prop.rates < 1.0f)
		{
			cur_node = new expr::binary_fun_node(expr::binary_fun_node::fun_t::multiply,
				new expr::constant_node(prop.rates),
				cur_node);
		}


		if (prop.maxTrim < 1.0f)
		{
			cur_node = new expr::binary_fun_node(expr::binary_fun_node::fun_t::min,
				new expr::constant_node(prop.maxTrim),
				cur_node);
		}

		if (prop.minTrim > -1.0f)
		{
			cur_node = new expr::binary_fun_node(expr::binary_fun_node::fun_t::max,
				new expr::constant_node(prop.minTrim),
				cur_node);
		}

		cur_node = new expr::binary_fun_node(expr::binary_fun_node::fun_t::multiply,
								cur_node,
								new expr::unary_fun_node(expr::unary_fun_node::fun_t::sign, new expr::arg_node(channel_id)));

		auto new_exp = new expression(this->in_count);
		new_exp->set_root(cur_node);
		return new_exp;
	}
private:
	std::vector<channel_props_t> props;
};

class signal_transformer
{
public:
	signal_transformer(unsigned in_channel_count, unsigned out_channel_count)
		:cur_value(out_channel_count, 0.0f), input_channel_count(in_channel_count)
	{

	}

	template<typename It>
	void compute(It first, It last)
	{
		check_assertion(std::distance(first, last), this->input_channel_count());
	}

	const float * get_out_value() const
	{
		return cur_value.data();
	}
private:
	std::vector<float> cur_value;
	unsigned input_channel_count;

};

class uav_signal_transformer : public signal_transformer
{
public:
	uav_signal_transformer(unsigned logical_hw_channel_count, unsigned physical_hw_channel_count)
		:signal_transformer(logical_hw_channel_count, physical_hw_channel_count)
	{
	}
};

class station_signal_transformer : public signal_transformer
{
public:
};

#endif
