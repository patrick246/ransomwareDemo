#include <iostream>
#include <array>
#include <fstream>

#include "Crypto.h"
#include "FileWalker.h"



int main()
{
	CryptoPP::AutoSeededRandomPool rng;
	AESKey filemasterkey = generateKey(rng);
	save_key(filemasterkey);

	FileWalker fileWalker("D:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
		std::cout << filename << "\n";
	});

	fileWalker.start();

	std::cin.get();
}

