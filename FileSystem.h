#pragma once

#include <vector>
#include <string>

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	std::vector<char> ReadFile(const std::string& i_filename);
};

