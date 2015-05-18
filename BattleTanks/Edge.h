#pragma once

#include "Vec2.h"

class Sector;

class Edge
{
	friend class Sector;
	friend class Map;
	friend class PathFactory;
public:
	Edge();
	Edge(Vec2 p1, Vec2 p2);
	~Edge(void);

	float getLength();

private:
	bool mIsSolid;
	Vec2 mPoint1;
	Vec2 mPoint2;
	Sector* mAdjacentSector;

	bool getIntersection(const Edge&, Vec2&) const;
	bool checkOnSegment(Vec2 p, Vec2 q, Vec2 r) const;
	int getOrientation(Vec2 p, Vec2 q, Vec2 r) const;
	bool checkIntersection(const Edge& other) const;

	//Comparison operator, compare both elements
	bool operator == (const Edge& other)
	{
		return (   this->mPoint1 == other.mPoint1 && this->mPoint2 == other.mPoint2
				|| this->mPoint1 == other.mPoint2 && this->mPoint2 == other.mPoint1);
	}
};
