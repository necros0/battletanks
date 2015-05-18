#pragma once

#include "Defs.h"
#include "EntityFlags.h"
#include "IThink.h"
#include "Vec2.h"
#include "Collision.h"

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class sf::Drawable;
class Texture;
class Anim;
class Shape;
class MouseListener;
class b2Body;

struct Telegram;

class AEntity
{
	friend class CollisionSolver;
	friend class EntityManager;
public:
	AEntity(void);
	virtual ~AEntity(void);
	void clearThinks(void);

	EID getID() const;

	void evaluate(TIME delta);
	virtual void update(TIME delta) = 0;
	void updatePosition();
	virtual bool handleMessage(Telegram const &telegram) = 0;
	virtual void draw(sf::RenderTarget& target);
	virtual Shape getCollisionShape() = 0;

	AEntity* getOwner() { return this->owner; }

	bool getIsRemoved() { return isRemoved; };
	void setRemoved() { isRemoved = true; };

	//virtual void beginContact(Collision&) {};
	//virtual void endContact (Collision&) {};
	//virtual void PreSolve (b2Contact *contact, const b2Manifold *oldManifold);
	//virtual void PostSolve (b2Contact *contact, const b2ContactImpulse *impulse);

	static EID getUniqueID();

protected:
	Vec2 mOrigin;
	Vec2 mVelocity;

	TIME nextthink;

	sf::Drawable* mDrawable;
	//Texture* mTexture;
	Anim* mAnim;

	IThink* think;

	MouseListener* mouseListener;

	AEntity* owner;

	//EntityFlags flags;

	b2Body* physBody;
	void createPhysBodyCirc(float, bool, uint16, uint16);

	enum CollisionFilter
	{
		SOLID_ENTITY = 1,
		SOLID_PROJECTILE = 2,
	};

private:
	EID mID;
	static EID nextID;
	bool isRemoved;
};