#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"
#include "mtl/stream_caller_mapper.hpp"
#include "mtl/remote_endpoint.hpp"
#include "mtl/remote_acceptor.hpp"
#include <sstream>


#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

//#include "lest.hpp"



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






using namespace std;

int add(int a, int b)
{
	return a + b;
}

int main (int argc, char * argv[])
{

	//register_reflection<Test>;
	mtl::function_mapper<mtl::binary_stream> functions;
	functions.add_function("add", add);


	using acceptor = mtl::remote::context_caller_mapper_acceptor<mtl::binary_stream>;
	using endpoint = mtl::remote::endpoint<mtl::binary_stream, std::string, acceptor>;


	endpoint::function<void(int, int)> remote_add = { "add" };
	
	
	auto l = acceptor::stream_context::lock(functions);
	remote_add(1, 2);


	/*
	stringstream ss;
	ss << std::quoted("add") << 1 << " " << 2;


	functions.call_from_stream(ss);
	*/

	return 0;
}
