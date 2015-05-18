#include "KineticStrike.h"
#include "Shape.h"
#include "Game.h"
#include "EntityManager.h"
#include "MessageDefs.h"
#include "MessageDispatcher.h"
#include "Utility.h"
#include "Info.h"

#include <SFML/Graphics.hpp>

#include <iostream>

#define KINETICSTRIKE_RADIUS (80.0f)
#define KINETICSTRIKE_LENGTH (WINDOW_WIDTH + 32.0f)

KineticStrike::KineticStrike(Vec2 org, EID ownerID) :
	mOwnerID(ownerID),
	mRadius(0.0f),
	mState(0),
	mNextStateTime(Game::instance().getTime() + 3.0f), //3 second period before effect occurs
	mGraphicsTime(0.0f),
	mCircle(NULL),
	mStrikeOrigin(),
	mStrikeProgress(0.0f)
{
	this->mOrigin = org;
	createPhysBodyCirc(KINETICSTRIKE_RADIUS, true, 0, 0);

	sf::CircleShape* c = new sf::CircleShape(3.0f, 8);
	c->setOrigin(3.0f, 3.0f);
	c->setPosition(mOrigin);
	c->setFillColor(sf::Color(255, 0, 0, 255));
	this->mDrawable = mCircle = c;

	//play targetting sound
	Game::Sound().get("resources/sound/items/kine_target.wav")->play(1.0f, 0);
}

KineticStrike::~KineticStrike(void)
{
	mDrawable = mCircle; //Let AEntity clean up mDrawable (but this might be NULL because of the flashing effect)
}

void KineticStrike::update(TIME delta)
{
	TIME time = Game::instance().getTime();
	if (time > mNextStateTime) //time to move to the next state
		++mState;

	//switch gets cpu speed boost with consecutive int cases.
	switch (mState)
	{
	case 0:
		if (time > mGraphicsTime)
		{
			if (mDrawable) //flash laser on and off
				mDrawable = NULL;
			else
				mDrawable = mCircle;
			mGraphicsTime = time + 0.1f;
		}
		break;
	case 1:
		if (time > mNextStateTime)
		{
			b2Filter filter;
			for (b2Fixture* f = physBody->GetFixtureList(); f; f = f->GetNext())
			{
				filter = f->GetFilterData();
				filter.categoryBits = SOLID_ENTITY; //make sensor trigger on entities for one frame
				filter.maskBits = SOLID_ENTITY;
				f->SetFilterData(filter);
			}

			mDrawable = mCircle; //make sure circle is visible if it was off at the end of state0

			mNextStateTime = time + 0.1f; //leave trigger field active for a short while to collect collisions
		}
		break;
	case 2:
		if (time > mNextStateTime)
		{
			b2Filter filter;
			for (b2Fixture* f = physBody->GetFixtureList(); f; f = f->GetNext())
			{
				filter = f->GetFilterData();
				filter.categoryBits = 0; //make sensor non-solid again
				filter.maskBits = 0;
				f->SetFilterData(filter);
			}
	
			//Stun everything in range
			for (auto it = mStunList.begin(); it != mStunList.end(); it++)
			{
				MessageDispatcher::instance().dispatchMessage(0, this->getID(), *it, MSG_DEBUFF_STUN, NULL);
				struct t_damage* t = new struct t_damage(25, mOwnerID, mOrigin);
				MessageDispatcher::instance().dispatchMessage(0, this->getID(), *it, MSG_DAMAGE, t);
			}
			
			float angle = random() * 360;
			mStrikeOrigin.x = cos(angle);
			mStrikeOrigin.y = sin(angle);
			mStrikeOrigin = mStrikeOrigin * KINETICSTRIKE_LENGTH;
			mStrikeOrigin = mStrikeOrigin + Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f);

			Game::Sound().get("resources/sound/items/kine_flyby.wav")->play(1.0f, 0);
			mNextStateTime = time + 0.35f; //wait a short while between woosh noise and explosions
		}

		{
			float t = mNextStateTime - time;
			mStrikeProgress = t / 0.35f; //sqrt(t / 10.35f);
		}

		break;
	case 3:
		{
			float explosionTime = 1.5f;
			if (time > mNextStateTime)
			{
				//Play explosion once
				Game::Sound().get("resources/sound/explosion_1.wav")->play(1.0f, 0);
				mDrawable = mCircle; //start drawing circle again
				mCircle->setPointCount(24); //explosion is big so use more vertices
				mNextStateTime = time + explosionTime;
			}
		
			float t = mNextStateTime - time;
			
			//Stop graphical glitches when time is slightly off
			if (t < 0) //> explosionTime)
				break;
			
			t = sqrt(t / explosionTime);
			float rad = t * KINETICSTRIKE_RADIUS;
			mCircle->setRadius(rad);
			mCircle->setOrigin(Vec2(rad, rad));
			mCircle->setFillColor(sf::Color((int)(255 * t), 0, 0, (int)(255 * t)));

			float progressT = 1 - (t / 0.35f);
			if (progressT <= 1)
			{
				mStrikeProgress =  progressT; //sqrt(progressT);
			}
			else
				mStrikeProgress = 1.0f; 
		}
		break;
	case 4:
		EntityManager::instance().remove(this->getID()); //now remove it.
		break;
	}
	
}

void KineticStrike::draw(sf::RenderTarget& target)
{
	switch (mState)
	{
	case 2:
		{
			Vec2 v(mOrigin - mStrikeOrigin);
			v = v.normalize();
			Vec2 moveVec = v * KINETICSTRIKE_LENGTH;
			Vec2 startPos = mOrigin - (moveVec * mStrikeProgress);

			Vec2 p[] =
			{
				startPos,
				startPos - v * 4.0f + v.getNormal() * 4.0f,
				startPos - v * 250.0f,
				startPos - v * 4.0f + v.getNormal() * -4.0f
			};

			sf::Vertex missile[] =
			{
				sf::Vertex(p[0].toScreenVec(), sf::Color(255, 255, 0, 255)),
				sf::Vertex(p[1].toScreenVec(), sf::Color(255, 0, 0, 255)),
				sf::Vertex(p[2].toScreenVec(), sf::Color(100, 0, 0, 0)),
				sf::Vertex(p[3].toScreenVec(), sf::Color(255, 0, 0, 255))
			};

			target.draw(missile, 4, sf::Quads);
		}
		break;
	case 3:
		break;
	}
	AEntity::draw(target);
}

bool KineticStrike::handleMessage(const Telegram& telegram)
{
	if (telegram.msg == MSG_COLLISION)
	{
		//not worth std::set overhead for this.
		if (std::find(mStunList.begin(), mStunList.end(), telegram.sender) != mStunList.end()) //already in list
			return true;

		mStunList.push_back(telegram.sender); //record who collided with the explosion zone.
	}
	return false;
}

Shape KineticStrike::getCollisionShape()
{
	return Shape(this->mRadius, this->mOrigin, this->mVelocity, 0);
}
