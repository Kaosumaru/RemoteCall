#ifndef MTL_STREAM_CALLER_MAPPER_ASYNC
#define MTL_STREAM_CALLER_MAPPER_ASYNC

#include <functional>
#include <map>
#include <unordered_map>
#include <future>
#include "stream_caller.hpp"
#include "context.hpp"
#include "future.hpp"

namespace mtl
{
	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_async_proxy
	{
	private:
		template<typename R, typename T>
		struct LambdaCreator
		{
			static auto create(T& function)
			{
				return [=](Stream& ss, Stream& out)
				{
					using c = context<R, function_mapper_async_proxy>;
					out << mtl::call_from_stream(ss, function);
				};
			}
		};

		template<typename T>
		struct LambdaCreator<void,T>
		{
			static auto create(T& function)
			{
				return [=](Stream& ss, Stream& out)
				{
					mtl::call_from_stream(ss, function);
				};
			}
		};

	public:
		using Function = std::function<void(Stream& ss, Stream& out)>;
		using Map = std::unordered_map<FunctionID, Function>;

		template<typename T>
		void add_function(const FunctionID& id, const T& function)
		{
			using traits = function_traits<T>;
			auto& f = _functions[id];
			if (f)
				throw std::domain_error("Duplicate entry");
			
			f = LambdaCreator<typename traits::return_type, T >::create(function);
		}

		void call_from_stream_out(Stream& ss, Stream& out)
		{
			FunctionID id;
			ss >> id;
			auto it = _functions.find(id);
			if (it == _functions.end())
				throw std::domain_error("Unknown function");

			auto &f = it->second;
			f(ss, out);
		}


		template<typename R>
		mtl::future<R> call_from_stream(Stream& ss)
		{
			auto promise = mtl::promise<R>::create();
			Stream out;
			call_from_stream_out(ss, out);

			R r;
			ss >> r;
			promise->set_value(r);


			return promise->get_future();
		}

		template<>
		mtl::future<void> call_from_stream(Stream& ss)
		{
			auto promise = mtl::promise<void>::create();
			Stream out;
			call_from_stream_out(ss, out);
			promise->set_value();
			return promise->get_future();
		}

	protected:


		Map _functions;
	};

}

#endif