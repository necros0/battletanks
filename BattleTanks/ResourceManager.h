#pragma once

#include "SoundAsset.h"
#include "MusicAsset.h"
#include "TextureAsset.h"

#include <string>
#include <map>
#include <iostream>
#include <algorithm>

template<typename T> class ResourceManager
{
private:
	std::map<std::string, T> resources;

public:
	ResourceManager(void) {};
	~ResourceManager(void);

	void clear()
	{
		for (auto it = resources.begin(); it != resources.end(); it++)
		{
			delete(it);
		}
		resources.clear();
	}
	
	void add(std::string alias, T asset);
	//void remove(T);

	T get(std::string);
};

template<typename T> void ResourceManager<T>::add(std::string alias, T asset)
{
	//this->resources.insert(std::pair<std::string, T>(alias, asset)); 

	if (this->resources.find(alias) != this->resources.end())
	{
		std::cerr << "Tried to add a resource that already exists! (" << alias << ")" << std::endl;
		return;
	}		
	this->resources[alias] = asset;
};

template<typename T> T ResourceManager<T>::get(std::string alias)
{
	//Aliases are always lowercase
	std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);

	if (this->resources.find(alias) != this->resources.end())
		return this->resources[alias];

	for (auto it = resources.begin(); it != resources.end(); it++)
	{
		std::cerr << "Resource in collection: " << (*it).first << std::endl;
	}

	std::cerr << "Couldn't find resource with alias '" << alias << "'" << std::endl;
	return NULL;
};

template<typename T> ResourceManager<T>::~ResourceManager(void)
{
	if (this->resources.size() > 0)
	{
		for (auto it = this->resources.begin(); it != this->resources.end(); it++)
		{
			delete((*it).second); //delete all assets objects
		}
		this->resources.clear();
	}

	std::cout << __FUNCTION__ << " has been called, memory freed." << std::endl;
};