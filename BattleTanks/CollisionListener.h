#pragma once

#include "AEntity.h"
#include "MessageDispatcher.h"
#include "MessageDefs.h"

#include <Box2D/Box2D.h>

class CollisionListener : public b2ContactListener 
{
	void BeginContact (b2Contact *contact)
	{
		EID selfID, otherID;

		//Get the world manifold which has information about the collision
		b2WorldManifold worldManifold;
		contact->GetWorldManifold( &worldManifold );

		//create the collision object on the heap
		Collision* col = new Collision();

		//check what the userdata is here
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if (bodyUserData) //this body didn't have a pointer... can't do anything with it.
		{
			AEntity* self = (AEntity*)bodyUserData; //Only entities will have body user data...
			selfID = self->getID();
		}
		else
			selfID = -1; //this is the world if there's no pointer here
		//HACK: Note that this is still a hack because world physics objects don't have pointers to
		//  their backing entities (because they don't have them) so we need to hack in the use of the
		//  world EID hack number

		//same as above but for the other colliding physics object
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserData)
		{
			AEntity* other = (AEntity*)bodyUserData;
			otherID = other->getID();
		}
		else
			otherID = -1;

		col->normal = Vec2(worldManifold.normal); //collision normal
		Collision* col2 = new Collision(*col); //create a copy of the collision on the heap so we can send with the mirror message

		MessageDispatcher::instance().dispatchMessage(0, selfID, otherID, MSG_COLLISION, col);
		
		//FIXME: Need to send this message both ways because there is only one collision event.
		col2->otherID = selfID; //change otherID to match this collision
		MessageDispatcher::instance().dispatchMessage(0, otherID, selfID, MSG_COLLISION, col2);
	}

	//We don't need all the granularity that Box2D offers
	void EndContact (b2Contact *contact) {};
	void PreSolve (b2Contact *contact, const b2Manifold *oldManifold) {};
	void PostSolve (b2Contact *contact, const b2ContactImpulse *impulse) {};
};