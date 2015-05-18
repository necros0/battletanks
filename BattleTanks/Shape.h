#pragma once

#include "Defs.h"
#include "Collision.h"

class AEntity;

class Shape
{
public:
	Shape(float radius, Vec2 org, Vec2 vel, float delta);
	~Shape(void);

	bool getCollision(Shape& other, Collision& col);

	float radius;
	Vec2 oldOrigin;
	Vec2 newOrigin;
	Vec2 velocity;
	float delta;
};

