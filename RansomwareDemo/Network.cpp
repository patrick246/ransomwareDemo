#include "Network.h"
#include <asio.hpp>

#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/oids.h>
#include <string>
#include <sstream>
#include <iostream>

#include "Commands.h"
#include "PaymentException.h"

std::pair<std::uint64_t, PublicKey> getPublicKey(CryptoPP::AutoSeededRandomPool& rng)
{
	using namespace asio::ip;
	asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query("10.0.0.13", "1337");
	tcp::resolver::iterator endpointIt = resolver.resolve(query);

	tcp::socket socket(io_service);
	connect(socket, endpointIt);

	Commands command = Commands::GET_PUBLIC_KEY;
	asio::error_code error;
	socket.write_some(asio::buffer(reinterpret_cast<char*>(&command), sizeof(Commands)), error);
	if (error)
	{
		std::cerr << "Could not write command\n";
		std::cerr << error.value() << error.message() << "\n";
	}

	std::string received;
	for (;;)
	{
		std::array<char, 256> buffer;
		asio::error_code error;
		std::size_t bytes_read = socket.read_some(asio::buffer(buffer), error);
		received.append(buffer.data(), std::min(buffer.size(), bytes_read));
		if (error == asio::error::eof)
		{
			break;
		}
	}

	std::stringstream s(received);
	std::uint64_t clientId;
	s >> clientId;
	
	std::string base64publicKey;
	s >> base64publicKey;
	
	
	std::string decoded_public;
	CryptoPP::StringSource stringsource(base64publicKey, true, 
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded_public)
		)
	);

	CryptoPP::ECP::Point publicPoint;
	PublicKey pub;
	pub.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp112r1());
	pub.GetGroupParameters().GetCurve().DecodePoint(publicPoint, CryptoPP::StringSource(decoded_public, true), pub.GetGroupParameters().GetCurve().EncodedPointSize(true));
	pub.SetPublicElement(publicPoint);
	if (!pub.Validate(rng, 3))
	{
		std::cerr << "Could not validate key\n";
	}

	return std::make_pair(clientId, pub);
}

PrivateKey getPrivateKey(CryptoPP::AutoSeededRandomPool& rng, std::uint64_t clientId)
{
	using namespace asio::ip;
	asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query("10.0.0.13", "1337");
	tcp::resolver::iterator endpointIt = resolver.resolve(query);

	asio::error_code error;
	tcp::socket socket(io_service);
	connect(socket, endpointIt);

	Commands command = Commands::GET_PRIVATE_KEY;
	std::array<asio::const_buffer, 2> sendbuffers = {
		asio::buffer(reinterpret_cast<byte*>(&command), sizeof(command)),
		asio::buffer(reinterpret_cast<byte*>(&clientId), sizeof(clientId))
	};

	
	socket.write_some(sendbuffers, error);
	if (error)
	{
		std::cerr << "Could not write command\n";
		std::cerr << error.value() << error.message() << "\n";
	}

	std::string received;
	for (;;)
	{
		std::array<char, 256> buffer;
		std::size_t bytes_read = socket.read_some(asio::buffer(buffer), error);
		received.append(buffer.data(), std::min(buffer.size(), bytes_read));
		if (error == asio::error::eof)
		{
			break;
		}
	}

	if (received == "not paid")
	{
		throw PaymentException{};
	}

	std::stringstream s(received);
	std::string base64privateKey;
	s >> base64privateKey;


	std::string decoded_private;
	CryptoPP::StringSource stringsource(base64privateKey, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded_private)
		)
	);

	CryptoPP::Integer privateExp;
	privateExp.BERDecode(CryptoPP::StringSource(decoded_private, true));
	PrivateKey priv;
	priv.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp112r1());
	priv.SetPrivateExponent(privateExp);

	if (!priv.Validate(rng, 3))
	{
		std::cerr << "Could not validate key\n";
	}

	return priv;
}
