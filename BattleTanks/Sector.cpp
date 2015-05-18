#include "Sector.h"
#include "Shape.h"
#include "DebugDraw.h"
#include "Edge.h"
#include "Game.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdexcept>
#include <sstream>

/*Sector::Sector(std::vector<Vec2> verts) :
	mVertices(verts)
{
	Vec2 mins(mVertices[0].x, mVertices[0].y);
	sf::ConvexShape s(mVertices.size());
	for(auto it = mVertices.begin(); it != mVertices.end(); it++)
	{
		s.setPoint(it - mVertices.begin(), (*it));
		if ((*it).x < mins.x)
			mins.x = (*it).x;
		if ((*it).y < mins.y)
			mins.y = (*it).y;
	}

	Vec2 scr(WINDOW_WIDTH, WINDOW_HEIGHT);
	s.setOrigin(mins);

	this->mShapes.push_back(s);
}*/

Sector::Sector(std::vector<Vec2> verts) :
	mShape(3), //Always 3 sides
	debugHighlight(false)
{
	if (verts.size() != 3)
		throw std::invalid_argument( "Not 3 verts used for Sector construction." );

	setup(verts);
}

Sector::~Sector(void)
{
}

void Sector::setup(std::vector<Vec2> verts)
{
	//Build edges connected by the vertices
	constructEdges(verts);
	mVertices = verts; //assign argument verts to the sector itself
					   //do this after constructEdges so we preserve the swapped (if needed) verts order

	this->mAbsmin = Vec2(mVertices[0].x, mVertices[0].y);
	this->mAbsmax = Vec2(mVertices[0].x, mVertices[0].y);
	for(auto it = mVertices.begin(); it != mVertices.end(); it++)
	{
		mShape.setPoint(it - mVertices.begin(), (*it));
		if ((*it).x < this->mAbsmin.x)
			this->mAbsmin.x = (*it).x;
		if ((*it).y < this->mAbsmin.y)
			this->mAbsmin.y = (*it).y;

		if ((*it).x > this->mAbsmax.x)
			this->mAbsmax.x = (*it).x;
		if ((*it).y > this->mAbsmax.y)
			this->mAbsmax.y = (*it).y;
	}


	//Reuse verts because we've already copied the original values to mVertices.
	for(auto it = verts.begin(); it != verts.end(); it++)
	{
		//These changes account for disparity between the way screen space and world space work
		(*it).x -= this->mAbsmin.x; //displace accordingly
		(*it).y -= this->mAbsmin.y; //displace accordingly
		//(*it).x *=  1; //reflect on x axis
		(*it).y *=  -1; //reflect on y axis
	}

	float width = this->mAbsmax.x - this->mAbsmin.x;
	float height = this->mAbsmax.y - this->mAbsmin.y;

	Vec2 absMinScreen = this->mAbsmin.toScreenVec();

	//mShape.setOrigin(this->mAbsmin.x * -1, this->mAbsmin.y - WINDOW_HEIGHT); //account for screen size
	mShape.setPosition(absMinScreen);
	mShape.setPoint(0, verts[0]);
	mShape.setPoint(1, verts[1]);
	mShape.setPoint(2, verts[2]);
	//mShape.setFillColor(sf::Color(0, 0, 0, 255));
	mShape.setTexture(&Game::Texture().get("resources/texture/ter_mud1.tga")->getTexture());
	mShape.setTextureRect(sf::IntRect(absMinScreen.x, absMinScreen.y, width, height));
	//mShape.setOutlineThickness(-1.0f);
	//mShape.setOutlineColor(sf::Color(150, 150, 150, 255));
}

//Used only during construction of the sector (or setVertex).  Checks that the vertices were sent in clockwise and flips if not.
void Sector::constructEdges(std::vector<Vec2>& verts)
{
	std::vector<Edge> testEdges = createEdges(verts); //create some test edges first

	// http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
	float sum = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sum = sum + (testEdges[i].mPoint2.x - testEdges[i].mPoint1.x) * (testEdges[i].mPoint2.y + testEdges[i].mPoint1.y);
	}
	//std::cout << "Sector sum: " << sum << std::endl;
	if (sum < 0) //counter-clockwise, invert vertex 2 and 3:
	{
		Vec2 temp = verts[2];
		verts[2] = verts[1]; //swap last two verts to make it clockwise
		verts[1] = temp;
		this->mEdges = createEdges(verts); //create edges again, in clockwise direction
		//std::cout << "Sector was counter-clockwise.  Vertices swapped." << std::endl;
	}
	else
		this->mEdges = testEdges; //these edges were ok, so just assign them to the object
}

std::vector<Edge> Sector::createEdges(std::vector<Vec2>& verts)
{
	if (verts.size() != 3)
		throw std::invalid_argument( "Not 3 verts used for createEdges." );

	std::vector<Edge> edges;
	edges.push_back(Edge(verts[0], verts[1]));
	edges.push_back(Edge(verts[1], verts[2]));
	edges.push_back(Edge(verts[2], verts[0]));

	return edges;
}


void Sector::draw(sf::RenderTarget& target)
{
	//if (debugHighlight)
	//	mShape.setFillColor(sf::Color(150, 100, 100, 255));
	//else
	//	mShape.setFillColor(sf::Color(100, 100, 100, 255));

	target.draw(mShape);
	
	for (int i = 0; i < 3; i++)
	{
		if (!mEdges[i].mIsSolid && Game::instance().demoMode == 0)
			continue;

		sf::Vertex v1(mEdges[i].mPoint1.toScreenVec());
		sf::Vertex v2(mEdges[i].mPoint2.toScreenVec());

		/*if (!mEdges[i].mIsSolid)
		{
			v1.color = sf::Color(140, 0, 0, 255);
			v2.color = sf::Color(140, 0, 0, 255);
		}
		else
		{*/
			v1.color = sf::Color(150, 150, 150, 255);
			v2.color = sf::Color(150, 150, 150, 255);
		//}

		sf::VertexArray vArr(sf::Lines, 2);
		vArr.append(v1);
		vArr.append(v2);
		target.draw(vArr);
	}

	/*
	Vec2 mid;
	for (int i = 0; i < 3; i++)
	{
		mid = mid + mVertices[i];
	}
	mid = mid * (1.0f/3.0f);


	for (int i = 0; i < 3; i++)
	{
		Vec2 dir = mVertices[i] - mid;
		dir = dir * 0.8f;
		std::stringstream ss;
		ss << i;
		DebugDraw::draw(mid + dir, ss.str());
	}
	*/
}

//Returns a collection which lets us return references (or nothing if no match found)
std::vector<Edge*> Sector::getEdges(Vec2 v1, Vec2 v2)
{
	std::vector<Edge*> result;
	for (auto it = mEdges.begin(); it != mEdges.end(); it++)
	{
		if ( ((*it).mPoint1 == v1 && (*it).mPoint2 == v2)
			|| ((*it).mPoint2 == v1 && (*it).mPoint1 == v2))
			result.push_back(&(*it));
	}
	return result; //return what you found
}

std::vector<Edge*> Sector::getEdges(Vec2 v)
{
	std::vector<Edge*> result;
	for (auto it = mEdges.begin(); it != mEdges.end(); it++)
	{
		if ((*it).mPoint1 == v || (*it).mPoint2 == v)
			result.push_back(&(*it));
	}
	return result;
}

// http://www.wikihow.com/Calculate-the-Area-of-a-Triangle
float Sector::getArea()
{
	float s = (mEdges[0].getLength() + mEdges[1].getLength() + mEdges[2].getLength()) / 2;
	return sqrt(s * (s - mEdges[0].getLength()) * (s - mEdges[1].getLength()) * (s - mEdges[2].getLength()));
}

void Sector::setHighlight(bool b)
{
	debugHighlight = b;
}

bool Sector::getCollision(Shape& shape, Collision& col)
{
	//Check if entity is within the bounding box (expanded to account for shape size)
	/*
	if (!(shape.newOrigin.x >= (this->mAbsmin.x - shape.radius - COLLISION_POSITION_EPSILON) && shape.newOrigin.x <= (this->mAbsmax.x + shape.radius + COLLISION_POSITION_EPSILON)
	   && shape.newOrigin.y >= (this->mAbsmin.y - shape.radius - COLLISION_POSITION_EPSILON) && shape.newOrigin.y <= (this->mAbsmax.y + shape.radius + COLLISION_POSITION_EPSILON)
	   ))
		return false;
	//*/


	//if (this->isInside(shape.oldOrigin, shape.radius))
	//	return false; //we're still inside the triangle, not colliding with walls

	//std::cout << "outside triangle" << std::endl;

	bool result = false;
	for (int i = 0; i < 3; i++)
	{
		if (!mEdges[i].mIsSolid) //can't collide on non-solid edge
			continue;

		if (getSign(mEdges[i].mPoint1, mEdges[i].mPoint2, shape.newOrigin, shape.radius) < 0)
			continue;

		//If moving away from the edge normal (or parallel to it), then we can't be colliding with it
		//if ( (mEdges[i].mPoint2 - mEdges[i].mPoint1).getNormal() * shape.velocity >= 0)
		//	continue;
		
		//if (!mEdges[i].checkIntersection(Edge(shape.oldOrigin, shape.newOrigin)))
		//	continue;

		Collision checkCollision = col;
		if (!getIntersection(mEdges[i].mPoint1, mEdges[i].mPoint2, shape.oldOrigin, shape.newOrigin, shape, checkCollision))
		{
			//not solid, vertex isn't solid either and don't collide on vertices with tiny objects (eg: lasers)
			if (!mEdges[i].mIsSolid)// || shape.radius <= 1.0f) 
				continue;

			//We didn't collide against an edge, but we may still be colliding with a corner (eg: vertex)
			if (!isCollidingWithVertex(mEdges[i].mPoint1, shape, checkCollision)
				&& !isCollidingWithVertex(mEdges[i].mPoint2, shape, checkCollision))
				continue;
		}



		//DebugDraw::draw(checkCollision.origin, checkCollision.origin + (mEdges[i].mPoint2 - mEdges[i].mPoint1).getNormal(), sf::Color(255, 0, 0, 255));

		if (!result || checkCollision.minimumTranslation.vlen() < col.minimumTranslation.vlen())
			col = checkCollision;

		result = true; //if this doesn't return true, we've screwed up...
	}

	//std::cout << "Collision check complete with result: " << (result ? "true" : "false") << std::endl;
	//DebugDraw::draw(col.origin, col.origin + col.normal);

	return result;
}

bool Sector::isCollidingWithVertex(Vec2 p, Shape& shape, Collision& checkCollision)
{
	/*
	Vec2 vertexToObject = (shape.newOrigin - p);
	float len = vertexToObject.vlen();
	if (len <= shape.radius - COLLISION_POSITION_EPSILON) //added epsilon offset....... seemed to stop getting stuck.  not sure why though
	{
		checkCollision.normal = -vertexToObject;//vertexNormal;
		checkCollision.minimumTranslation = vertexToObject.normalize() * (shape.radius - len - COLLISION_POSITION_EPSILON);
		checkCollision.origin = p;
		//DebugDraw::draw(p, p + checkCollision.normal, sf::Color(0, 200, 0, 255));
		return true;
	}

	return false;
	//*/

	Shape vertex(0.5f, p, Vec2(), 0);
	return shape.getCollision(vertex, checkCollision);
}

//Get side shape is on via expansion
float Sector::getSign(Vec2 v1, Vec2 v2, Vec2 p, float radius) const
{
	p = p - (v2 - v1).getNormal().normalize() * radius;

	// http://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
	return (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x);
}

// Alexander Hristov
// http://www.ahristov.com/tutorial/geometry-games/intersection-lines.html
bool Sector::getIntersection(Vec2 v1, Vec2 v2, Vec2 p1, Vec2 p2, Shape& shape, Collision& col)
{
	//Extend edge outwards by radius amount
	//Vec2 vDir = (v2 - v1).normalize();
	//v1 = v1 - vDir * shape.radius;
	//v2 = v2 + vDir * shape.radius;

	//DebugDraw::draw(v1, v2, sf::Color(0, 255, 255, 255));

	Vec2 normal = (v2 - v1).getNormal();
	Vec2 normal_N = normal.normalize();
	p1 = p1 - normal_N * shape.radius; //offset shape origin to account for radius
	p2 = p2 - normal_N * shape.radius;

	//DebugDraw::draw(p1, p2, sf::Color(255, 0, 0, 255));

	Edge e1(v1, v2);
	Edge e2(p1 + normal_N * 0.2f, p2 - normal_N * 0.2f); //add 0.4 BACKWARDS so give the movement vector some thickness, otherwise has a tendency to 'fall through' a wall since the line is so short.
	if (!e1.checkIntersection(e2))
		return false;

	float d = (v1.x-v2.x)*(p1.y-p2.y) - (v1.y-v2.y)*(p1.x-p2.x);
	if (fabsf(d) < 0.01f) //allow some play on what is considered 0, otherwise we can get insanely huge numbers if d is small
		return false;

	float xi = ((p1.x-p2.x)*(v1.x*v2.y-v1.y*v2.x)-(v1.x-v2.x)*(p1.x*p2.y-p1.y*p2.x))/d;
	float yi = ((p1.y-p2.y)*(v1.x*v2.y-v1.y*v2.x)-(v1.y-v2.y)*(p1.x*p2.y-p1.y*p2.x))/d;

	col.origin = Vec2(xi,yi);
	col.normal = normal;
	col.minimumTranslation = (col.origin - p1);

	//DebugDraw::draw(shape.oldOrigin, shape.oldOrigin + col.normal.normalize() * 100.0f, sf::Color(0, 0, 255, 255));

	//floating point inaccuracy with small numbers and sometimes result is crazy minimum translation vectors.
	//This is a workaround that stops the object from moving off the screen or worse
	//*
	if (col.minimumTranslation.vlen() > shape.velocity.vlen() * shape.delta)
	{
		//std::cout << "WARNING: huge minimum translation" << std::endl;
		col.minimumTranslation = Vec2();
		col.origin = shape.oldOrigin;
		col.normal = shape.velocity * -1.0f;
		//return false;
	}
	//*/
	return true;
}



bool Sector::isInside(Vec2 p, float expansion) const
{
	return (getSign(mVertices[0], mVertices[1], p, expansion) < 0
		 && getSign(mVertices[1], mVertices[2], p, expansion) < 0
		 && getSign(mVertices[2], mVertices[0], p, expansion) < 0);
}

//NOTE: Is this bugged???
// barycentric method...
// http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
/*
{
	float area = 1/2*(-mVertices[1].y*mVertices[2].x + mVertices[0].y*(-mVertices[1].x + mVertices[2].x) + mVertices[0].x*(mVertices[1].y - mVertices[2].y) + mVertices[1].x*mVertices[2].y);
	float s = 1/(2*area)*(mVertices[0].y*mVertices[2].x - mVertices[0].x*mVertices[2].y + (mVertices[2].y - mVertices[0].y)*p.x + (mVertices[0].x - mVertices[2].x)*p.y);
	float t = 1/(2*area)*(mVertices[0].x*mVertices[1].y - mVertices[0].y*mVertices[1].x + (mVertices[0].y - mVertices[1].y)*p.x + (mVertices[1].x - mVertices[0].x)*p.y);
	return !((0 <= s && s <= 1) && (0 <= t && t <= 1) && (s + t <= 1));
}
*/


void Sector::setVertex(int index, Vec2 v)
{
	std::vector<Vec2> verts = this->mVertices;
	verts[index] = v; //make the change
	setup(verts); //and rerun the setup method
}

float Sector::getDistanceTo(Sector& oSector)
{
	//Vec2 tMid = (this->mAbsmin + this->mAbsmax) * 0.5f;
	//Vec2 oMid = (oSector.mAbsmin + oSector.mAbsmax) * 0.5f;

	return (this->getCenter() + oSector.getCenter()).vlen();
}

Vec2 Sector::getCenter()
{
	return (mVertices[0] + mVertices[1] + mVertices[2]) * (1/3.0f);
}