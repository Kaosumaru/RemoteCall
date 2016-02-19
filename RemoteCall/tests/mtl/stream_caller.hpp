#ifndef MTL_STREAM_CALLER
#define MTL_STREAM_CALLER

#include "function_traits.hpp"
#include <utility>

namespace mtl
{
	namespace impl
	{


		struct transform_strategy_forward
		{
			//stream_caller requires signature R(Args...)
			//types in stream and Call return type may differ
			//transform_argument_type is used to transfrom Args... into Args'...
			//transform_return_type is used to transform R into R'
			//Call will return R', and stream_caller will assume that Args' are encoded in the stream
			//`convert` is used to convert between X and X' type
			
			//so, if signature is int(int&), transformed signature by default strategy would be int(int)

			template<typename T>
			using transform_return_type = T;

			//transform_argument_type transforms X(A,B,C) into X(A',B',C'), and constructs std::tuple<A',B',C'> args.
			template<typename T>
			using transform_argument_type = typename std::decay<T>::type;

			template<typename TO, typename FROM>
			TO& convert(FROM &t)
			{
				return t;
			}

			template<typename TO, typename FROM>
			static TO&& convert(FROM &&t)
			{
				return std::move(t);
			}
		};


		template<typename Stream, typename Func, typename Transformer = transform_strategy_forward>
		struct stream_caller
		{

		};



		template<typename Stream, typename Transformer, typename R, typename ...Args>
		struct stream_caller<Stream, R(Args...), Transformer>
		{
			template<typename T>
			using transform_return_type = typename Transformer::template transform_return_type<T>;

			template<typename T>
			using transform_argument_type = typename Transformer::template transform_argument_type<T>;


			using ReturnType = transform_return_type<R>;

			template<typename Callable>
			ReturnType Call(Stream &s, const Callable& f)
			{
				return CallImpl(s, f, std::index_sequence_for<Args...>());
			}

		protected:
			template<typename Callable, std::size_t... Is>
			ReturnType CallImpl(Stream &s, const Callable& f, std::index_sequence<Is...>)
			{
				std::tuple< transform_argument_type<Args>... > args;
				auto fill_args = {
					0, ((s >> std::get<Is>(args)), 0)...
				};

				//convert optionally converts arg type from 'transform_argument_type<Args>' to Args
				return Transformer::template convert<ReturnType>(
						f(
							Transformer::template convert<Args>( std::get<Is>(args) )...
						)
					);
			}
		};
	}


	template <typename Stream, typename Func>
	auto call_from_stream(Stream& stream, const Func& func) -> typename impl::stream_caller<Stream, Func>::ReturnType
	{
		return impl::stream_caller<Stream, Func>().Call(stream, func);
	}

	template <typename Strategy, typename Stream, typename Func>
	auto call_from_stream_strategy(Stream& stream, const Func& func) -> typename impl::stream_caller<Stream, Func, Strategy>::ReturnType
	{
		return impl::stream_caller<Stream, Func, Strategy>().Call(stream, func);
	}


	template <typename Stream, typename... Args>
	Stream& add_arguments_to_stream(Stream& ss, Args&&... args)
	{
		auto fill_args = {
			0, ((ss << std::forward<Args>(args)), 0)...
		};
		return ss;
	}

	template <typename Stream, typename... Args>
	Stream arguments_to_stream(Args&&... args)
	{
		Stream ss;
		add_arguments_to_stream(std::forward<Args>(args)...);
		return ss;
	}

}

#endif