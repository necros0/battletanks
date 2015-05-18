#pragma once

#include "Path.h"

#include <list>
#include <map>

class Sector;

/**
 * This class is responsible for creating paths to distant goals
 * The expected return type for this factory is a Path which itself
 * containts a list of Sector*s.
 */
class PathFactory
{
public:
	PathFactory() {}

	Path* getPath(Sector*, Sector*);
private:
	Sector* getLowestF(std::list<Sector*>&, std::map<Sector*, PathData>&);
};

