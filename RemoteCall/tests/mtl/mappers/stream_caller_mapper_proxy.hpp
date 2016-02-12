#ifndef MTL_STREAM_CALLER_MAPPER_PROXY
#define MTL_STREAM_CALLER_MAPPER_PROXY

#include <functional>
#include <map>
#include <unordered_map>
#include <future>
#include "../stream_caller.hpp"
#include "../context.hpp"
#include "../future.hpp"

namespace mtl
{



	template<typename Stream, typename Proxy>
	class function_mapper_proxy : public Proxy
	{

	public:
		using ProxyCallback = std::function<void(Stream& ret)>;

		Stream create_stream()
		{
			return Proxy::create_stream();
		}

		template<typename R>
		mtl::future<R> call_from_stream(Stream& arg)
		{
			auto promise = mtl::promise<R>::create();
			Proxy::proxy_call(arg, [=](Stream& ret)
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
			Proxy::proxy_call(arg, [=](Stream& ret)
			{
				promise->set_value();
			});

			return promise->get_future();
		}


		auto& proxy() { return (Proxy&)*this; }
	protected:
	};

}

#endif