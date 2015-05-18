#pragma once

#include <string>

class ResourceLoader
{
public:
	ResourceLoader(void);
	~ResourceLoader(void);

	void init();

private:
	void loadDirectory(std::string path);
	void loadFile(std::string path);
};

