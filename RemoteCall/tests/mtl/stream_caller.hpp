#ifndef MTL_STREAM_CALLER
#define MTL_STREAM_CALLER

#include "function_traits.hpp"
#include <utility>
#include "pointer/remote_pointer.hpp"

namespace mtl
{
	namespace impl
	{
		template<typename Stream, typename Func>
		struct stream_caller
		{

		};

		template<typename Stream, typename R, typename ...Args>
		struct stream_caller<Stream, R(Args...)>
		{
			using ReturnType = typename remote::TransformType<R>::type; //= R; 

			template<typename Callable>
			ReturnType Call(Stream &s, const Callable& f)
			{
				return CallImpl(s, f, std::index_sequence_for<Args...>());
			}

		protected:
			//TODO perhaps that should be dependant on stream?
			template<typename T>
			using TransformTypeCallType = typename remote::TransformType<typename std::decay<T>::type>::type; // = typename std::decay<T>::type;

			template<typename Callable, std::size_t... Is>
			ReturnType CallImpl(Stream &s, const Callable& f, std::index_sequence<Is...>)
			{
				std::tuple< TransformTypeCallType<Args>... > args;
				auto fill_args = {
					0, ((s >> std::get<Is>(args)), 0)...
				};

				//unwrap_type transforms remote_pointers into pointers (this can throw if pointer is invalid)
				return remote::wrap_type<ReturnType>(f(remote::unwrap_type<Args>(std::get<Is>(args))...));
			}
		};
	}


	template <typename Stream, typename Func>
	auto call_from_stream(Stream& stream, const Func& func) -> typename impl::stream_caller<Stream, Func>::ReturnType
	{
		return impl::stream_caller<Stream, Func>().Call(stream, func);
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