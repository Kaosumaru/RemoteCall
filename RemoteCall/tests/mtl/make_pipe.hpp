#ifndef MTL_MAKE_PIPE
#define MTL_MAKE_PIPE

#include "function_traits.hpp"

namespace mtl
{
	template<typename Func>
	auto make_pipe(Func f)
	{
		using Traits = function_traits<Func>;
		static_assert(Traits::arity == 1, "Arity of provided function should be 1");

		using ArgType = typename Traits::argument<0>::type;
		return [=](ArgType arg) -> Traits::return_type
		{
			return f(std::forward<ArgType>(arg));
		};
	}


	template<typename Func, typename ...Funcs>
	auto make_pipe(Func f, Funcs... funcs)
	{
		using Traits = function_traits<Func>;
		static_assert(Traits::arity == 1, "Arity of provided function should be 1");

		using ArgType = typename Traits::argument<0>::type;
		return [=](ArgType arg) -> Traits::return_type
		{
			return make_pipe(funcs...)(
				f(std::forward<ArgType>(arg))
				);
		};
	}
}

#endif