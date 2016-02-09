#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"
#include "mtl/stream_caller.hpp"
#include <sstream>

#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

//#include "lest.hpp"

using namespace std;

/*
template<class F>
struct function_traits
{
private:
	using call_type = function_traits<decltype(&F::operator())>;
public:
	using return_type = typename call_type::return_type;

	static constexpr std::size_t arity = call_type::arity - 1;

	template <std::size_t N>
	struct argument
	{
		static_assert(N < arity, "error: invalid parameter index.");
		using type = typename call_type::template argument<N + 1>::type;
	};
};
*/


template<class T>
struct register_reflection
{

};

struct Test
{
	int a;
	int b;
};


int add(int a, int b)
{
	return a + b;
}

int main (int argc, char * argv[])
{
	//register_reflection<Test>;

	stringstream ss;
	ss << 1 << 2;

	auto r = mtl::call_from_stream(ss, add);
	return 0;
}
