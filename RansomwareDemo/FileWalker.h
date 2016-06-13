#pragma once
#include <string>
#include <functional>

class FileWalker
{
private:
	std::string rootDir;
	std::function<void(const std::string&)> handler;

public:
	FileWalker(const std::string& rootDir, std::function<void(const std::string&)> handler);
	void start();

private:
	void walkDirectory(const std::string& rootDir);
};

