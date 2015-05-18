/*
#pragma once

enum EntityFlags
{
    FL_NONSOLID = 1,		//can't be collided with (can still collide with solids)
	FL_MOVETYPE_FLY = 16,	//no friction
	FL_MOVETYPE_HOVER = 32 //friction
};

inline EntityFlags operator|(EntityFlags left, EntityFlags right)
{
	return static_cast<EntityFlags>(static_cast<int>(left) | static_cast<int>(right));
}
*/
