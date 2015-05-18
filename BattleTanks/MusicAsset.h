#pragma once

#include <string>

class MusicAsset
{
public:
	MusicAsset(std::string s) : path(s) {};
	~MusicAsset(void) {};

	std::string getPath() { return path; };

private:
	std::string path;
};

