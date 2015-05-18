#pragma once

#include "Vec2.h"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>
#include <string>
#include <iostream>

class Line;

// Warning: This class is full of hacks.

class DebugDraw
{
public:
	DebugDraw(void);
	~DebugDraw(void);

	static void draw(Vec2 org, std::string str);
	static void draw(Vec2 v1, Vec2 v2);
	static void draw(Vec2 v1, Vec2 v2, sf::Color colour);
	static void drawRaw(Vec2 v1, Vec2 v2);
	static void display(sf::RenderTarget& target);
	
	static void drawPermanent(Vec2 v1, Vec2 v2, sf::Color colour);

	static void clear();
	
	static DebugDraw& instance();

private:
	class IDebugDrawable
	{
	public:
		IDebugDrawable() {};
		virtual ~IDebugDrawable() {};

		virtual sf::Drawable* present(void) = 0;
	};

	class Line : public IDebugDrawable
	{
	public:
		Line() {};
		~Line() {};

		sf::Vertex v1;
		sf::Vertex v2;
		sf::Color c;

		virtual sf::Drawable* present(void);
	};

	class Text : public IDebugDrawable
	{
	public:
		Text() {};
		~Text() {};

		std::string s;
		Vec2 origin;

		virtual sf::Drawable* present(void);
	};

	std::vector<IDebugDrawable*> drawables;
	std::vector<sf::Drawable*> drawablesPermanent;

};

