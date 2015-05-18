#include "Map.h"
#include "Sector.h"
#include "Shape.h"
#include "Utility.h"
#include "DebugDraw.h"
#include "AEntity.h"
#include "Trace.h"
#include "CollisionListener.h"
#include "Tank.h"
#include "EntityManager.h"
#include "Defs.h"
#include "TextureAsset.h"
#include "Game.h"

#include <Box2D/Box2D.h>

#include <vector>
#include <iostream>
#include <list>

Map::Map(void) :
	mPlayerSector(NULL),
	physWorld(new b2World(b2Vec2(0.0f, 0.0f))),
	physListener(new CollisionListener()),
	mMapTexture(Game::Texture().get("resources/texture/rock_grey2.tga"))
{
	physWorld->SetAllowSleeping(true);
	physWorld->SetContactListener(physListener);


	std::vector<Vec2> verts;

	float MIN_SECTOR_SIZE = 600.0f;
	float VERTEX_MERGE_DISTANCE = 32.0f;

	Vec2 org;
	org.x = WINDOW_WIDTH * 0.5f; //random() * (WINDOW_WIDTH - 200) + 100;
	org.y = WINDOW_HEIGHT * 0.5f; //random() * (WINDOW_HEIGHT - 200) + 100;

	//verts.push_back(org + Vec2::toVector(random() * 120 + 000) * (random() * 15 + 550));
	//verts.push_back(org + Vec2::toVector(random() * 120 + 120) * (random() * 15 + 550));
	//verts.push_back(org + Vec2::toVector(random() * 120 + 240) * (random() * 15 + 550));

	verts.push_back(org + Vec2(-100, 50));
	verts.push_back(org + Vec2(0, -100));
	verts.push_back(org + Vec2(100, 20));

	this->pushGeometry(Sector(verts));
	
	//TODO: Move this stuff off into methods.
	
	int numGrowths = 0;
	int loopCount = 0;
	int MAX_LOOP_COUNT = 30; //cap loops in case there are too many rejects
	//FIXME: This nested shit is ugly.
	while(numGrowths < 100 && loopCount < MAX_LOOP_COUNT) //loop until we've grown 30 new sectors from edges
	{
		std::cout << loopCount << std::endl;
		loopCount++;
		//A collection to store all the new sectors we make so we can add then all in at once at the end
		std::vector<Sector> tempNewSectors;

		for(auto it = mGeometry.begin(); it != mGeometry.end(); it++)
		{
			Sector& sector = *it; //for convenience...

			for (int edgeIndex = 0; edgeIndex < 3; edgeIndex++)
			{
				if (!sector.mEdges[edgeIndex].mIsSolid) //can't grow from here, non-solid means already connected to a sector
					continue;

				if (random() < 0.5f) //50% chance to not grow this edge
					continue;


				//80% chance to make a hallway instead of a corner
				//Hallway = 2 triangles roughly perpendicular to the edge being grown
				//Corner = 1 triangle with 2 vertices sharing the parent triangle
				if (random() < 0.8f)
				{ //HALLWAY
					verts.clear();
					verts.push_back(sector.mEdges[edgeIndex].mPoint1); //create the first two vertices that share
					verts.push_back(sector.mEdges[edgeIndex].mPoint2); //their positions with the parent sector

					Vec2 edgeDir = (verts[1] - verts[0]);
					Vec2 n = edgeDir.getNormal().normalize(); //get the edge's normal
					Vec2 mid = (verts[0] + verts[1]) * 0.5f;  //get the edge's mid point
					Vec2 newPoint = mid - n * edgeDir.vlen() * (0.5f + random() * 0.5f) + edgeDir.normalize() * edgeDir.vlen() * (random() * 0.3f + 0.3f);

					//DebugDraw::drawPermanent(mid, newPoint, sf::Color(0, 100, 255, 255));

					//Make sure this sector doesn't overlap with anything
					if (!isNewSectorValid(newPoint, verts[0], verts[1], tempNewSectors))
						continue;

					//go outward and then to the side
					verts.push_back(newPoint);
					Sector s(verts);
					if (s.getArea() < MIN_SECTOR_SIZE) //too small, reject
						continue;

					tempNewSectors.push_back(s); //add it to the list of sectors to add to the collection.
					tempNewSectors.back().mEdges[2].mIsSolid = false; //connecting edge on new sector is non-solid

					//juggle verts around, vert 0 and 2 are the ones making up the mid point
					verts[1] = verts[2];
					verts.pop_back(); //remove last vert

					//create the vert for the other side of the hallway, this time go the opposite side of the midpoint
					newPoint = mid - n * edgeDir.vlen() * (0.5f + random() * 0.5f)  - edgeDir.normalize() * edgeDir.vlen() * (random() * 0.3f + 0.3f);

					//Still need to check if the other side overlaps
					if (!isNewSectorValid(newPoint, verts[0], verts[1], tempNewSectors))
					{
						// same as below, we have to do this because code never reaches
						// that point if the second part of the hallway fails
						sector.mEdges[edgeIndex].mIsSolid = false;
						continue;
					}
					
					//since we have decided to add the other half of the hallway, the edge that bisects the hallway is not solid
					tempNewSectors.back().mEdges[0].mIsSolid = false; 

					verts.push_back(newPoint);
					tempNewSectors.push_back(Sector(verts)); //add it to the list of sectors to add to the collection.
					tempNewSectors.back().mEdges[2].mIsSolid = false; //connecting edge on new sector is non-solid

					numGrowths = numGrowths + 2; //we're growing this edge. (2 because hallway is 2 triangles)
				}
				else
				{ //CORNER
					verts.clear(); //just keep reusing this thing.
					verts.push_back(sector.mEdges[edgeIndex].mPoint1); //create the first two vertices that share
					verts.push_back(sector.mEdges[edgeIndex].mPoint2); //their positions with the parent sector

					Vec2 edgeDir = (verts[1] - verts[0]);
					Vec2 n = edgeDir.getNormal().normalize(); //get the edge's normal
					Vec2 mid = (verts[0] + verts[1]) * 0.5f;				//get the edge's mid point
					Vec2 newPoint = mid - n * (verts[1] - verts[0]).vlen() * (random() * 0.3f + 0.3f);

					if (!isNewSectorValid(newPoint, verts[0], verts[1], tempNewSectors))
						continue;

					//project outwards from the midpoint of the edge to create a new vertex for a new sector
					verts.push_back(newPoint);
					Sector s(verts);
					if (s.getArea() < MIN_SECTOR_SIZE) //too small, reject
						continue;
					tempNewSectors.push_back(s); //add it to the list of sectors to add to the collection.
					tempNewSectors.back().mEdges[2].mIsSolid = false; //connecting edge on new sector is non-solid
					numGrowths++; //we're growing this edge.

				}

				//if we're growing this edge, then it's not solid!
				sector.mEdges[edgeIndex].mIsSolid = false;
			}
		}

		//put all the new sectors into the collection
		//FIXME: how to add collection to another collection like in java?????????????
		for (auto it = tempNewSectors.begin(); it != tempNewSectors.end(); it++)
		{
			this->pushGeometry(*it);
		}
	}



	//TODO: Instead of the below, grab a vertex, get the pair of edges connected to it and check dot product
	// if close enough together, check distance of the vertices at the other ends of the edges to see if a merge
	// can be done.  will need to move other verts on the same spot.

	//TODO: LINK VERTICES TOGETHER AT CONSTRUCTION???????

	/*
	//Now, merge vertices that are close together...
	for(auto sIt = mGeometry.begin(); sIt != mGeometry.end(); sIt++)
	{
		Sector& self = *sIt;
		for (int sVIndex = 0; sVIndex < 3; sVIndex++)
		{
			Vec2& sV = self.mVertices[sVIndex];
			for(auto oIt = mGeometry.begin(); oIt != mGeometry.end(); oIt++)
			{
				Sector& other = *oIt;
				for (int oVIndex = 0; oVIndex < 3; oVIndex++)
				{
					Vec2& oV = other.mVertices[oVIndex];

					float dot = ().normalize() * ().normalize();

					float len = (sV - oV).vlen();
					if (len < VERTEX_MERGE_DISTANCE)
					{
						other.setVertex(oVIndex, sV);
					}
				}
			}
		}
	}
	*/

	//create the main box2d body for the map (defaults to static)
	b2BodyDef mapbd;
	b2Body* mapBody = this->getPhysicsWorld()->CreateBody(&mapbd);

	//Now, link edges to adjacent sectors
	for(auto sIt = mGeometry.begin(); sIt != mGeometry.end(); sIt++)
	{
		Sector& self = *sIt;
		for (int sEIndex = 0; sEIndex < 3; sEIndex++)
		{
			Edge& sE = self.mEdges[sEIndex];

			if (sE.mIsSolid)
			{
				//While we're visiting all the edges, create the physics edges for Box2D
				b2EdgeShape edgeShape;
				edgeShape.Set(b2Vec2(sE.mPoint1.x, sE.mPoint1.y), b2Vec2(sE.mPoint2.x, sE.mPoint2.y));
				mapBody->CreateFixture(&edgeShape, 0.0f);
			}

			for(auto oIt = mGeometry.begin(); oIt != mGeometry.end(); oIt++)
			{
				Sector& other = *oIt;

				if (&self == &other)
					continue;

				for (int oEIndex = 0; oEIndex < 3; oEIndex++)
				{
					Edge& oE = other.mEdges[oEIndex];

					//these edges are shared between the two sectors
					if (sE == oE)
					{
						sE.mAdjacentSector = &other;
					}
				}
			}
		}
	}

	mPlayerSector = &mGeometry.front();
}

bool Map::isNewSectorValid(Vec2 newPoint, Vec2 verts0, Vec2 verts1, const std::vector<Sector>& sectorCollection)
{
	//Outside of screen
	if (newPoint.x < 0 || newPoint.x > WINDOW_WIDTH || newPoint.y < 0 || newPoint.y > WINDOW_HEIGHT)
		return false;

	//check existing sectors for overlap     check new sectors for overlap (easy out)
	if (checkOverlap(newPoint, mGeometry) || checkOverlap(newPoint, sectorCollection))
		return false;

	//Check if either of the edges intersect any edges of the other sectors (more precise check for overlap)
	Edge e1(verts0, newPoint);
	if (checkIntersection(e1, mGeometry) || checkIntersection(e1, sectorCollection))
		return false;

	Edge e2(verts1, newPoint);
	if (checkIntersection(e2, mGeometry) || checkIntersection(e2, sectorCollection))
		return false;

	return true;
}

Map::~Map(void)
{
	this->mGeometry.clear();

	if (this->physWorld)
		delete(this->physWorld);

	if (this->physListener)
		delete(this->physListener);
}


void Map::draw(sf::RenderTarget& target)
{
	sf::Sprite sprite;
	sprite.setTexture(this->mMapTexture->getTexture());
	sprite.setTextureRect(sf::IntRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

	target.draw(sprite);
	for (auto it = this->mGeometry.begin(); it != this->mGeometry.end(); ++it)
	{
		it->draw(target);
	}
}

void Map::pushGeometry(Sector& s)
{
	this->mGeometry.push_back(s);
}

/*bool Map::getCollision(AEntity& self, Shape& shape, Collision& col)
{
	bool isInsideWorld = false;
	bool result = false;
	for (auto it = this->mGeometry.begin(); it != this->mGeometry.end(); it++)
	{
		//Check inside with negative radius.  We want to expand outwards (rather than inwards) here
		//because while a point can only be in one triangle at once, a circle can be in 1 or more.
		if (!(*it).isInside(shape.oldOrigin, -shape.radius))
		{
			//(*it).setHighlight(false);
			continue;
		}
		//else
		//	(*it).setHighlight(true);

		if (self.getID() == 1) //if this is the player, track their location on the map.
		{
			if ((*it).isInside(shape.oldOrigin, 0))
				mPlayerSector = &(*it);
		}

		isInsideWorld = true;

		Collision checkCollision = col;

		checkCollision.otherID = 0;
		if (!(*it).getCollision(shape, checkCollision))
			continue;
		
		//Save the shortest minimum translation
		//!result so that the first time bypasses the comparison check
		if (!result || checkCollision.minimumTranslation.vlen() < col.minimumTranslation.vlen())
			col = checkCollision;
		result = true;
	}
	
	//This kind of weird boolean lets us count a collision as true if the entity is not inside ANY
	//world sectors (eg: outside in the void)
	//This will stop edge cases where entities could go through a wall at a very oblique angle because
	//they will register a collision on the void anyway. (this only happens with tiny objects like projectiles)
	return result || !isInsideWorld;
}*/

bool Map::checkOverlap(Vec2 p, const std::vector<Sector>& sectorCollection)
{
	for (auto sectorCheckIt = sectorCollection.begin(); sectorCheckIt != sectorCollection.end(); sectorCheckIt++)
	{
		if ((*sectorCheckIt).isInside(p, 0))
			return true;
	}

	return false;
}

bool Map::checkIntersection(const Edge& e, const std::vector<Sector>& sectorCollection)
{
	for (auto sectorCheckIt = sectorCollection.begin(); sectorCheckIt != sectorCollection.end(); sectorCheckIt++)
	{
		const Sector& s = *sectorCheckIt;
		for (int i = 0; i < 3; i++)
		{
			if (   e.mPoint1 == s.mEdges[i].mPoint1 || e.mPoint1 == s.mEdges[i].mPoint2
				|| e.mPoint2 == s.mEdges[i].mPoint1 || e.mPoint2 == s.mEdges[i].mPoint2)
				continue;

			if (e.checkIntersection((*sectorCheckIt).mEdges[i]))
			{
				//DebugDraw::drawPermanent(e.mPoint1, e.mPoint2, sf::Color(0, 150, 0, 255));
				//DebugDraw::drawPermanent(s.mEdges[i].mPoint1, s.mEdges[i].mPoint2, sf::Color(150, 0, 0, 255));
				return true;
			}
		}
	}
	return false;
}

Sector* Map::getPlayerSector()
{
	return mPlayerSector;
}

Sector* Map::getSector(Vec2 p)
{
	for (auto it = this->mGeometry.begin(); it != this->mGeometry.end(); it++)
	{
		if (!(*it).isInside(p, 0))
			continue;
		
		return &(*it);
	}
	return NULL;
}

Trace Map::traceLine(Vec2 sOrg, Vec2 eOrg, bool ignoreMonsters)
{
	bool hasCollided = false;
	float nearestColDist = 999999;
	Vec2 nearestColPos;
	Edge sE(sOrg, eOrg);
	for(auto oIt = mGeometry.begin(); oIt != mGeometry.end(); oIt++)
	{
		Sector& other = *oIt;
		for (int oEIndex = 0; oEIndex < 3; oEIndex++)
		{
			Edge& oE = other.mEdges[oEIndex];
			if (!oE.mIsSolid) //edge isn't solid
				continue;

			if (sE.checkIntersection(oE))
			{
				Vec2 result;
				sE.getIntersection(oE, result);
				
				float newDist = (result - sOrg).vlen();
				if (newDist < nearestColDist)
				{
					nearestColPos = result;
					nearestColDist = newDist;
					hasCollided = true;
				}
			}
		}
	}
	Trace t;
	t.startPoint = sOrg;
	if (hasCollided)
	{
		t.endPoint = nearestColPos;
		t.frac = nearestColDist / (eOrg - sOrg).vlen();
	}
	else
	{
		t.endPoint = eOrg;
		t.frac = 1.0f;
	}
	return t;
}

Sector* Map::getRandomSector()
{
	int sIndex = (int)(random() * mGeometry.size());
	return &mGeometry[sIndex];
}

void Map::spawnAI()
{
	Tank* t = new Tank(false);
	t->setOrigin(getRandomSector()->getCenter());
	EntityManager::instance().spawn(t);
}