#ifndef MTL_STREAM_CALLER_MAPPER
#define MTL_STREAM_CALLER_MAPPER

#include <functional>
#include <map>
#include <unordered_map>
#include "stream_caller.hpp"


namespace mtl
{
	template<typename Stream, typename FunctionID = std::string>
	class function_mapper
	{
	public:
		using Function = std::function<void(Stream& ss)>;
		using Map = std::unordered_map<FunctionID, Function>;

		template<typename T>
		void add_function(const FunctionID& id, const T& function)
		{
			_functions[id] = [=](Stream& ss)
			{
				auto r = mtl::call_from_stream(ss, function);
			};
		}

		void call_from_stream(Stream& ss)
		{
			FunctionID id;
			ss >> id;
			auto it = _functions.find(id);
			if (it == _functions.end())
				return;
			auto &f = it->second;
			f(ss);
		}
	protected:


		Map _functions;
	};

}

#endif