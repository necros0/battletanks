#include "ThinkTankAI.h"
#include "Defs.h"
#include "Tank.h"
#include "Game.h"
#include "Utility.h"

#include <iostream>

ThinkTankAI::ThinkTankAI(Tank* o) :
	self(o)
{
	x = 0;
}


ThinkTankAI::~ThinkTankAI(void)
{
}

void ThinkTankAI::think()
{
	if (x == 1)
	{
		std::cout << "Start" << std::endl;
		self->mWishVelocity.x = random() * 2 - 1;
		self->mWishVelocity.y = random() * 2 - 1;
	}
	else
	{
		std::cout << "Stop" << std::endl;
		self->mWishVelocity = Vec2(0, 0);
	}

	x = 1 - x;

	self->nextthink = Game::instance().getTime() + 1.5f;
}