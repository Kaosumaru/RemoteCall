#ifndef MTL_REMOTE_ENDPOINT_ASYNC
#define MTL_REMOTE_ENDPOINT_ASYNC

#include <string>
#include "remote_endpoint.hpp"

namespace mtl
{
namespace remote
{
namespace async
{

	
	template<typename Stream, typename FunctionID = std::string, typename Acceptor = dummy_acceptor<Stream, FunctionID> >
	struct endpoint
	{
		using SenderFunction = std::function<void(Stream& stream)>;

		//receives incomming data, and returns response
		Stream HandleStream(Stream& stream)
		{
			return{};
		}




	};


}
}
}

#endif