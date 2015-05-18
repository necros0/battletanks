#pragma once

#include <SFML/Graphics/Font.hpp>

class FontAsset
{
public:
	FontAsset(void);
	~FontAsset(void);

	sf::Font& getFont();

private:
	sf::Font mFont;
};

