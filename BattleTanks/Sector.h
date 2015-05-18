#pragma once

#include "Defs.h"
#include "Vec2.h"
#include "Collision.h"
#include "Edge.h"

#include <SFML/Graphics.hpp>

#include <vector>

class Shape;

class Sector
{
	friend class Map;
	friend class PathFactory;
public:
	Sector(std::vector<Vec2> verts);
	~Sector(void);

	void draw(sf::RenderTarget& target);
	bool getCollision(Shape& shape, Collision& col);
	bool isInside(Vec2 p, float expansion) const; //is point inside sector

	void setHighlight(bool b);

	float getDistanceTo(Sector&);
	//Get a vector of the center of the triangle
	Vec2 getCenter();

private:
	sf::ConvexShape mShape; //the cached convex shape we'll be drawing
	std::vector<Vec2> mVertices;		//The vertices of this sector
	Vec2 mAbsmin;
	Vec2 mAbsmax;
	std::vector<Edge> mEdges;		//Cached edges linking groups of 2 vertices into 3 edges

	bool debugHighlight;

	float getSign(Vec2, Vec2, Vec2, float) const;

	bool getIntersection(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Shape& shape, Collision& col);
	bool isCollidingWithVertex(Vec2 p, Shape& shape, Collision& checkCollision);

	//Get edges that match the coordinates (should always just return one or none)
	std::vector<Edge*> getEdges(Vec2 v1, Vec2 v2);

	//Get edges that have one of their vertices matching
	std::vector<Edge*> getEdges(Vec2 v);

	//Gets the area of the sector
	float getArea();

	//Sets a vertex to a new position.  NECESSARY because shape (graphical representation) and edges need to be updated.
	void setVertex(int, Vec2);

	//Used by constructor and setVertex.  Does all the setup to link edges and create the graphical representation
	void setup(std::vector<Vec2> verts);

	void constructEdges(std::vector<Vec2>& verts);
	std::vector<Edge> createEdges(std::vector<Vec2>& verts);
};

