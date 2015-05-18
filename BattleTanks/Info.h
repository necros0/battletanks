#pragma once

#include "Defs.h"
#include "Vec2.h"

struct t_damage
{
	t_damage() :
		damage(0),
		attacker(0),
		origin(Vec2())
	{};

	t_damage(float d, EID a, Vec2 o) :
		damage(d),
		attacker(a),
		origin(o)
	{};

	float damage;	//how much damage was inflicted
	EID attacker;	//who the inflictor belonged to
	Vec2 origin;	//The point of impact
};

