#pragma once

#include "Defs.h"
#include "Vec2.h"

struct t_collision_data
{
	Vec2 normal;
	Vec2 minimumTranslation;
	Vec2 origin;
	EID otherID;
	//bool ignore;

	t_collision_data() :
		normal(),
		minimumTranslation(),
		origin(),
		otherID(0)//,
		//ignore(false)
	{};

	void clear()
	{
		normal = Vec2();
		minimumTranslation = Vec2();
		origin = Vec2();
		otherID = 0;
		//ignore = false;
	};

} typedef Collision;