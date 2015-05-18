/*
#pragma once

#include "Game.h"
#include "Vec2.h"
#include "Map.h"

#include <Box2D/Box2D.h>

class IPhysObject
{
public:
	IPhysObject() : physObject(NULL)
	{
	};
	virtual ~IPhysObject(void) 
	{
		//if (physObject)
		//	delete(physObject);
	};

	void setBody(const b2BodyDef& body)
	{
		physObject = Game::instance().getMap().getPhysicsWorld()->CreateBody(&body);
	};

protected:
	b2Body* physObject;
};
*/