#pragma once

#include "Collision.h"
#include "Trace.h"

#include <SFML/Graphics.hpp>

#include <vector>

class Sector;
class Shape;
class Edge;
class AEntity;
class b2World;
class b2ContactListener;
class TextureAsset;

class Map
{
	friend class PathFactory;
public:
	Map(void);
	~Map(void);

	void draw(sf::RenderTarget& target);

	void pushGeometry(Sector& s);
	//bool getCollision(AEntity& self, Shape& shape, Collision& col);
	void spawnAI();
	
	Sector* getPlayerSector();
	Sector* getSector(Vec2);
	Sector* getRandomSector();
	Trace traceLine(Vec2, Vec2, bool);

	b2World* getPhysicsWorld() { return physWorld; };

private:
	std::vector<Sector> mGeometry;

	Sector* mPlayerSector;

	b2World* physWorld;
	b2ContactListener* physListener;

	TextureAsset* mMapTexture;

	bool checkOverlap(Vec2 p, const std::vector<Sector>& sectorCollection);
	bool checkIntersection(const Edge& e, const std::vector<Sector>& sectorCollection);
	bool isNewSectorValid(Vec2, Vec2, Vec2, const std::vector<Sector>& sectorCollection);
};

