#ifndef MTL_REMOTE_ENDPOINT_TCP
#define MTL_REMOTE_ENDPOINT_TCP

#include <string>
#include "binary_stream.hpp"
#include "stream_caller.hpp"
#include "netLink/netLink.h"
#include "remote_endpoint.hpp"

namespace mtl
{
namespace remote
{
namespace tcp
{
	/*
	template<typename Stream, typename FunctionID = std::string>
	struct acceptor
	{
		using mapper = function_mapper< Stream, FunctionID >;
		using stream_context = context< mapper >;

		template<typename R>
		static std::future<R> accept_stream(Stream &ss)
		{
			auto &current = stream_context::current();
			return current.accept_stream(ss);
		}
	};

	template<typename Stream, typename FunctionID = std::string, typename Acceptor = >
	struct endpoint
	{
	public:
		endpoint()
		{
			
		}

		template<typename T>
		struct function
		{

		};

		template<typename R, typename... Args>
		struct function<R(Args...)>
		{
			function(const FunctionID& id) : _id(id) {}

			using ReturnType = decltype(Acceptor::template accept_stream<R>(Stream{}));

			ReturnType operator() (Args... args)
			{
				auto stream = arguments_to_stream<Stream>(_id, std::forward<Args>(args)...);
				return Acceptor::template accept_stream<R>(stream);
			}
		private:
			FunctionID _id;
		};

	protected:
		using mapper = function_mapper< Stream, FunctionID >;

		template<typename R>
		std::future<R> accept_stream(Stream &ss)
		{
			std::promise<R>  ret;
			auto f = ret.get_future();
			ret.set_value(current.call_from_stream<R>(ss));

			return f;
		}

		template<>
		std::future<void> accept_stream<void>(Stream &ss)
		{
			std::promise<R>  ret;
			auto f = ret.get_future();
			_mapper.call_from_stream_void(ss);
			ret.set_value();

			return f;
		}


		mapper _mapper;
	};
	*/
	/*
	void netlinkTest()
	{
		netLink::SocketManager socketManager;

		//Allocate a new socket and insert it into the SocketManager
		std::shared_ptr<netLink::Socket> socket = socketManager.newMsgPackSocket();

		//Define a callback, fired when a new client tries to connect
		socketManager.onConnectRequest = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> serverSocket, std::shared_ptr<netLink::Socket> clientSocket) {
			std::cout << "Accepted connection from " << clientSocket->hostRemote << ":" << clientSocket->portRemote << std::endl;

			//Accept all new connections
			return true;
		};

		//Define a callback, fired when a sockets state changes
		socketManager.onStatusChange = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, netLink::Socket::Status prev) {
			netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(socket.get());

			switch (socket->getStatus()) {
			case netLink::Socket::Status::READY:
				std::cout << "Connection got accepted at " << socket->hostRemote << ":" << socket->portRemote << std::endl;

				//Prepare a MsgPack encoded message
				msgPackSocket << MsgPack__Factory(MapHeader(2));
				msgPackSocket << MsgPack::Factory("type");
				msgPackSocket << MsgPack::Factory("post");
				msgPackSocket << MsgPack::Factory("message");
				msgPackSocket << MsgPack::Factory("Hello World!");
				break;
			case netLink::Socket::Status::NOT_CONNECTED:
				if (prev == netLink::Socket::Status::CONNECTING)
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
		socketManager.onReceiveMsgPack = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
			//hostRemote and portRemote are now set to the origin of the last received message
			std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << std::endl;

			// Parse the *element
			auto elementMap = dynamic_cast<MsgPack::Map*>(element.get());
			if (elementMap) { // The received element is a map
								// Iterate them sequentially
				std::cout << "Iterate them sequentially:" << std::endl;
				auto container = elementMap->getElementsMap();
				for (auto& pair : container)
					std::cout << pair.first << " : " << *pair.second << std::endl;
				// Or just access them directly
				if (elementMap->getLength() >= 2) {
					std::cout << "Access them directly:" << std::endl;
					std::cout << elementMap->getKey(0)->stdString() << " : " << *elementMap->getValue(0) << std::endl;
					std::cout << elementMap->getKey(1)->stdString() << " : " << *elementMap->getValue(1) << std::endl;
					// ... and so on
				}
			}
		};

		//Ask user for a nice IP address
		std::cout << "Enter a IP-Adress to connect to a sever or '*' to start a server:" << std::endl;
		while (socket->getStatus() == netLink::Socket::Status::NOT_CONNECTED) {
			try {
				std::cin >> socket->hostRemote;

				//Init socket as TCP server or client on port 3823
				if (socket->hostRemote == "*")
					socket->initAsTcpServer("127.0.0.1", 3823);
				else
					socket->initAsTcpClient(socket->hostRemote, 3823);
			}
			catch (netLink::Exception exc) {
				std::cout << "Address is already in use, please try again..." << std::endl;
			}
		}

		//Let the SocketManager poll from all sockets, events will be triggered here
		while (socket->getStatus() != netLink::Socket::Status::NOT_CONNECTED)
			socketManager.listen();

		std::cout << "Quit" << std::endl;
	}
	*/


}
}
}

#endif