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
				return [=](Stream& ss)
				{
					using c = context<R, function_mapper>;
					c::current() = mtl::call_from_stream(ss, function);
				};
			}
		};

		template<typename T>
		struct LambdaCreator<void,T>
		{
			static auto create(T& function)
			{
				return [=](Stream& ss)
				{
					mtl::call_from_stream(ss, function);
				};
			}
		};

	public:
		using Function = std::function<void(Stream& ss)>;
		using Map = std::unordered_map<FunctionID, Function>;

		template<typename T>
		void add_function(const FunctionID& id, const T& function)
		{
			using traits = function_traits<T>;
			auto& f = _functions[id];
			if (!f)
				throw std::domain_error("Duplicate entry");
			
			f = LambdaCreator<typename traits::return_type, T >::create(function);
		}

		template<typename R>
		void call_from_stream(Stream& ss, const std::function<void(R&&)>& callback = nullptr)
		{
			FunctionID id;
			ss >> id;
			auto it = _functions.find(id);
			if (it == _functions.end())
				throw std::domain_error("Unknown function");

			auto &f = it->second;

			R r;
			{
				using c = context<R, function_mapper>;
				auto l = c::lock(r);
				f(ss);
			}
			if (callback)
				callback(std::move(r));
		}
		
		
		void call_from_stream_void(Stream& ss, const std::function<void(void)>& callback = nullptr)
		{
			FunctionID id;
			ss >> id;
			auto it = _functions.find(id);
			if (it == _functions.end())
				return;
			auto &f = it->second;
			f(ss);
			if (callback)
				callback();
		}
	protected:


		Map _functions;
	};

}

#endif