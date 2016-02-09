#ifndef MTL_REMOTE_ENDPOINT
#define MTL_REMOTE_ENDPOINT

#include <string>
#include "binary_stream.hpp"
#include "stream_caller.hpp"


namespace mtl
{
	namespace remote
	{
		template<typename Stream,typename FunctionID = std::string>
		struct dummy_acceptor
		{
			template<typename R>
			static R accept_stream(Stream &ss)
			{
				return {};
			}

			template<>
			static void accept_stream<void>(Stream &ss)
			{
				return;
			}
		};


		template<typename Stream, typename FunctionID = std::string, typename Acceptor = dummy_acceptor<Stream, FunctionID> >
		struct endpoint
		{


			template<typename T>
			struct function
			{

			};

			template<typename R, typename... Args>
			struct function<R(Args...)>
			{
				function(const FunctionID& id) : _id(id) {}

				R operator() (Args... args)
				{
					auto stream = arguments_to_stream<Stream>(_id, std::forward<Args>(args)...);
					return Acceptor::template accept_stream<R>(stream);
				}
			private:
				FunctionID _id;
			};

		};
	}
}

#endif