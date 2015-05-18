#include "CollisionSolver.h"
#include "Game.h"
#include "Map.h"
#include "EntityManager.h"
#include "Shape.h"
#include "AEntity.h"
#include "DebugDraw.h"
#include "Sector.h"

#include <Box2D/Box2D.h>

CollisionSolver::CollisionSolver(void)
{
}


CollisionSolver::~CollisionSolver(void)
{
}
/*
bool CollisionSolver::evaluate(AEntity& self, Shape& shape)
{
	if (self.mVelocity.vlen() < 1) //moving really slowly, just stop
	{
		self.mVelocity = Vec2();
		return false;
	}

	if (self.mOrigin.x != self.mOrigin.x || self.mVelocity.x != self.mVelocity.x)
		std::cerr << "NaN" << std::endl;
	
	Vec2 oldOrigin = self.mOrigin;

	bool result = false;
	int collisionLoopCount = 0;
	int MAX_COLLISION_LOOP = 8;
	Collision col; //create a collision struct to collection collision data
	while (collisionLoopCount++ < MAX_COLLISION_LOOP && (evaluateWorld(self, shape, col) || evaluateEntity(self, shape, col)))
	{
		//Move only if the minimum translation is larger than the epsilon
		if (col.minimumTranslation.vlen() > COLLISION_MIN_TRANSLATION_EPSILON)
			shape.oldOrigin = self.mOrigin = self.mOrigin + col.minimumTranslation; //put it as close as is safe

		Vec2 nNormal = col.normal.normalize();
		//remove the vector component that is moving into the object
		self.mVelocity = shape.velocity = shape.velocity - (shape.velocity * nNormal) * nNormal;
		
		//Update the shape newOrigin to account for our processing of velocity above
		shape.newOrigin = shape.oldOrigin + shape.velocity * shape.delta;

		if (shape.newOrigin.x < 0 || shape.newOrigin.x > WINDOW_WIDTH || shape.newOrigin.y < 0 || shape.newOrigin.y > WINDOW_HEIGHT)
		{
			std::cerr << "An entity is moving outside of the screen." << std::endl;
			collisionLoopCount = MAX_COLLISION_LOOP;
		}

		col.clear(); //clean collision struct data

		result = true;
	}

	if (collisionLoopCount >= MAX_COLLISION_LOOP)
	{
		self.mOrigin = oldOrigin;
		self.mVelocity = Vec2();
		std::cout << "Unstuck" << std::endl;
		return true;
	}


	//Do the actual move
	self.mOrigin = self.mOrigin + self.mVelocity * shape.delta;
	return result;
}
*/

void CollisionSolver::evaluate(float delta)
{
	Map& map = Game::instance().getMap();
	map.getPhysicsWorld()->Step(delta, 8, 4); //run Box2D physics
}

/*
bool CollisionSolver::evaluateWorld(AEntity& self, Shape& shape, Collision& col)
{
	return Game::instance().getMap().getCollision(self, shape, col);
}

bool CollisionSolver::evaluateEntity(AEntity& self, Shape& shape, Collision& col)
{
	//EntityManager runs collisions on all entities
	return EntityManager::instance().getCollision(self, shape, col);
}
*/
