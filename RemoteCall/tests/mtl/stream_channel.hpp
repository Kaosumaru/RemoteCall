#ifndef MTL_STREAM_CHANNEL
#define MTL_STREAM_CHANNEL


#include <utility>
#include <functional>
#include <vector>
#include <memory>

namespace mtl
{
	template<typename Stream>
	struct stream_channel_demuxer;

	template<typename Stream>
	struct stream_channel
	{
		using id_type = uint32_t;
		friend struct stream_channel_demuxer<Stream>;

		void send_stream(Stream& ss)
		{
			if (_parent)
				_parent->send_stream(_id, ss);
		}

		virtual void received_stream(Stream& ss) = 0;

	protected:
		void assign_id(stream_channel_demuxer<Stream> *parent, id_type id)
		{
			_parent = parent;
			_id = id;
		}

		id_type _id;
		stream_channel_demuxer<Stream> *_parent = nullptr;;
	};

	template<typename Stream>
	struct stream_channel_demuxer
	{
		friend struct stream_channel<Stream>;

		using id_type = uint32_t;
		using channel = stream_channel<Stream>;
		using channel_pointer = std::shared_ptr<channel>;

		void add_stream(const channel_pointer& channel)
		{
			channel->assign_id(this, channel.size());
			_channels.push_back(channel);
		}


	protected:
		virtual void send_stream(id_type id, Stream& ss) = 0;
		void receive_stream(Stream& ss)
		{
			id_type id;
			ss >> id;
			if (id < _channels.size())
				_channels[id]->received_stream(ss);
		}


		
		std::vector<channel_pointer> _channels;
	};

}

#endif