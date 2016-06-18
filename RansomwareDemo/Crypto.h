#pragma once
#include <cryptopp/osrng.h>
#include <array>

#include "Network.h"

typedef std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH> AESKey;

AESKey generateKey(CryptoPP::AutoSeededRandomPool& rng);
void save_key(AESKey& aes_key, PublicKey& publicKey, CryptoPP::AutoSeededRandomPool& rng);
void encrypt_file(const std::string& filename, std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& aes_key, CryptoPP::AutoSeededRandomPool& rng);
void decrypt_file(const std::string& filename, std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& aes_key);
AESKey loadKey(PrivateKey& privateKey, CryptoPP::AutoSeededRandomPool& rng);