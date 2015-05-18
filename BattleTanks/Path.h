#pragma once

#include <list>
#include <vector>


class Sector;

class Path
{
public:
	Path();
	void add(Sector*);
	Sector* pop();
	Sector* peek();
	Sector* getCurrent();
	void draw(void);
private:
	std::list<Sector*> path;
	Sector* current;
};

//Stores A* related infos
class PathData
{
public:
	PathData() :
		g(0.0f),
		h(0.0f),
		cell(NULL),
		parent(NULL)
	{}

	PathData(Sector* c) :
		g(0.0f),
		h(0.0f),
		cell(c),
		parent(NULL)
	{}

	float g, h;
	Sector* cell;
	Sector* parent;

};

