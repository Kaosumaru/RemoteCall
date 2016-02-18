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

			static Stream create_stream()
			{
				return{};
			}

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


			template<typename R>
			struct TransformType
			{
				using type = R;
			};
		};


		template<typename Stream, typename FunctionID = std::string, typename Acceptor = dummy_acceptor<Stream, FunctionID> >
		struct endpoint
		{
		protected:
			template<typename R, typename... Args>
			struct function_impl
			{
				function_impl(const FunctionID& id) : _id(id) {}

				using ReturnType = decltype(Acceptor::template accept_stream<R>(Stream{}));

				//
				ReturnType operator() (Args... args)
				{
					auto stream = Acceptor::create_stream();
					add_arguments_to_stream<Stream>(stream, _id, std::forward<Args>(args)...);
					return Acceptor::template accept_stream<R>(stream);
				}
			private:
				FunctionID _id;
			};
		public:


			template<typename T>
			struct function
			{

			};

			template<typename R, typename... Args>
			struct function<R(Args...)> : public function_impl<R, typename TransformType<Args>::type...>
			{
				using function_impl<R, typename TransformType<Args>::type...>::function_impl;

			};



		};
	}
}

#endif