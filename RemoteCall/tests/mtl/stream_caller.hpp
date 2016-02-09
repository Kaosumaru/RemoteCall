#ifndef MTL_STREAM_CALLER
#define MTL_STREAM_CALLER

#include "function_traits.hpp"
#include <utility>

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
			using ReturnType = R;

			template<typename Callable>
			R Call(Stream &s, const Callable& f)
			{
				return CallImpl(s, f, std::index_sequence_for<Args...>());
			}

		protected:
			template<typename Callable, std::size_t... Is>
			R CallImpl(Stream &s, const Callable& f, std::index_sequence<Is...>)
			{
				std::tuple<typename std::decay<Args>::type...> args;
				auto fill_args = {
					((s >> std::get<Is>(args)), 0)...
				};

				return f(std::get<Is>(args)...);
			}
		};
	}


	template <typename Stream, typename Func>
	auto call_from_stream(Stream& stream, const Func& func) -> typename impl::stream_caller<Stream, Func>::ReturnType
	{
		return impl::stream_caller<Stream, Func>().Call(stream, func);
	}


}

#endif