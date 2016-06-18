#include "fs.h"
#include <Windows.h>

void createDir(const std::string & path)
{
	CreateDirectoryA(path.c_str(), nullptr);
}
