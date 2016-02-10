#ifndef MTL_REMOTE_ACCEPTOR
#define MTL_REMOTE_ACCPETOR

#include <string>
#include "context.hpp"
#include "stream_caller_mapper.hpp"
#include <future>

namespace mtl
{
	namespace remote
	{
		template<typename Stream,typename FunctionID = std::string>
		struct context_caller_mapper_acceptor
		{
			using mapper = function_mapper< Stream, FunctionID >;
			using stream_context = context< mapper >;

			template<typename R>
			static R accept_stream(Stream &ss)
			{
				auto &current = stream_context::current();
				return current.call_from_stream<R>(ss);
			}

			template<>
			static void accept_stream<void>(Stream &ss)
			{
				auto &current = stream_context::current();
				current.call_from_stream_void(ss);
			}
		};


		//WIP
		template<typename Stream, typename FunctionID = std::string>
		struct delayed_context_caller_mapper_acceptor
		{
			using mapper = function_mapper< Stream, FunctionID >;
			using stream_context = context< mapper >;

			template<typename R>
			static std::future<R> accept_stream(Stream &ss)
			{
				auto &current = stream_context::current();
				
				std::promise<R>  ret;
				auto f = ret.get_future();
				ret.set_value(current.call_from_stream<R>(ss));

				return f;
			}

			template<>
			static std::future<void> accept_stream<void>(Stream &ss)
			{
				auto &current = stream_context::current();

				std::promise<R>  ret;
				auto f = ret.get_future();
				current.call_from_stream_void(ss);
				ret.set_value();

				return f;
			}
		};

	}
}

#endif