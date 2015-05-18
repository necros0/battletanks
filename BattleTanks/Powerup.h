#include "AEntity.h"

#pragma once
class Powerup : public AEntity
{
public:
	Powerup(Vec2);
	virtual ~Powerup(void) {};

	Shape getCollisionShape() override;
	bool handleMessage(Telegram const &telegram) override;
	virtual bool handlePickupMessage(Telegram const &telegram) = 0;
};

class PowerupKinetic : public Powerup
{
public:
	PowerupKinetic(Vec2 org);
	virtual ~PowerupKinetic(void);
	virtual bool handlePickupMessage(Telegram const &telegram);

	virtual void update(TIME);
};

class PowerupShield : public Powerup
{
public:
	PowerupShield(Vec2 org);
	virtual ~PowerupShield(void);
	virtual bool handlePickupMessage(Telegram const &telegram);

	virtual void update(TIME);
};