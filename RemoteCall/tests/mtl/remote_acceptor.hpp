#ifndef MTL_REMOTE_ACCEPTOR
#define MTL_REMOTE_ACCPETOR

#include <string>
#include "context.hpp"
#include "stream_caller_mapper.hpp"

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
				current.call_from_stream(ss); //todo get return
				return {};
			}

			template<>
			static void accept_stream<void>(Stream &ss)
			{
				auto &current = stream_context::current();
				current.call_from_stream(ss);
				return;
			}
		};


	}
}

#endif