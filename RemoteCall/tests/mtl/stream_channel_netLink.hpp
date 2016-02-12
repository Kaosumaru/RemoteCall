#ifndef MTL_STREAM_CHANNEL_NETLINKsocketManager
#define MTL_STREAM_CHANNEL_NETLINKsocketManager


#include <utility>
#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <array>
#include "netLink/netLink.h"

namespace mtl
{
	namespace netLink
	{
		template<typename Stream>
		struct stream_sender : public stream_channel_demuxer<Stream>
		{
			using socket = ::netLink::Socket;
			using manager = ::netLink::SocketManager;


			stream_sender(const std::string& host, unsigned port, bool server = false)
			{
				_server = server;
				_socket = _manager.newMsgPackSocket();

				//Define a callback, fired when a new client tries to connect
				_manager.onConnectRequest = [](manager* manager, std::shared_ptr<socket> serverSocket, std::shared_ptr<socket> clientSocket) {
					std::cout << "Accepted connection from " << clientSocket->hostRemote << ":" << clientSocket->portRemote << std::endl;
					//Accept all new connections
					return true;
				};

				//Define a callback, fired when a sockets state changes
				_manager.onStatusChange = [](manager* manager, std::shared_ptr<socket> socket, socket::Status prev) {
					
					::netLink::MsgPackSocket& msgPackSocket = *static_cast<::netLink::MsgPackSocket*>(socket.get());

					switch (socket->getStatus()) {
					case ::netLink::Socket::Status::READY:
						std::cout << "Connection got accepted at " << socket->hostRemote << ":" << socket->portRemote << std::endl;
						break;
					case ::netLink::Socket::Status::NOT_CONNECTED:
						if (prev == ::netLink::Socket::Status::CONNECTING)
							std::cout << "Connecting to " << socket->hostRemote << ":" << socket->portRemote << std::endl << " failed";
						else
							std::cout << "Lost connection of " << socket->hostRemote << ":" << socket->portRemote << std::endl;
						break;
					default:
						std::cout << "Status of " << socket->hostRemote << ":" << socket->portRemote << " changed from " << prev << " to " << socket->getStatus() << std::endl;
						break;
					}
				};

				//Define a callback, fired when a socket receives data
				_manager.onReceiveMsgPack = [&](manager* manager, std::shared_ptr<socket> socket, std::unique_ptr<MsgPack::Element> element) {
					::netLink::MsgPackSocket& msgPackSocket = *static_cast<::netLink::MsgPackSocket*>(socket.get());
					auto guard = context<::netLink::MsgPackSocket>::lock(msgPackSocket);
					
					//hostRemote and portRemote are now set to the origin of the last received message
					std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << std::endl;

					// Parse the *element
					auto arr = dynamic_cast<MsgPack::Array*>(element.get());
					if (!arr)
						return;

					auto element_id = dynamic_cast<MsgPack::Number*>(arr->getEntry(0));
					auto element_stream = dynamic_cast<MsgPack::String*>(arr->getEntry(1));

					if (!element_id || !element_stream)
						return;

					id_type id = element_id->getValue<uint64_t>();
					Stream ss;
					ss.write( element_stream->getData(), element_stream->getLength() );
					
					receive_stream(id, ss);
				};


				_socket->hostRemote = host;
				if (_server)
					_socket->initAsTcpServer(host, port);
				else
					_socket->initAsTcpClient(host, port);
			}

			void start_listening()
			{
				_thread = std::make_unique<std::thread>([&]() 
				{
					while (_socket->getStatus() != ::netLink::Socket::Status::NOT_CONNECTED)
						_manager.listen();
				});
			}

			auto& streams() { return _streams; }
		protected:

			void send_stream(id_type id, Stream& ss) override
			{
				auto& msgPackSocket = !_server ? *static_cast<::netLink::MsgPackSocket*>(_socket.get()) : context<::netLink::MsgPackSocket>::current();

				msgPackSocket << MsgPack__Factory(ArrayHeader(2));
				msgPackSocket << MsgPack::Factory((uint64_t)id);
				msgPackSocket << MsgPack::Factory(ss.str());
			}
			
			bool    _server = false;
			std::shared_ptr<socket>  _socket;
			manager _manager;
			std::unique_ptr<std::thread> _thread;
		};
	}

}

#endif