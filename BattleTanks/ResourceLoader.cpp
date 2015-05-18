#include "ResourceLoader.h"
#include "dirent.h"
#include "SoundAsset.h"
#include "Game.h"
#include "MusicAsset.h"
#include "TextureAsset.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <SFML/Graphics/Font.hpp>

ResourceLoader::ResourceLoader(void)
{
}


ResourceLoader::~ResourceLoader(void)
{
}

void ResourceLoader::init()
{
	std::cout << "Loading resources..." << std::endl;
	loadDirectory("resources");
}

// Basic code from:
// http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void ResourceLoader::loadDirectory(std::string path)
{

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) == NULL) 
	{
		perror ("Error reading from directory");
		return;
	}

	/* print all the files and directories within directory */
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] == '.')
			continue;

		if (ent->d_type == DT_DIR)
			loadDirectory((path + "/" + ent->d_name).c_str());
		else
			loadFile(path + "/" + ent->d_name);
	}
	closedir (dir);
}

void ResourceLoader::loadFile(std::string path)
{
	printf ("Loading file: '%s'\n", path.c_str());

	unsigned int extIndex = path.rfind(".", path.length() - 1); //find last . for extension
	
	if (extIndex == std::string::npos) //if no . at all.... just stop.
	{
		std::cerr << "Couldn't read file: " << path << std::endl;
		return;
	}

	std::string extension = path.substr(extIndex + 1, path.length() - 1); //get the extension string (ignore .)
	
	// Store always as lower case
	// http://notfaq.wordpress.com/2007/08/04/cc-convert-string-to-upperlower-case/

	std::string alias = path;
	std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);


	//std::cout << "extension for file: " << extension << std::endl;

	if (extension == "wav" || extension == "WAV") // sound (non-streaming)
	{
		SoundAsset* sound = new SoundAsset(path);
		Game::Sound().add(alias, sound);
		printf ("Sound '%s' loaded.\n", alias.c_str());
	}
	else if (extension == "ttf" || extension == "TTF")
	{

		sf::Font* font = new sf::Font();
		if (font->loadFromFile(path))
		{
			Game::Font().add(alias, font);
			printf ("Font '%s' loaded.\n", alias.c_str());
		}
		else
			std::cerr << "Failed to load font '" << path << "'" << std::endl;
	}
	else if (extension == "ogg" || extension == "OGG")
	{
		MusicAsset* music = new MusicAsset(path);
		Game::Music().add(alias, music);
		std::cout << "Music " << alias << " loaded." << std::endl;

		//FIXME: Centralize ALL audio assets into one.
		SoundAsset* sound = new SoundAsset(path);
		Game::Sound().add(alias, sound);
		printf ("Sound '%s' loaded.\n", alias.c_str());
	}
	else if (   extension == "png" || extension == "PNG"
			 || extension == "tga" || extension == "TGA")
	{
		TextureAsset* texture = new TextureAsset(path);
		Game::Texture().add(alias, texture);
		std::cout << "Texture " << alias << " loaded." << std::endl;
	}
	else
	{
		std::cout << "Unknown file format, ignoring." << std::endl;
	}

}