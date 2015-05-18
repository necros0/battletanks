#include "EntityManager.h"
#include "AEntity.h"
#include "Shape.h"
#include "DebugDraw.h"
#include "MessageDispatcher.h"
#include "MessageDefs.h"
#include "Info.h"
#include "EntityFlags.h"
#include "CollisionSolver.h"
#include "Game.h"

#include <iostream>

EntityManager::~EntityManager(void)
{
	clear(); //if it hasn't been called.  can lead to crashes if Mouse has already been destroyed
}


void EntityManager::clear(void)
{
	for (auto it = mEntities.begin(); it != mEntities.end(); it++)
	{
		delete(it->second);
	}
	mEntities.clear();

	//addlist are entities that haven't been added to the main collection
	//we need to clear these out too
	for (auto it = addList.begin(); it != addList.end(); it++)
	{
		delete(*it);
	}
	addList.clear();
}

EntityManager &EntityManager::instance()
{
	static EntityManager instance;
	return instance;
}

void EntityManager::spawn(AEntity* e)
{
	if (!e)
	{
		std::cerr << "Tried to spawn null." << std::endl;
		return;
	}

	//std::cout << "Spawned entity with id: " << e->getID() << std::endl;
	
	this->addList.push_back(e);
}

void EntityManager::remove(EID id)
{
	this->removeList.insert(id);
	getEntityByID(id)->setRemoved(); //so anything called during this frame can know that it is removed even if it is still in the main collection (for now...)
}


AEntity* EntityManager::getEntityByID(EID id) const
{
	//can't use [] because this will create the key/val pair if it doesn't exist.
	std::map<EID, AEntity *>::const_iterator it = this->mEntities.find(id);
	if (it == this->mEntities.end()) //couldn't find it
		return NULL;
	
	//got the entity
	return it->second;
}

void EntityManager::update(TIME delta)
{
	//Do any queued entity removal or spawning now.  These are spawns from the previous frame.
	flushSpawns();

	for (auto it = this->mEntities.begin(); it != this->mEntities.end(); it++)
	{
		(*it).second->evaluate(delta); //run update methods
	}
}

void EntityManager::updateFromPhysics()
{
	for (auto it = this->mEntities.begin(); it != this->mEntities.end(); it++)
	{
		(*it).second->updatePosition(); //update positions from physics bodies
	}
}

void EntityManager::draw(sf::RenderTarget& target)
{
	for (auto it = this->mEntities.begin(); it != this->mEntities.end(); it++)
	{
		//If we removed it this frame, don't draw even if it's still in the list
		if (it->second->isRemoved)
			continue;
		it->second->draw(target);
	}
}

/*bool EntityManager::getCollision(AEntity& self, Shape& thisShape, Collision& col)
{
	if (self.getIsRemoved()) //entity was removed this frame
		return false;

	bool result = false;
	for (auto it = this->mEntities.begin(); it != this->mEntities.end(); it++)
	{
		if ((*it).second == &self) //don't collide with yourself
			continue;

		//if ((*it).second->flags & FL_NONSOLID) //this entity can't be collided with
		//	continue;
		
		if (self.getOwner() == (*it).second || &self == (*it).second->getOwner()) //don't collide with owned objects
			continue;

		if ((*it).second->getIsRemoved()) //other entity was removed this frame
			return false;

		Shape otherShape = (*it).second->getCollisionShape();
		Collision checkCollision = col; //we'll compare which collision was closer below

		checkCollision.otherID = (*it).second->getID();
		if(!thisShape.getCollision(otherShape, checkCollision))
			continue;
		
		//Save the shortest minimum translation
		//!result so that the first time bypasses the comparison check
		if (!result || checkCollision.minimumTranslation.vlen() < col.minimumTranslation.vlen())
			col = checkCollision;
		result = true;
	}

	if (col.otherID)
	{
		//Only send message at the end of the loop.  We're only really 'colliding' with the one that gave us the shortest minimum translation (eg: the closest one)
		//MessageDispatcher::instance().dispatchMessage(1, self.getID(), col.otherID, MessageType::MSG_COLLISION, new Vec2(col.origin));
		//Send a message both ways so they know they collided
		MessageDispatcher::instance().dispatchMessage(1, col.otherID, self.getID(), MessageType::MSG_COLLISION, new Vec2(col.origin));
	}
	
	return result;
}*/

void EntityManager::flushSpawns()
{
	for (auto it = removeList.begin(); it != removeList.end(); it++)
	{
		delete(mEntities[*it]);
		this->mEntities.erase(*it);
	}
	removeList.clear();

	while (!this->addList.empty())
	{
		this->mEntities.insert(std::make_pair(addList.back()->getID(), addList.back()));
		this->addList.pop_back();
	}
}