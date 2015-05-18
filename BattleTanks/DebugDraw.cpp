#include "DebugDraw.h"
#include "Defs.h"
#include "Game.h"

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

DebugDraw::DebugDraw(void)
{
}


DebugDraw::~DebugDraw(void)
{
	this->clear();

	for(auto it = instance().drawablesPermanent.begin(); it != instance().drawablesPermanent.end(); it++)
	{
		delete(*it);
	}
	instance().drawablesPermanent.clear();
}

void DebugDraw::draw(Vec2 org, std::string str)
{
	Text* t = new Text();
	t->s = str;
	t->origin = org.toScreenVecAlt();

	instance().drawables.push_back(t);
}

void DebugDraw::draw(Vec2 v1, Vec2 v2)
{
	draw(v1, v2, sf::Color(255, 255, 255, 255));
}

void DebugDraw::draw(Vec2 v1, Vec2 v2, sf::Color colour)
{
	Line* line = new Line();
	line->v1 = v1.toScreenVec(); //Vec2(v1.x, WINDOW_HEIGHT - v1.y);
	line->v2 = v2.toScreenVec(); //Vec2(v2.x, WINDOW_HEIGHT - v2.y);
	line->c = colour;

	instance().drawables.push_back(line);
}

void DebugDraw::drawPermanent(Vec2 v1, Vec2 v2, sf::Color colour)
{
	sf::Vertex vtx1(v1.toScreenVec());
	sf::Vertex vtx2(v2.toScreenVec());

	vtx1.color = colour;
	vtx2.color = colour;

	sf::VertexArray* vArr = new sf::VertexArray(sf::Lines, 2);
	vArr->append(vtx1);
	vArr->append(vtx2);
	instance().drawablesPermanent.push_back(vArr);
}

void DebugDraw::drawRaw(Vec2 v1, Vec2 v2)
{
	Line* line = new Line();
	line->v1 = v1;
	line->v2 = v2;
	line->c = sf::Color(255, 255, 255, 255);
	instance().drawables.push_back(line);
}

void DebugDraw::display(sf::RenderTarget& target)
{
	for(auto it = instance().drawables.begin(); it != instance().drawables.end(); it++)
	{
		sf::Drawable* ptr = (*it)->present();
		target.draw(*ptr);
		delete(ptr); //this is the worst ever.
	}

	for(auto it = instance().drawablesPermanent.begin(); it != instance().drawablesPermanent.end(); it++)
	{
		target.draw(*(*it));
	}
}

void DebugDraw::clear()
{
	for(auto it = instance().drawables.begin(); it != instance().drawables.end(); it++)
	{
		delete(*it);
	}
	instance().drawables.clear();
}

DebugDraw& DebugDraw::instance()
{
	static DebugDraw instance;
	return instance;
}

sf::Drawable* DebugDraw::Line::present()
{
	sf::Vertex line[2];
	line[0] = this->v1;
	line[1] = this->v2;

	line[0].color = this->c;
	line[1].color = this->c;

	//haha ghetto.
	sf::VertexArray* vArr = new sf::VertexArray(sf::Lines, 2);
	vArr->append(line[0]);
	vArr->append(line[1]);
	return vArr;
}

sf::Drawable* DebugDraw::Text::present()
{
	sf::Text* t = new sf::Text(this->s, *Game::Font().get("resources/font/dejavusansmono.ttf"), 10);

	Vec2 displayOrigin = this->origin;
	displayOrigin.x = displayOrigin.x + (t->getLocalBounds().width * 0.5f);
	displayOrigin.y = displayOrigin.y + (t->getLocalBounds().height * 0.5f);
	t->setOrigin(displayOrigin);
	return t;
}
