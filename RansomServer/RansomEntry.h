#pragma once
#include <cstdint>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/oids.h>
#include <fstream>
#include <cryptopp/files.h>

class RansomEntry
{
public:
	typedef CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey PrivateKey;
	typedef CryptoPP::ECIES<CryptoPP::ECP>::PublicKey PublicKey;

public:
	static RansomEntry create(CryptoPP::AutoSeededRandomPool& rng);
	static RansomEntry readFromFile(std::uint64_t clientId, std::ifstream& privateFile, std::ifstream& publicFile, std::ifstream& infoFile);

	void saveToFile(std::ofstream& privateFile, std::ofstream& publicFile, std::ofstream& infoFile);
	PrivateKey& privateKey()
	{
		return privateKey_;
	}
	PublicKey& publicKey()
	{
		return publicKey_;
	}
	std::uint64_t clientId()
	{
		return clientId_;
	}
	bool hasPaid() 
	{
		return hasPaid_;
	}
	void setPaymentStatus(bool status)
	{
		hasPaid_ = status;
	}

private:
	std::uint64_t clientId_;
	PrivateKey privateKey_;
	PublicKey publicKey_;
	bool hasPaid_;

private:
	RansomEntry(PublicKey publicKey, PrivateKey privateKey, std::uint64_t clientId, bool paid);
};

