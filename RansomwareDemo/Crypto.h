#pragma once
#include <cryptopp/osrng.h>
#include <array>

typedef std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH> AESKey;

AESKey generateKey(CryptoPP::AutoSeededRandomPool& rng);
void save_key(AESKey& aes_key);
void encrypt_file(const std::string& filename, std::array<byte, CryptoPP::AES::DEFAULT_KEYLENGTH>& aes_key, CryptoPP::AutoSeededRandomPool& rng);