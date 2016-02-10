#ifndef MTL_STREAM_CALLER_MAPPER
#define MTL_STREAM_CALLER_MAPPER

#include <functional>
#include <map>
#include <unordered_map>
#include "stream_caller.hpp"
#include "context.hpp"

namespace mtl
{
	template<typename Stream, typename FunctionID = std::string>
	class function_mapper
	{
	private:
		template<typename R, typename T>
		struct LambdaCreator
		{
			static auto create(T& function)
			{
				return [=](Stream& ss, Stream& out)
				{
					using c = context<R, function_mapper>;
					out << mtl::call_from_stream(ss, function);
				};
			}
		};

		template<typename T>
		struct LambdaCreator<void,T>
		{
			static auto create(T& function)
			{
				return [=](Stream& ss, Stream& out)
				{
					mtl::call_from_stream(ss, function);
				};
			}
		};

	public:
		using Function = std::function<void(Stream& ss, Stream& out)>;
		using Map = std::unordered_map<FunctionID, Function>;

		template<typename T>
		void add_function(const FunctionID& id, const T& function)
		{
			using traits = function_traits<T>;
			_functions[id] = LambdaCreator<typename traits::return_type, T >::create(function);
		}

		void call_from_stream_out(Stream& ss, Stream& out)
		{
			FunctionID id;
			ss >> id;
			auto it = _functions.find(id);
			if (it == _functions.end())
				return;
			auto &f = it->second;
			f(ss, out);
		}


		template<typename R>
		R call_from_stream(Stream& ss)
		{
			Stream out;
			call_from_stream_out(ss, out);

			R r;
			out >> r;
			return r;
		}
		
		
		void call_from_stream_void(Stream& ss)
		{
			Stream out;
			call_from_stream_out(ss, out);
		}
	protected:


		Map _functions;
	};

}

#endif