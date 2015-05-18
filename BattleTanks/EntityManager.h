#pragma once

#include "Defs.h"
#include "Collision.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <list>
#include <set>

class AEntity;
class Shape;

class EntityManager
{
private:
	//Kevin's static singleton
	EntityManager() {};
	EntityManager(EntityManager const &mgr); // hide cop.y constructor
	EntityManager& operator=(EntityManager const &mgr); // hide assignment operator

public:
	~EntityManager();

	static EntityManager& instance(); //same as java static singleton?

	void spawn(AEntity *e);  //only method to add new entities to the game
	void remove(EID id); //only method to remove entities from the game

	void clear();

	void flushSpawns(); //adds all entities in the addlist to the main list immediately.

	AEntity* getEntityByID(EID id) const;

	void update(TIME delta);
	void updateFromPhysics();
	void draw(sf::RenderTarget& target);
	//bool getCollision(AEntity& self, Shape& thisShape, Collision& col);

private:
	std::map<EID, AEntity*> mEntities; //collection of entities
	std::set<EID> removeList;
	std::list<AEntity*> addList;
};

