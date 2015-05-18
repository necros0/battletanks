#pragma once

#include <SFML/Graphics/Texture.hpp>

class TextureAsset
{
public:
	TextureAsset(std::string path);
	~TextureAsset(void);

	const sf::Texture& getTexture();

private:
	sf::Texture mTexture;
};

