#ifndef MTL_STREAM_CALLER_MAPPER_CHANNEL_PROXY
#define MTL_STREAM_CALLER_MAPPER_CHANNEL_PROXY
#include "../stream_channel.hpp"
#include <cassert>

namespace mtl
{





	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_channel_proxy : public stream_channel<Stream>
	{
	public:
		using request_id_type = uint32_t;
		using ProxyCallback = std::function<void(Stream& ss)>;

		using RequestMap = std::map<request_id_type, ProxyCallback>;

		Stream create_stream()
		{
			Stream arg;
			arg << _magic_number;
			arg << _magic_number;
			return arg;
		}

		void proxy_call(Stream& arg, const ProxyCallback& callback)
		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);


			auto id = _last_id++;

			//replacing header in stream - stream should be created by us in create_stream
			//so we are replacing first _magic_number
			{
				auto pp = arg.tellp();
				arg.seekp(0);
				arg << id;
				arg.seekp(pp);
			}

			_requests[id] = callback;
			send_stream(arg);
		}


		void received_stream(Stream& ss) override
		{
			request_id_type id;
			ss >> id;

			request_id_type number;
			ss >> number;
			if (number != _magic_number)
			{
				assert(false);
				return;
			}



			ProxyCallback callback;
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				auto it = _requests.find(id);
				if (it == _requests.end())
					return;
				callback = it->second;
				_requests.erase(it);
			}
			callback(ss);
		}

	protected:
		constexpr static request_id_type _magic_number = 0xFF003200;
		request_id_type _last_id = 1;
		std::recursive_mutex _mutex;
		RequestMap _requests;
	};


	template<typename Stream, typename FunctionID = std::string>
	class function_mapper_channel_proxy_receiver : public stream_channel<Stream>
	{
	public:
		using request_id_type = uint32_t;
		using ProxyCallback = std::function<void(Stream& ss)>;

		void received_stream(Stream& ss) override
		{
			request_id_type id;
			ss >> id;

			request_id_type number;
			ss >> number;
			if (number != _magic_number)
			{
				assert(false);
				return;
			}

			Stream out;

			out << id;
			out << _magic_number;

			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_mapper.call_from_stream_out(ss, out);
			}

			send_stream(out);
		}


		auto& mapper() { return _mapper; }
	protected:
		using mapper_type = function_mapper<Stream, FunctionID>;

		constexpr static request_id_type _magic_number = 0xFF003200;
		std::recursive_mutex _mutex;
		mapper_type _mapper;
	};

}

#endif