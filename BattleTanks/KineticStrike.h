#pragma once
#include "AEntity.h"
#include "Vec2.h"

#include <list>

class sf::CircleShape;

class KineticStrike :
	public AEntity
{
public:
	KineticStrike(Vec2, EID);
	~KineticStrike(void);

	virtual void update(TIME) override;
	virtual void draw(sf::RenderTarget& target) override;

	virtual bool handleMessage(const Telegram&) override;

	virtual Shape getCollisionShape() override;

private:
	EID mOwnerID;
	Vec2 mStrikeOrigin;
	float mStrikeProgress;

	float mRadius;

	int mState; //internally track which state we're in
	TIME mNextStateTime; //time until next state change

	std::list<int> mStunList;

	TIME mGraphicsTime; //general purpose timer for flashing or grow/fade effects

	sf::CircleShape* mCircle;
};

