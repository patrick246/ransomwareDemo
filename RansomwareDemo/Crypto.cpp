#include "Crypto.h"
#include <cryptopp/files.h>
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/eccrypto.h>

#include "Network.h"

AESKey generateKey(CryptoPP::AutoSeededRandomPool& rng)
{
	AESKey key;
	rng.GenerateBlock(key.data(), key.size());
	return key;
}

void save_key(AESKey& aes_key, PublicKey& publicKey, CryptoPP::AutoSeededRandomPool& rng)
{
	std::ofstream filemasterkey("filemaster.key", std::ios::binary);
	CryptoPP::ECIES<CryptoPP::ECP>::Encryptor e(publicKey);

	CryptoPP::ArraySource(aes_key.data(), aes_key.size(), true,
		new CryptoPP::PK_EncryptorFilter(rng, e,
			new CryptoPP::FileSink(filemasterkey)
		)
	);
}

AESKey loadKey(PrivateKey& privateKey, CryptoPP::AutoSeededRandomPool& rng)
{
	std::ifstream filemasterkey("filemaster.key", std::ios::binary);
	CryptoPP::ECIES<CryptoPP::ECP>::Decryptor d(privateKey);

	AESKey aes_key;

	CryptoPP::FileSource(filemasterkey, true,
		new CryptoPP::PK_DecryptorFilter(rng, d,
			new CryptoPP::ArraySink(aes_key.data(), aes_key.size())
		)
	);

	return aes_key;
}

void encrypt_file(const std::string& filename, std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& aes_key, CryptoPP::AutoSeededRandomPool& rng)
{
	// Generate IV
	std::array<byte, CryptoPP::AES::BLOCKSIZE> iv;
	rng.GenerateBlock(iv.data(), iv.size());

	std::ifstream infile(filename, std::ios::binary);
	std::ofstream outfile(filename + ".enc", std::ios::binary);

	// Save IV
	outfile.write(reinterpret_cast<const char*>(iv.data()), iv.size());

	CryptoPP::GCM<CryptoPP::AES>::Encryption e;
	e.SetKeyWithIV(aes_key.data(), aes_key.size(), iv.data(), iv.size());

	CryptoPP::FileSource(infile, true,
		new CryptoPP::AuthenticatedEncryptionFilter(e,
			new CryptoPP::FileSink(outfile)
			)
		);
}

void decrypt_file(const std::string& filename, std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& aes_key)
{
	std::ifstream infile(filename, std::ios::binary);
	std::ofstream outfile(filename + ".dec", std::ios::binary);

	std::array<byte, CryptoPP::AES::BLOCKSIZE> iv;
	infile.read(reinterpret_cast<char*>(iv.data()), iv.size());

	CryptoPP::GCM<CryptoPP::AES>::Decryption d;
	d.SetKeyWithIV(aes_key.data(), aes_key.size(), iv.data(), iv.size());

	CryptoPP::FileSource(infile, true,
		new CryptoPP::AuthenticatedDecryptionFilter(d,
			new CryptoPP::FileSink(outfile)
		)
	);
}