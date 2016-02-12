#ifndef MTL_REMOTE_ACCEPTOR
#define MTL_REMOTE_ACCPETOR

#include <string>
#include "context.hpp"
#include "stream_caller_mapper.hpp"
#include "stream_caller_mapper_proxy.hpp"


namespace mtl
{
	namespace remote
	{
		template<typename Stream,typename FunctionID = std::string>
		struct context_caller_mapper_acceptor
		{
			using mapper = function_mapper< Stream, FunctionID >;
			using stream_context = context< mapper >;


			static Stream create_stream()
			{
				return{};
			}

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
		template<typename Stream, typename Proxy >
		struct context_caller_mapper_proxy_acceptor
		{
			using mapper = function_mapper_proxy< Stream, Proxy >;
			using stream_context = context< mapper >;


			static Stream create_stream()
			{
				auto &current = stream_context::current();
				return current.create_stream();
			}

			template<typename R>
			static mtl::future<R> accept_stream(Stream &ss)
			{
				auto &current = stream_context::current();
				return current.call_from_stream<R>(ss);
			}
		};

	}
}

#endif