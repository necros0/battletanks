#pragma once

#include "Collision.h"

#define COLLISION_MIN_TRANSLATION_EPSILON (0.1f)

class Shape;
class AEntity;

class CollisionSolver
{
private:
	CollisionSolver(void);

public:
	~CollisionSolver(void);

	//static bool evaluate(AEntity& self, Shape& shape);
	static void evaluate(float delta);

private:
	//static bool evaluateWorld(AEntity& self, Shape& shape, Collision& col);
	//static bool evaluateEntity(AEntity& self, Shape& shape, Collision& col);
};

