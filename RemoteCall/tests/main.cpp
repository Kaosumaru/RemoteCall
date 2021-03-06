#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"
#include "mtl/mappers/stream_caller_mapper.hpp"
#include "mtl/pointer/remote_pointer.hpp"
#include "mtl/remote_endpoint.hpp"
#include "mtl/remote_acceptor.hpp"
#include "mtl/future.hpp"

#include "mtl/stream_channel.hpp"
#include "mtl/mappers/stream_caller_mapper_channel_proxy.hpp"
#include <sstream>

#include "mtl/stream_channel_netLink.hpp"


#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

//#include "lest.hpp"


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


struct TestStructPointer
{
	int a = 6;

	int getA()
	{
		return a;
	}
};
TestStructPointer _tsp;

TestStructPointer* get_tsp()
{
	return &_tsp;
}

auto get_a_from_tsp(TestStructPointer* tsp)
{
	return tsp->a;
}

namespace mtl
{
	namespace remote
	{
		template<>
		struct class_traits<TestStructPointer>
		{
			using pointer_type = raw_pointer_unsafe<TestStructPointer>;
			const char* name = "TestStructPointer";

			struct member_functions
			{
				mtl::remote::member_function<int()> getA = { "getA" };
			};
		};
	}
}


int add(int a, int b)
{
	return a + b;
}

float my_div(float a, float b)
{
	if (b == 0.0f)
		throw std::exception("b shouldn't be 0");
	return a / b;
}

struct default_caller_proxy
{
	using Proxy = mtl::function_mapper_channel_proxy<mtl::binary_stream>;
	using acceptor = mtl::remote::context_caller_mapper_proxy_acceptor<mtl::binary_stream, Proxy>;
	using endpoint = mtl::remote::endpoint<mtl::binary_stream, std::string, acceptor>;
	using stream_context = acceptor::stream_context;

	static auto create_sending_channel()
	{
		using mapper_type = mtl::function_mapper_proxy<mtl::binary_stream, Proxy>;
		auto channel = std::make_shared<mapper_type>();
		return channel;
	}

	static auto create_receiving_channel()
	{
		using receiver_mapper_type = mtl::function_mapper_channel_proxy_receiver<mtl::binary_stream>;
		auto channel = std::make_shared<receiver_mapper_type>();
		return channel;
	}
};


void test1()
{
	//in short:
	//- endpoint is used to configure remote function
	//- function creates strem from arguments, and sends it to acceptor, and returns data from acceptor. context_caller_mapper_proxy_acceptor sends stream to current function_mapper_proxy
	//- function_mapper_proxy gets args, and return future<R> with result of call (internally uses Proxy to do the work besides that)
	//- Proxy is resposible for sending arg stream, and return ret stream when done
	//
	// In default implementation, proxy is also responsible for creating a Stream for args.
	//In the end, you get your mtl::future<R> from stream provided by Proxy


	mtl::test::local_stream_sender<mtl::binary_stream> local;


	//proxy at sender
	auto functions = default_caller_proxy::create_sending_channel();
	local.streams()[0].add_channel(functions);


	//mapper at receiver
	{
		auto receiver = default_caller_proxy::create_receiving_channel();
		receiver->mapper().add_function("add", add);

		local.streams()[1].add_channel(receiver);
	}


	//using endpoint
	using endpoint = default_caller_proxy::endpoint;
	using stream_context = default_caller_proxy::stream_context;

	endpoint::function<int(int, int)> remote_add = { "add" };

	//locking proxy sender
	auto l = stream_context::lock(functions);
	auto f = remote_add(1, 2);
	f.then([](int &a)
	{
	});
}


//using endpoint
using endpoint = default_caller_proxy::endpoint;
using stream_context = default_caller_proxy::stream_context;

endpoint::function<int(int, int)> remote_add = { "add" };
endpoint::function<float(float, float)> remote_div = { "mdiv" };
endpoint::function<TestStructPointer*()> remote_get_tsp = { "get_tsp" };
endpoint::function<int(TestStructPointer*)> remote_get_a_from_tsp = { "get_a_from_tsp" };

void test2()
{
#ifdef WIN32
	netLink::init();
#endif
	using namespace std::chrono;

	/*
	mtl::promise<int> p1;
	auto f1 = p1.get_future();
	f1.then([](int &a) 
	{
		mtl::promise<int> p2;
		auto f2 = p2.get_future();
		f2.then([](auto &a) {});
	});

	{
		std::function<void(int&)> f1 = [](auto &a)
		{
			
		};
		std::function<void(int&)> f2 = [](auto &x) {};
	}
	*/

	mtl::netLink::stream_sender<mtl::binary_stream> server("127.0.0.1", 6987, true);
	{
		auto receiver = default_caller_proxy::create_receiving_channel();
		receiver->mapper().add_function("add", add);
		receiver->mapper().add_function("mdiv", my_div);
		receiver->mapper().add_function("get_tsp", get_tsp);
		receiver->mapper().add_function("get_a_from_tsp", get_a_from_tsp);

		server.add_channel(receiver);
	}
	server.start_listening();
	std::this_thread::sleep_for(500ms);
	

	mtl::netLink::stream_sender<mtl::binary_stream> client("127.0.0.1", 6987);
	
	auto functions = default_caller_proxy::create_sending_channel();
	client.add_channel(functions);
	client.start_listening();
	std::this_thread::sleep_for(500ms);
	
	
	




	//locking proxy sender
	auto l = stream_context::lock(functions);

	auto f = remote_add(1, 2);
	f.then([](int &a)
	{
		remote_add(1, 2).then([](int &a) {});
	});

	remote_div(3.0f, 2.0f).then([](float &a)
	{

	});

	
	auto o = remote_get_tsp();
	o.then([](mtl::remote::raw_pointer_unsafe<TestStructPointer> &p) //strangely, auto &p doesn't work
	{
		auto fx = remote_get_a_from_tsp(p);

		//p.remote().getA();

		fx.then([](int &a)
		{
		});

	});
	



	std::this_thread::sleep_for(30s);
}


int main (int argc, char * argv[])
{
	test2();


	return 0;
}
