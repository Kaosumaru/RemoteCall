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


		enum class ResponseType : uint8_t
		{ 
			valid = 0, 
			std_exception, 
			generic_exception 
		};

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

			ResponseType type = ResponseType::valid;
			ss >> (uint8_t&)type;

			ProxyCallback callback;
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				auto it = _requests.find(id);
				if (it == _requests.end())
					return;
				callback = it->second;
				_requests.erase(it);
			}

			if (type == ResponseType::valid)
			{
				callback(ss);
				return;
			}

			std::string exc_message;
			ss >> exc_message;
			//TODO send this error to callback
			assert(false);
			
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
		using ResponseType = typename function_mapper_channel_proxy<Stream, FunctionID>::ResponseType;
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

			ResponseType type = ResponseType::valid;
			std::string exc_message;

			Stream out;
			try
			{
				out << id;
				out << _magic_number;
				out << (uint8_t)type;

				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_mapper.call_from_stream_out(ss, out);
			}
			catch (std::exception& exc)
			{
				type = ResponseType::std_exception;
				exc_message = exc.what();
			}
			catch (...)
			{
				type = ResponseType::generic_exception;
			}

			static_assert(sizeof(uint8_t) == sizeof(ResponseType), "ResponseType is not what we expect");

			

			if (type != ResponseType::valid)
			{
				Stream exc_out;

				exc_out << id;
				exc_out << _magic_number;
				exc_out << (uint8_t)type;
				exc_out << exc_message;
				send_stream(exc_out);
				return;
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