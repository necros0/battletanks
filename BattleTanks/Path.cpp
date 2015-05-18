#include "Path.h"
#include "Sector.h"
#include "DebugDraw.h"


Path::Path() :
	current(NULL)
{
}

void Path::add(Sector* c)
{
	path.push_front(c);
	current = c;
}

Sector* Path::pop()
{
	if (path.empty())
	{
		return current = NULL;
	}
	this->current = path.front();
	path.pop_front();
	return this->current;
}

Sector* Path::peek()
{
	if (path.empty() || *(path.begin()++) == NULL)
	{
		return NULL;
	}
	return *(path.begin()++);
}

Sector* Path::getCurrent()
{
	return this->current;
}

void Path::draw()
{
	Sector* lastSector = current;
	for(auto it = path.begin(); it != path.end(); it++)
	{
		DebugDraw::draw((*it)->getCenter(), lastSector->getCenter(), sf::Color(50, 180, 255, 255));
		lastSector = *it;
	}
}