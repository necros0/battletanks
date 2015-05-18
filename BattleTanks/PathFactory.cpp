#include "PathFactory.h"
#include "Path.h"
#include "Game.h"
#include "Sector.h"
#include "Map.h"
#include "Edge.h"
#include "DebugDraw.h"

#include <algorithm>
#include <list>
#include <vector>
#include <map>
#include <iostream>

Sector* PathFactory::getLowestF(std::list<Sector*> &l, std::map<Sector*, PathData> &sectorData)
{
	Sector* lowestFNode = NULL;
	float currentF = 99999.0f; //huge number to start off with

	for (auto it = l.begin(); it != l.end(); ++it)
	{
		if (sectorData[*it].g + sectorData[*it].h < currentF) //compare the currentF value to the node in question.
		{ //if it is smaller (ie: it's path is potentially better than the current choice)
			currentF = sectorData[*it].g + sectorData[*it].h; //set currentF to this node's F
			lowestFNode = *it; //and remember it
		}
	}
	return lowestFNode; //return the node that we found had the smallest F
}

//A* from here: http://www.policyalmanac.org/games/aStarTutorial.htm
Path* PathFactory::getPath(Sector* start, Sector* goal)
{
	if (!start)
		std::cerr << "getPath() called with null start sector*.  Pathing aborted." << std::endl;

	if (!goal)
		std::cerr << "getPath() called with null goal sector*.  Pathing aborted." << std::endl;

	if (!start || !goal)
		return NULL;

	//Get reference to the level array
	Map &map = Game::instance().getMap();
	std::map<Sector*, PathData> sectorData;

	std::list<Sector*> openList; //nodes we haven't checked yet
	std::list<Sector*> closedList; //nodes we've checked
	Sector *sNode, *cNode, *gNode, *adjNode;
	float adjNode_newG;

	//sNode: start node
	//cNode: current node
	//gNode: goal node

	sNode = cNode = start; //start with the node the AI is on
	gNode = goal;

	if (cNode == gNode) //already there.
		return NULL;

	sectorData[cNode].g = 0.0f; //get the distance from the current position (0 because it's the current position... but we need to start it off)
	sectorData[cNode].h = cNode->getDistanceTo(*gNode); //'heuristic' -> find the distance to the target in cell indices
	//FIXME: Try to come up with a better way to calculate this thing... doesn't account for visibility from cNode to goal or anything.

	openList.push_back(cNode); //start off with the current node that the AI is on and add it to the openList
	sectorData[cNode].parent = NULL; //this will be null since it is the start
	while (!openList.empty())
	{
		//Start pathing...
		cNode = getLowestF(openList, sectorData); //get the next lowest node
		
		openList.remove(cNode); //remove node from openList...
		closedList.push_back(cNode); //...and add to closed list.

		if (cNode == gNode) //we've found the goal node!
		{
			/**
			 * Create a path object on the heap and fill it with the nodes
			 * Remember to delete this thing when you're done!
			 */
			Path* path = new Path();
			path->add(cNode);
			/**
			 * We trace backward from the goal node following the parent links until we get to the node
			 * that links to the node the AI is on.  This builds the path to the goal from the start point.
			 */
			//std::cout << "Path: ";
			//std::cout << cNode << "<";
			while (sectorData[cNode].parent != sNode)
			{
				cNode = sectorData[cNode].parent; //iterate backwards through the list.
				//std::cout << cNode->toString() << "<";
				path->add(cNode);
			}
			//std::cout << " Start" << std::endl;

			return path; //return the path (remember to delete it later!)
		}

		// Search all nodes connected to the current node (cNode) and do appropriate actions.
		for(int eIndex = 0; eIndex < 3; eIndex++)
		{
			//Look for adjacent connections
			adjNode = cNode->mEdges[eIndex].mAdjacentSector;

			if (!adjNode) //node does not exist at all!
				continue; //This is the edge of the map, do nothing and keep checking other directions
//DebugDraw::draw(cNode->getCenter(), adjNode->getCenter());
			//if (!adjNode->isCellOpen() && adjNode != gNode) //don't try to path through an occupied cell
			//	continue;

			/**
			 * Found a node adjacent to the current node.  Now we have to check if it's in either open or closed list.
			 * and decide what to do.
			 */
			adjNode_newG = sectorData[cNode].g + adjNode->getDistanceTo(*cNode); //accumulate g value as we move along

			if (std::find(closedList.begin(), closedList.end(), adjNode) != closedList.end() ||
				std::find(openList.begin(), openList.end(), adjNode) != openList.end()) //already in closed or open list
			{ //we're just updating G here as this node is already in a list.
				if (adjNode_newG < sectorData[adjNode].g) //new G is closer than original G calculation
				{
					sectorData[adjNode].g = adjNode_newG; //update with new G
					sectorData[adjNode].parent = cNode; //change the parent to the current node since this is a better path
				}
			}
			else //not in either list, so add to openList and update G and H
			{
				//this is a fresh node.  We need to both add it into the openList and calculate G and H.
				openList.push_back(adjNode);
				sectorData[adjNode].g = adjNode_newG; //this was calculated above.
				sectorData[adjNode].h = adjNode->getDistanceTo(*sNode);

				sectorData[adjNode].parent = cNode; //change the parent to the current node
			}
		}
	}

	//If we got here, this means we emptied out the entire openList without
	//arriving at the goal node.  This means the path is IMPOSSIBLE.
	//Return null and let calling function sort it out.
	
	//throw 1; //Couldn't find path to goal.
	std::cerr << "WARNING: Couldn't find path to goal" << std::endl;
	return NULL;
}
