#include "Powerup.h"
#include "Game.h"
#include "Shape.h"
#include "MessageDispatcher.h"
#include "MessageDefs.h"
#include "Info.h"
#include "Utility.h"
#include "EntityManager.h"
#include "ResourceManager.h"
#include "TextureAsset.h"
#include "Tank.h"

#include <SFML/Graphics.hpp>

#define POWERUP_RADIUS (TANK_RADIUS + 4)

Powerup::Powerup(Vec2 org)
{
	mOrigin = org;
};

Shape Powerup::getCollisionShape()
{
	return Shape(POWERUP_RADIUS, this->mOrigin, this->mVelocity, 0);
}

bool Powerup::handleMessage(Telegram const &telegram)
{
	if (telegram.msg == MessageType::MSG_COLLISION)
	{
		return handlePickupMessage(telegram);
	}
	else if (telegram.msg == MessageType::MSG_POWERUP_PICKUP)
	{
		//Play pickup sound
		Game::Sound().get("resources/sound/items/pickup.wav")->play(1.0f, 0);
		EntityManager::instance().remove(this->getID()); //remove powerup
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////// Kinetic Strike powerup
PowerupKinetic::PowerupKinetic(Vec2 org) :
	Powerup(org)
{
	createPhysBodyCirc(POWERUP_RADIUS, true, SOLID_ENTITY, SOLID_ENTITY);

	sf::CircleShape* s = new sf::CircleShape(POWERUP_RADIUS, 20);
	s->setOrigin(Vec2(POWERUP_RADIUS, POWERUP_RADIUS));
	s->setTexture(&Game::instance().Texture().get("resources/texture/kinetic.png")->getTexture(), false);
	
	mDrawable = s;
}

PowerupKinetic::~PowerupKinetic()
{
}

bool PowerupKinetic::handlePickupMessage(Telegram const &telegram)
{
	MessageDispatcher::instance().dispatchMessage(0, this->getID(), telegram.sender, MSG_POWERUP_KINETIC_ACQUIRE, NULL);
	return true;
}

void PowerupKinetic::update(TIME delta)
{
}


/////////////////////////////////////////////////////// Shield powerup
PowerupShield::PowerupShield(Vec2 org) :
	Powerup(org)
{
	createPhysBodyCirc(POWERUP_RADIUS, true, SOLID_ENTITY, SOLID_ENTITY);

	sf::CircleShape* s = new sf::CircleShape(POWERUP_RADIUS, 20);
	s->setOrigin(Vec2(POWERUP_RADIUS, POWERUP_RADIUS));
	s->setTexture(&Game::instance().Texture().get("resources/texture/healing.png")->getTexture(), false);
	
	mDrawable = s;
}

PowerupShield::~PowerupShield()
{
}

bool PowerupShield::handlePickupMessage(Telegram const &telegram)
{
	MessageDispatcher::instance().dispatchMessage(0, this->getID(), telegram.sender, MSG_POWERUP_SHIELD_ACQUIRE, NULL);
	return true;
}

void PowerupShield::update(TIME delta)
{
}