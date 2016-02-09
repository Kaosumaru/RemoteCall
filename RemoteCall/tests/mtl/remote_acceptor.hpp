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
				R ret;
				current.call_from_stream<R>(ss, [&](auto && r) { ret = std::move(r); });
				return ret;
			}

			template<>
			static void accept_stream<void>(Stream &ss)
			{
				auto &current = stream_context::current();
				call_from_stream_void(ss, nullptr);
			}
		};


		//WIP
		template<typename Stream, typename FunctionID = std::string>
		struct delayed_context_caller_mapper_acceptor
		{
			using mapper = function_mapper< Stream, FunctionID >;
			using stream_context = context< mapper >;

			template<typename R>
			static std::promise<R> accept_stream(Stream &ss)
			{
				auto &current = stream_context::current();
				std::promise<R> ret;
				current.call_from_stream<R>(ss, [&](auto && r) { ret.set_value(std::move(r)); });
				return ret;
			}

			template<>
			static std::promise<void> accept_stream<void>(Stream &ss)
			{
				auto &current = stream_context::current();

				std::promise<void> ret;
				call_from_stream_void(ss, [&]() { ret.set_value(); });
				return ret;
			}
		};

	}
}

#endif