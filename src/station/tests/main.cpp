
#include <expression.h>
#include <signal_transformer/signal_transformer.h>
#include <tester.h>
#include <random>

using namespace std;

static std::default_random_engine gen;

UAV_TEST(expression)
{
	std::uniform_real_distribution<float> distr(-1000.0, 1000.0);

	//Constant node
	{
		expression expr;
		expr.set_root(new expr::constant_node(3.0f));
		UAV_TEST_EQ(expr.evaluate(nullptr), 3.0f);
	}

	//Arg node
	{
		expression expr(3);
		expr.set_root(new expr::arg_node(2));

		float targs[] = { 0.0, 0.0, 0.0 };
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 3; ++j)
				targs[j] = distr(gen);
			UAV_TEST_EQ(expr.evaluate(targs), targs[2]);
		}
	}

	//Conditional node
	{
		expression expr(1);
		expr.set_root(new expr::conditional_node(expr::less_condition(0, 0.5), new expr::constant_node(12.0f), new expr::constant_node(-33.0f)));

		float targs[] = { 0.0};
		for (int i = 0; i < 100; ++i)
		{
			targs[0] = distr(gen);

			if (targs[0] < 0.5)
				UAV_TEST_EQ(expr.evaluate(targs), 12.0f);
			else
				UAV_TEST_EQ(expr.evaluate(targs), -33.0f);
		}
	}
	
	//Binary add
	{
		expression expr(2);
		auto l_arg = new expr::arg_node(0), r_arg = new expr::arg_node(1);

		expr.set_root(new expr::binary_fun_node(expr::binary_fun_node::fun_t::add, l_arg, r_arg));

		float targs[] = { 0.0, 0.0};
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 2; ++j)
				targs[j] = distr(gen);
			UAV_TEST_EQ(expr.evaluate(targs), targs[0] + targs[1]);
		}
	}
}

UAV_TEST(direct_transform)
{
	std::default_random_engine gen;
	std::uniform_real_distribution<float> distr(-1000.0, 1000.0);

	direct_transform trans(4, 6);
	trans.set_mapping(0, 0, direct_transform::mapping_t::direct);
	trans.set_mapping(1, 0, direct_transform::mapping_t::reverse);
	trans.set_mapping(2, 1, direct_transform::mapping_t::direct);
	trans.set_mapping(3, 2, direct_transform::mapping_t::reverse);

	trans.compile();
	
	float targs[] = { 0.0, 0.0, 0.0, 0.0 };
	float res[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	for (int i = 0; i < 1000; ++i)
	{
		for (int j = 0; j < 4; ++j)
			targs[j] = distr(gen);

		trans.compute(targs, res);

		UAV_TEST_EQ(res[0], targs[0]);
		UAV_TEST_EQ(res[1], -targs[0]);
		UAV_TEST_EQ(res[2], targs[1]);
		UAV_TEST_EQ(res[3], -targs[2]);
	}
}

UAV_TEST(adjust_transform)
{
	std::default_random_engine gen;
	std::uniform_real_distribution<float> distr(-1000.0, 1000.0);

	adjust_transform trans(1);
	trans.set_props(0, 0.4);

}

UAV_TEST(hw_limits_transform)
{
	std::uniform_real_distribution<float> x_distr(-1.0, 1.0);

	hw_limits_transform trans(1);
	trans.set_prop(0, -0.6, 0.2, 0.5);
	trans.compile();

	float targs[] = {0.0};
	float res[] = { 0.0 };
		
	auto correct_fun = [](float x){
		if (x < 0.0f)
			return 0.8*x + 0.2;
		else
			return 0.3*x + 0.2;
	};

	for (int i = 0; i < 1000; ++i)
	{
		targs[0] = x_distr(gen);
		trans.compute(targs, res);

		UAV_TEST_EQ(res[0], (float)correct_fun(targs[0]));
	}
}

UAV_TEST(uav_transforms)
{
	uav_signal_transformer trans(5, 6);
	
}



int main(int argc, char ** argv)
{
	//uav_tester tester_instance;
	return tester_singleton::get_singleton()->exec();
}
