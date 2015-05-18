#include "AEntity.h"
#include "Game.h"
#include "Map.h"
#include "Shape.h"

#include <Box2D/Box2D.h>

AEntity::AEntity(void) :
	mID(getUniqueID()),
	mDrawable(NULL),
	//mTexture(NULL),
	mAnim(NULL),
	think(NULL), //initialize this pointer to null
	nextthink(-1), //think at least once when spawned
	mouseListener(NULL), //initialize this pointer to null
	owner(NULL),
	mOrigin(Vec2()),
	mVelocity(Vec2()),
	isRemoved(false),
	physBody(NULL)
{
	//std::cout << "Entity created with id: " << mID << std::endl;
}

/*
AEntity::AEntity(Vec2 org, Vec2 vel, AEntity&) :
	mID(getUniqueID()),
	mDrawable(NULL),
	mTexture(NULL),
	mAnim(NULL),
	think(NULL), //initialize this pointer to null
	nextthink(-1), //think at least once when spawned
	mouseListener(NULL), //initialize this pointer to null
	owner(NULL),
	mOrigin(Vec2()),
	mVelocity(Vec2())
{
}
*/

AEntity::~AEntity(void)
{
	if (mDrawable)
		delete(mDrawable);

	//if (mTexture)
	//	delete(mTexture);

	//if (mAnim)
	//	delete(mAnim);

	//clean up box2d physics body
	if (physBody)
		physBody->GetWorld()->DestroyBody(physBody);

	clearThinks();
}

void AEntity::clearThinks()
{
	//Clean up thinks
	while (this->think) //if there's a think object, remove it
	{
		IThink* th = this->think;
		this->think = NULL;
		delete(th); //StunThinks will reset this->think to previous think in the destructor.
	}
}

void AEntity::evaluate(TIME delta)
{
	update(delta);

	if (!this->nextthink) //no nextthink set
		return;
	if (!this->think) //no think function
		return;
	if (Game::instance().getTime() <= this->nextthink) //not yet time to think
		return;
	
	this->nextthink = 0; //reset nextthink timer
	this->think->think(); //think
}

void AEntity::draw(sf::RenderTarget& target)
{
	if (!this->mDrawable)
		return;
	
	//FIXME: This is bad.
	sf::CircleShape* t = (sf::CircleShape*)mDrawable;
	t->setPosition(mOrigin.toScreenVec());
	target.draw(*mDrawable);
}

void AEntity::updatePosition()
{
	if (!this->physBody)
		return;

	this->mOrigin = Vec2(this->physBody->GetPosition());
	this->mVelocity = Vec2(this->physBody->GetLinearVelocity());
}

void AEntity::createPhysBodyCirc(float radius, bool isSensor, uint16 category, uint16 mask)
{
	b2BodyDef body;
	body.type = b2_dynamicBody; //entities are dynamic
	body.position.Set(mOrigin.x, mOrigin.y);
	body.angle = 0.0f;
	body.linearDamping = 0.0f;
	this->physBody = Game::instance().getMap().getPhysicsWorld()->CreateBody(&body);
	this->physBody->SetUserData(this);

	b2CircleShape physShape;
	physShape.m_radius = radius;

	b2FixtureDef fixture;
	fixture.shape = &physShape;
	fixture.density = 0.05f;
	fixture.isSensor = isSensor;
	fixture.filter.categoryBits = category;
	fixture.filter.maskBits = mask;

	this->physBody->CreateFixture(&fixture);
}


/////////////////////////////////
//Kevin's unique id generator
/////////////////////////////////

EID AEntity::nextID = 1; //start off at 1 (0 has special significance)

/**********************************************************************************************//**
 * Get a unique id (up until whatever type EID is overflows).
 *
 * \return	An ID.
 **************************************************************************************************/
EID AEntity::getUniqueID() {
	return nextID++;
}

/**********************************************************************************************//**
 * Gets the entity id.
 *
 * \return	The entities unique ID.
 **************************************************************************************************/
EID AEntity::getID() const {
	return mID;
}
/////////////////////////////////