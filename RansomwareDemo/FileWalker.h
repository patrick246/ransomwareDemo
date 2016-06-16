#pragma once
#include <string>
#include <functional>
#include <vector>

class FileWalker
{
private:
	std::string rootDir;
	std::function<void(const std::string&)> handler;
	std::vector<std::string> extensionFilter;

public:
	FileWalker(const std::string& rootDir, std::function<void(const std::string&)> handler);
	void addExtensions(const std::vector<std::string>& extensions);
	void addExtension(const std::string& extension);
	void start();

private:
	void walkDirectory(const std::string& rootDir);
	bool matchesExtension(const std::string& filename);
};

bool ends_with(std::string const & value, std::string const & ending);