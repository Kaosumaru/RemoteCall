#ifndef MTL_STREAM_CALLER_MAPPER_PROXY
#define MTL_STREAM_CALLER_MAPPER_PROXY

#include <functional>
#include <map>
#include <unordered_map>
#include <future>
#include "stream_caller.hpp"
#include "context.hpp"
#include "future.hpp"

namespace mtl
{



	template<typename Stream, typename Proxy>
	class function_mapper_proxy
	{

	public:
		using ProxyCallback = std::function<void(Stream& ret)>;



		void proxy_call(Stream& arg, const ProxyCallback& callback)
		{
			_proxy.proxy_call(arg, callback);
		}


		template<typename R>
		mtl::future<R> call_from_stream(Stream& arg)
		{
			auto promise = mtl::promise<R>::create();
			proxy_call(arg, [=](Stream& ret)
			{
				R r;
				ret >> r;
				promise->set_value(r);
			});

			return promise->get_future();
		}

		template<>
		mtl::future<void> call_from_stream(Stream& arg)
		{
			auto promise = mtl::promise<void>::create();
			proxy_call(arg, [=](Stream& ret)
			{
				promise->set_value();
			});

			return promise->get_future();
		}


		auto& proxy() { return _proxy; }
	protected:
		Proxy _proxy;
	};

}

#endif