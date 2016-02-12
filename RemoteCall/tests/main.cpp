#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"
#include "mtl/stream_caller_mapper.hpp"
#include "mtl/remote_endpoint.hpp"
#include "mtl/remote_acceptor.hpp"
#include "mtl/future.hpp"
#include "mtl/stream_channel.hpp"
#include "mtl/lockless/slist.hpp"
#include <sstream>
#include <cassert>

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





	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_channel_proxy : public stream_channel<Stream>
	{
	public:
		using request_id_type = uint32_t;
		using ProxyCallback = std::function<void(Stream& ss)>;

		using RequestMap = std::map<request_id_type, ProxyCallback>;

		Stream create_stream()
		{
			Stream arg;
			arg << _magic_number;
			arg << _magic_number;
			return arg;
		}

		void proxy_call(Stream& arg, const ProxyCallback& callback)
		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			

			auto id = _last_id++;

			//replacing header in stream - stream should be created by us in create_stream
			//so we are replacing first _magic_number
			{
				auto pp = arg.tellp();
				arg.seekp(0);
				arg << id;
				arg.seekp(pp);
			}

			_requests[id] = callback;
			send_stream(arg);
		}


		void received_stream(Stream& ss) override
		{
			request_id_type id;
			ss >> id;

			request_id_type number;
			ss >> number;
			if (number != _magic_number)
			{
				assert(false);
				return;
			}



			ProxyCallback callback;
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				auto it = _requests.find(id);
				if (it == _requests.end())
					return;
				callback = it->second;
				_requests.erase(it);
			}
			callback(ss);
		}

	protected:
		constexpr static request_id_type _magic_number = 0xFF003200;
		request_id_type _last_id = 1;
		std::recursive_mutex _mutex;
		RequestMap _requests;
	};


	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_channel_proxy_receiver : public stream_channel<Stream>
	{
	public:
		using request_id_type = uint32_t;
		using ProxyCallback = std::function<void(Stream& ss)>;

		void received_stream(Stream& ss) override
		{
			request_id_type id;
			ss >> id;

			request_id_type number;
			ss >> number;
			if (number != _magic_number)
			{
				assert(false);
				return;
			}

			Stream out;

			out << id;
			out << _magic_number;

			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_mapper.call_from_stream_out(ss, out);
			}

			send_stream(out);
		}


		auto& mapper() { return _mapper; }
	protected:
		using mapper_type = function_mapper<Stream, FunctionID>;

		constexpr static request_id_type _magic_number = 0xFF003200;
		std::recursive_mutex _mutex;
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


	mtl::test::local_stream_sender<mtl::binary_stream> local;

	//proxy at sender
	using Proxy = mtl::function_mapper_channel_proxy<mtl::binary_stream>;
	using mapper_type = mtl::function_mapper_proxy<mtl::binary_stream, Proxy>;
	auto functions = std::make_shared<mapper_type>();
	local.streams()[0].add_stream(functions);


	//mapper at receiver
	{
		using receiver_mapper_type = mtl::function_mapper_channel_proxy_receiver<mtl::binary_stream>;
		auto receiver = std::make_shared<receiver_mapper_type>();
		receiver->mapper().add_function("add", add);

		local.streams()[1].add_stream(receiver);
	}


	//using endpoint
	using acceptor = mtl::remote::context_caller_mapper_proxy_acceptor<mtl::binary_stream, Proxy>;
	using endpoint = mtl::remote::endpoint<mtl::binary_stream, std::string, acceptor>;


	endpoint::function<int(int, int)> remote_add = { "add" };
	
	//locking proxy sender
	auto l = acceptor::stream_context::lock(functions);
	auto f = remote_add(1, 2);
	f.then([](int &a) 
	{
	});


	return 0;
}
