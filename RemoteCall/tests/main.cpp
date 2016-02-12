#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"
#include "mtl/stream_caller_mapper.hpp"
#include "mtl/remote_endpoint.hpp"
#include "mtl/remote_acceptor.hpp"
#include "mtl/future.hpp"
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



namespace mtl
{
	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_thread_proxy
	{
	public:
		using ProxyCallback = std::function<void(Stream& ss)>;

		void proxy_call(Stream& arg, const ProxyCallback& callback)
		{
			Stream ret;
			_mapper.call_from_stream_out(arg, ret);
			callback(ret);
		}


		auto& mapper() { return _mapper; }
	protected:
		using mapper_type = function_mapper<Stream, FunctionID>;

		mapper_type _mapper;
	};
}





using namespace std;

int add(int a, int b)
{
	return a + b;
}




int main (int argc, char * argv[])
{
	//in short:
	//- endpoint is used to configure remote function
    //- function creates strem from arguments, and sends it to acceptor, and returns data from acceptor. context_caller_mapper_proxy_acceptor sends stream to current function_mapper_proxy
	//- function_mapper_proxy gets args, and return future<R> with result of call (internally uses Proxy to do the work besides that)
	//- Proxy is resposible for sending arg stream, and return ret stream when done
	//
	//In the end, you get your mtl::future<R> from stream provided by Proxy

	using Proxy = mtl::function_mapper_thread_proxy<mtl::binary_stream>;
	mtl::function_mapper_proxy<mtl::binary_stream, Proxy> functions;
	functions.proxy().mapper().add_function("add", add);

	
	using acceptor = mtl::remote::context_caller_mapper_proxy_acceptor<mtl::binary_stream, Proxy>;
	using endpoint = mtl::remote::endpoint<mtl::binary_stream, std::string, acceptor>;


	endpoint::function<int(int, int)> remote_add = { "add" };
	
	
	auto l = acceptor::stream_context::lock(functions);
	auto f = remote_add(1, 2);
	f.then([](int &a) 
	{
	});




	
	/*
	stringstream ss;
	ss << std::quoted("add") << 1 << " " << 2;


	functions.call_from_stream(ss);
	*/

	return 0;
}
