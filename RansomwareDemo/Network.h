#pragma once
#include <cryptopp/osrng.h>
#include <utility>
#include <cryptopp/eccrypto.h>

typedef CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey PrivateKey;
typedef CryptoPP::ECIES<CryptoPP::ECP>::PublicKey PublicKey;

std::pair<std::uint64_t, PublicKey> getPublicKey(CryptoPP::AutoSeededRandomPool& rng);
PrivateKey getPrivateKey(CryptoPP::AutoSeededRandomPool& rng, std::uint64_t clientId);