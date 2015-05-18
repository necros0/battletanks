#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

class MouseListener;

class MenuItem
{
public:
	MenuItem(MouseListener*);
	virtual ~MenuItem(void);

	virtual void draw(sf::RenderTarget& target) = 0;
	void setActive(bool b);

protected:
	bool mIsActive;

private:
	MouseListener* mouseListener;
};

