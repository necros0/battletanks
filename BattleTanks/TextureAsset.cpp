#include "TextureAsset.h"

#include <SFML/Graphics/Texture.hpp>

TextureAsset::TextureAsset(std::string path)
{
	mTexture.loadFromFile(path);
	mTexture.setRepeated(true);
}


TextureAsset::~TextureAsset(void)
{

}

const sf::Texture& TextureAsset::getTexture()
{
	return mTexture;
}