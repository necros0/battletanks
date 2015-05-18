#include "Tank.h"
#include "Game.h"
#include "Telegram.h"
#include "MessageDefs.h"
#include "MessageDispatcher.h"
#include "Info.h"
#include "ThinkTankAI.h"
#include "Shape.h"
#include "Collision.h"
#include "EntityManager.h"
#include "Projectile.h"
#include "Mouse.h"
#include "Utility.h"
#include "SoundAsset.h"
#include "Map.h"
#include "CollisionSolver.h"
#include "DebugDraw.h"
#include "PathFactory.h"
#include "Path.h"
#include "Sector.h"
#include "KineticStrike.h"
#include "GUIController.h"

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>

#define TANK_MAX_SHIELD (20.0f)

Tank::Tank(bool isPlayer) :
	mWishVelocity(Vec2(0, 0)),
	mButtons(0),
	mRadius(TANK_RADIUS),
	AEntity(),
	mHealth(100.0f),
	mShieldPower(0.0f),
	mItems(0),
	mTurretShape(3)
{
	if (isPlayer && getID() != 1)
		throw std::invalid_argument("Player must be spawned first.");

	sf::CircleShape* c = new sf::CircleShape(TANK_RADIUS, 24);
	c->setOrigin(Vec2(TANK_RADIUS, TANK_RADIUS));
	c->setFillColor(sf::Color(190, 0, 0, 255)); //enemy tanks are red
	this->mDrawable = c;
	if (isPlayer)
	{
		this->mouseListener = new MouseListenerTank(*this);
		this->think = new Tank::PlayerThink(*this);
		c->setFillColor(sf::Color(10, 50, 150, 255)); //player is blue
	}
	else
		this->think = new Tank::AIWander(*this); //AI starts off inactive

	mOrigin = Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f);
	mVelocity = Vec2(0, 0);

	//Create the turret graphic
	mTurretShape.setPointCount(3);
	mTurretShape.setPoint(0, Vec2(14, -3));
	mTurretShape.setPoint(1, Vec2(-6, 0));
	mTurretShape.setPoint(2, Vec2(14, 3));
	mTurretShape.setOrigin(Vec2(10, 0));
	if (isPlayer)
		mTurretShape.setFillColor(sf::Color(100, 150, 255, 255)); //player gun is light blue
	else
		mTurretShape.setFillColor(sf::Color(255, 0, 0, 255)); //AI gun is red

	//Create the box2d physics representation
	this->createPhysBodyCirc(TANK_RADIUS, false, SOLID_ENTITY, SOLID_ENTITY | SOLID_PROJECTILE);
	this->physBody->SetLinearDamping(1.2f);

	//Is this an AI tank?
	if (!isPlayer)
		MessageDispatcher::instance().dispatchMessage(0, getID(), -2, MSG_AI_SPAWNED, NULL); //notify game AI was spawned
}


Tank::~Tank(void)
{
	if (this->mouseListener) //if there's a mouse listener
	{
		delete(this->mouseListener); //delete it from memory
		this->mouseListener = NULL;
	}
}

void Tank::setOrigin(Vec2 o)
{
	this->mOrigin = o;
	this->physBody->SetTransform(o.toB2Vec2(), 0.0f);
};

void Tank::update(TIME delta)
{
	Vec2 wishVel = this->mWishVelocity; //if AI is sending velocity, use that, otherwise this sets it to '0 0'
	if (this->mButtons & BUTTON_UP)
		wishVel = wishVel + Vec2(0, 1);
	if (this->mButtons & BUTTON_DOWN)
		wishVel = wishVel + Vec2(0, -1);

	if (this->mButtons & BUTTON_RIGHT)
		wishVel = wishVel + Vec2(1, 0);
	if (this->mButtons & BUTTON_LEFT)
		wishVel = wishVel + Vec2(-1, 0);

	if (this->mButtons & BUTTON_UP ||
		this->mButtons & BUTTON_LEFT ||
		this->mButtons & BUTTON_DOWN ||
		this->mButtons & BUTTON_RIGHT)
	{
		Vec2 normalizedVel = wishVel.normalize();
		wishVel = normalizedVel * TANK_MAX_SPEED;
	}
	this->physBody->ApplyLinearImpulse(wishVel.toB2Vec2(), this->physBody->GetWorldCenter(), true);
	//ApplyForce(wishVel.toB2Vec2(), this->physBody->GetWorldCenter(), true);

	//get only vector in the direction of mWishVelocity
	//Vec2 currentVelocity = (this->mVelocity) * (this->mVelocity.normalize() * normalizedVel);

	//float currentSpeed = currentVelocity.vlen(); //check how fast we're going right now
	//if (currentSpeed < speed) //don't do anything if already moving faster than what we want to add in.
	//	this->mVelocity = this->mVelocity + wishVel * delta;


	//Shape thisShape(this->mRadius, this->mOrigin, this->mVelocity, delta);
	//CollisionSolver::evaluate(*this, thisShape);
}

bool Tank::handleMessage(Telegram const& telegram)
{
	if (this->mHealth <= 0) //dead, don't handle messages.
		return false;

	//Forward telegram to current think function
	if (this->think)
		this->think->handleMessage(telegram);


	//This semi-hack requires the keymove defs to be sequential.  DO NOT FORGET THAT!
	if (telegram.msg >= MSG_P_MOVE_UP && telegram.msg <= MSG_M_MOVE_RIGHT)
	{
		if (telegram.msg == MSG_P_MOVE_UP)
			this->mButtons = this->mButtons | BUTTON_UP;
		else if (telegram.msg == MSG_P_MOVE_RIGHT)
			this->mButtons = this->mButtons | BUTTON_RIGHT;
		else if (telegram.msg == MSG_P_MOVE_DOWN)
			this->mButtons = this->mButtons | BUTTON_DOWN;
		else if (telegram.msg == MSG_P_MOVE_LEFT)
			this->mButtons = this->mButtons | BUTTON_LEFT;
		else if (telegram.msg == MSG_M_MOVE_UP)
			this->mButtons = this->mButtons - (this->mButtons & BUTTON_UP);
		else if (telegram.msg == MSG_M_MOVE_RIGHT)
			this->mButtons = this->mButtons - (this->mButtons & BUTTON_RIGHT);
		else if (telegram.msg == MSG_M_MOVE_DOWN)
			this->mButtons = this->mButtons - (this->mButtons & BUTTON_DOWN);
		else if (telegram.msg == MSG_M_MOVE_LEFT)
			this->mButtons = this->mButtons - (this->mButtons & BUTTON_LEFT);

		return true;
	}
	
	if (telegram.msg == MSG_DEBUFF_STUN) //can't be stunned if dead
	{
		IThink* stun = new StunThink(*this, this->think, 2.0f);
		this->think = stun; //switch to stun behaviour
		return true;
	}
	
	////////// Handle damage message
	if (telegram.msg == MessageType::MSG_DAMAGE)
	{
		struct t_damage* damage = telegram.getInfo<struct t_damage>();
		if (damage->attacker == this->getID()) //don't damage self
			return false;

		//shield absorbs 50% damage
		if (mShieldPower > 0)
		{
			Game::Sound().get("resources/sound/items/shieldHit.wav")->play(1.0f, 0);
			mShieldPower = mShieldPower - damage->damage * 0.5f;
			damage->damage = damage->damage * 0.5f;
			if (mShieldPower < 0) //not enough shields to absorb everything
			{
				mShieldPower = mShieldPower * -1;
				damage->damage = damage->damage + mShieldPower;
				mShieldPower = 0.0f;
			}
		}
		this->mHealth = this->mHealth - damage->damage;
		Vec2 damageVel = (this->mOrigin - damage->origin).normalize() * (damage->damage * 10);
		//send reactive force impulse from explosion
		this->physBody->ApplyLinearImpulse(damageVel.toB2Vec2(), damage->origin.toB2Vec2(), true);

		//DebugDraw::drawPermanent(mOrigin, damage->origin, sf::Color(0, 150, 0, 255));

		if (this->mHealth <= 0) //just died
		{
			this->mButtons = 0; //release all buttons
			this->mWishVelocity = Vec2();

			this->mTurretShape.setFillColor(sf::Color(0, 0, 0, 0));

			if (this->mouseListener)
			{
				delete(this->mouseListener);
				this->mouseListener = NULL;
			}

			clearThinks();

			this->think = new DeathThink(*this);
			this->nextthink = 1; //start exploding next frame
			Game::Sound().get("resources/sound/explosion_1.wav")->play(1.0f, 0);
		}
		
		return true;
	}

	return false;
}

void Tank::draw(sf::RenderTarget& target)
{
	//These changes account for disparity between the way screen space and world space work
	//static_cast because we know it's a circleshape-- it was created in this class.
	//FIXME: This is ugly LEGACY CODE.
	static_cast<sf::CircleShape*>(this->mDrawable)->setPosition(mOrigin.toScreenVec());
	//target.draw(*mDrawable);
	AEntity::draw(target); //TODO: move entire implementation into child classes
	mTurretShape.setPosition(mOrigin.toScreenVec());
	mTurretShape.setRotation(-mTurretAngle + 180); //TODO: At some point, might be nice to track down why this is so effed up.
	target.draw(mTurretShape);

	if (mShieldPower > 0 && mHealth > 0)
	{
		sf::CircleShape shield(TANK_RADIUS + 2, 24);
		shield.setOutlineThickness((mShieldPower / 20) * 2.0f + 1.0f);
		shield.setOutlineColor(sf::Color(255, 255, 0, (int)( (mShieldPower / 20) * 200 ) + 55));
		shield.setFillColor(sf::Color(0, 0, 0, 0));
		shield.setOrigin(TANK_RADIUS + 2, TANK_RADIUS + 2);
		shield.setPosition(mOrigin.toScreenVec());
		target.draw(shield);
	}

	if (mHealth > 0 && Game::instance().demoMode == 0)
	{
		std::stringstream ss;
		ss << mHealth;
		DebugDraw::draw(mOrigin, ss.str());
	}
}

void Tank::shoot(Vec2 dir)
{
	dir = dir.normalize();
	Tank::TankLaser* laser = new Tank::TankLaser(this, this->mOrigin + dir * 4.0f, dir * TANK_LASER_SPEED);
	EntityManager::instance().spawn(laser);

	Game::Sound().get("resources/sound/laserlight.wav")->play(0.5f, 0);
}

Shape Tank::getCollisionShape()
{
	return Shape(this->mRadius, this->mOrigin, this->mVelocity, 0);
}

bool Tank::canSee(Vec2 v)
{
	Trace t = Game::instance().getMap().traceLine(mOrigin, v, false);
	if (t.frac < 1.0f)
		return false;

	return true;
}

bool Tank::canSee(EID oID)
{
	AEntity* oEnt = EntityManager::instance().getEntityByID(oID);
	if (!oEnt) //can't find ent, can't see ent.
		return false;
	Trace t = Game::instance().getMap().traceLine(mOrigin, oEnt->getCollisionShape().oldOrigin, false);
	if (t.frac < 1.0f)
		return false;

	return true;
}

////////////////////////////DEBUG//////////////////////////////////////////
std::string Tank::getOriginString()
{
	std::stringstream ss;
	ss << "[" << this->mOrigin.x << ", " << this->mOrigin.y << "]" << std::endl;
	return ss.str();
}
////////////////////////////DEBUG//////////////////////////////////////////


///// MOUSE LISTENER /////
Tank::MouseListenerTank::MouseListenerTank(Tank& self) :
	MouseListener(Vec2(0, 0), Vec2(WINDOW_WIDTH, WINDOW_HEIGHT)),
	self(self)
{}

void Tank::MouseListenerTank::mousePressed(sf::Event& e)
{
	updatePosition(e.mouseButton.x, e.mouseButton.y);
	setAttack(true);
}

void Tank::MouseListenerTank::mouseReleased(sf::Event& e)
{
	updatePosition(e.mouseButton.x, e.mouseButton.y);
	setAttack(false);
}

void Tank::MouseListenerTank::mouseClick(sf::Event& e)
{
	setAttack(false);
}

void Tank::MouseListenerTank::mouseContextClick(sf::Event& e)
{
	MessageDispatcher::instance().dispatchMessage(0, self.getID(), self.getID(), MSG_POWERUP_KINETIC_USE, new Vec2((float)e.mouseButton.x, (float)e.mouseButton.y));
}

void Tank::MouseListenerTank::mouseHover(sf::Event& e)
{
	updatePosition(e.mouseMove.x, e.mouseMove.y);
}

void Tank::MouseListenerTank::mouseDrag(sf::Event& e)
{
	updatePosition(e.mouseMove.x, e.mouseMove.y);
}

void Tank::MouseListenerTank::setAttack(bool b)
{
	if (b)
		MessageDispatcher::instance().dispatchMessage(0, self.getID(), self.getID(), MSG_PLAYER_ATTACK_ON, NULL);
	else
		MessageDispatcher::instance().dispatchMessage(0, self.getID(), self.getID(), MSG_PLAYER_ATTACK_OFF, NULL);
}

void Tank::MouseListenerTank::updatePosition(int x, int y)
{
	//Vec2* shotDir = new Vec2(Vec2((float)x, (float)y) - self.mOrigin);
	//self.mTurretAngle = shotDir->toAngle();
	self.mTurretTargetVec = Vec2((float)x, (float)y);
	//MessageDispatcher::instance().dispatchMessage(0, -1, 1, MessageType::MSG_PLAYER_UPDATE_ATTACK_TARGET, shotDir);
}

void Tank::MouseListenerTank::mouseMWheelUp(sf::Event& e)
{
	std::cout << "MWheelUp" << std::endl;
}

void Tank::MouseListenerTank::mouseMWheelDown(sf::Event& e)
{
	std::cout << "MWheelDown" << std::endl;
}

///// MOUSE LISTENER /////

///// THINK COMMAND OBJECT (AI) --------------- PURSUE AI /////
Tank::AIPursue::~AIPursue(void)
{
	if (path)
		delete(path);
	path = NULL;
}

void Tank::AIPursue::think()
{
	self.nextthink = Game::instance().getTime() + 0.1f;

	AEntity* enemy = EntityManager::instance().getEntityByID(1);
	if (!enemy) //dead
	{
		self.think = new Tank::AIWander(self);
		delete (this);
		return;
	}

	//Can we see the player?
	if (self.canSee(1))
	{
		self.think = new Tank::AIAttack(self);
		delete (this);
		return;
	}

	//We need to path to the player
	if (Game::instance().getTime() > nextPathCheckTime || !path)
	{
		if (path)
		{
			delete(path);
			path = NULL;
		}

		PathFactory p;
		Map& m = Game::instance().getMap();
		path = p.getPath(Game::instance().getMap().getSector(self.mOrigin), 
			Game::instance().getMap().getSector(enemy->getCollisionShape().oldOrigin)); //Game::instance().getMap().getPlayerSector());
		nextPathCheckTime = Game::instance().getTime() + 5.0f;
	}

	//no path?  stop.
	if (!path)
		return;

	//Path smoothing: as long as we can see the next node, pop current node and try to move directly to next node.
	while(path->peek() != NULL && self.canSee(path->peek()->getCenter()))
	{
		path->pop();
	}

	//Can't see goal anymore, re-path to same goal
	if (!self.canSee(path->getCurrent()->getCenter()))
	{
		delete(path);
		PathFactory p;
		path = p.getPath(Game::instance().getMap().getSector(self.mOrigin),
			Game::instance().getMap().getSector(enemy->getCollisionShape().oldOrigin));
	}

	if ((path->getCurrent()->getCenter() - self.mOrigin).vlen() < 20.0f)
	{
		path->pop();
		//DebugDraw::draw(path->getCurrent()->getCenter(), path->getCurrent()->getCenter().toString());
	}

	if (!path->getCurrent())
	{
		delete(path);
		path = NULL;
		return;
	}


	self.mWishVelocity = (path->getCurrent()->getCenter() - self.mOrigin).normalize() * TANK_MAX_AI_SPEED * 0.75f;
	self.mTurretAngle = self.mVelocity.toAngle();
}
///// THINK COMMAND OBJECT (AI) --------------- PURSUE AI /////

///// THINK COMMAND OBJECT (AI) --------------- ATTACK AI /////
Tank::AIAttack::~AIAttack(void) {}
void Tank::AIAttack::think()
{
	self.nextthink = Game::instance().getTime() + 0.05f;


	AEntity* enemy = EntityManager::instance().getEntityByID(1);
	if (enemy)
	{
		Shape e = enemy->getCollisionShape();

		Vec2 dir = e.oldOrigin - self.mOrigin;
		float dist = dir.vlen();
		float speed = dist / 500;
		if (speed > 1)
			speed = 1;
		else if (speed < 0)
			speed = 0;
		//Just head straight in slowly
		self.mWishVelocity = dir.normalize() * speed * TANK_MAX_AI_SPEED;


		float shotTime = dist / TANK_LASER_SPEED;
		Vec2 targetDir = (e.oldOrigin + e.velocity * shotTime) - self.mOrigin;
		self.mTurretAngle = targetDir.toAngle();

		if (Game::instance().getTime() > nextAttackTime)
		{
			self.shoot(targetDir);
			nextAttackTime = Game::instance().getTime() + 0.1f;
			++numShots;
			if (numShots > 5 || random() < 0.3)
			{
				nextAttackTime = nextAttackTime + 0.8f;
				numShots = 0;
			}
		}
	}



	//Can we see the player?
	if (!self.canSee(1))
	{
		self.think = new Tank::AIPursue(self);
		delete (this);
		return;
	}
}
///// THINK COMMAND OBJECT (AI) --------------- ATTACK AI /////

///// THINK COMMAND OBJECT (AI) --------------- WANDER AI /////
Tank::AIWander::~AIWander(void)
{
	if (path)
		delete(path);
}
void Tank::AIWander::think()
{
	self.nextthink = Game::instance().getTime() + 0.1f;

	float dist = 0;
	if (goal)
		dist = (goal->getCenter() - self.mOrigin).vlen();

	if (!path || dist < 20)
	{
		if (path)
		{
			delete(path);
			path = NULL;
		}

		PathFactory p;
		Map& m = Game::instance().getMap();
		goal = Game::instance().getMap().getRandomSector();
		path = p.getPath(Game::instance().getMap().getSector(self.mOrigin), goal);
	}

	//Wake up
	if (self.canSee(1) && Game::instance().demoMode != 1)
	{
		self.think = new Tank::AIAttack(self);
		delete (this);
		return;
	}

	if (!path)
		return;

	if (Game::instance().demoMode != 0)
		DebugDraw::clear();

	if (Game::instance().demoMode == 1)
		path->draw();

	//Path smoothing: as long as we can see the next node, pop current node and try to move directly to next node.
	while(path->peek() != NULL && self.canSee(path->peek()->getCenter()))
	{
		path->pop();
		if (Game::instance().demoMode == 1)
			DebugDraw::draw(self.mOrigin, path->getCurrent()->getCenter(), sf::Color(255, 0, 0, 255));
	}
	if (Game::instance().demoMode == 1)
		DebugDraw::draw(self.mOrigin, path->getCurrent()->getCenter(), sf::Color(0, 255, 0, 255));

	//Can't see goal anymore, re-path to same goal
	if (!self.canSee(path->getCurrent()->getCenter()))
	{
		delete(path);
		PathFactory p;
		path = p.getPath(Game::instance().getMap().getSector(self.mOrigin), goal);
	}

	if ((path->getCurrent()->getCenter() - self.mOrigin).vlen() < 20.0f)
		path->pop();

	if (!path->getCurrent())
	{
		delete(path);
		path = NULL;
		return;
	}

	self.mWishVelocity = (path->getCurrent()->getCenter() - self.mOrigin).normalize() * TANK_MAX_SPEED * 0.25f; //move slowly
	self.mTurretAngle = self.mVelocity.toAngle();
}
///// THINK COMMAND OBJECT (AI) --------------- WANDER AI /////

///// THINK COMMAND OBJECT (PLAYER) /////
Tank::PlayerThink::~PlayerThink(void)
{}

void Tank::PlayerThink::think()
{
	self.nextthink = 1; //Game::instance().getTime() + 0.05f;

	Vec2 attackDir = self.mTurretTargetVec - self.mOrigin;

	if (mIsFiring && Game::instance().getTime() > nextAttackTime)
	{
		//self.shoot(mAttackPosition);
		self.shoot(attackDir);
		nextAttackTime = Game::instance().getTime() + 0.1f;
	}

	//update turret angle based on target vector
	self.mTurretAngle = attackDir.toAngle();
}

void Tank::PlayerThink::handleMessage(Telegram const &telegram)
{
	/*if (telegram.msg == MSG_PLAYER_UPDATE_ATTACK_TARGET)
	{
		Vec2* v = telegram.getInfo<Vec2>();
		mAttackPosition = *v;
	}
	else*/ if (telegram.msg == MSG_PLAYER_ATTACK_ON)
	{
		mIsFiring = true;
	}
	else if (telegram.msg == MSG_PLAYER_ATTACK_OFF)
	{
		mIsFiring = false;
	}
	else if (telegram.msg == MSG_POWERUP_SHIELD_ACQUIRE)
	{
		self.mShieldPower = TANK_MAX_SHIELD;
		MessageDispatcher::instance().dispatchMessage(0, self.getID(), telegram.sender, MSG_POWERUP_PICKUP, NULL);
		Game::Sound().get("resources/sound/items/shielduse.ogg")->play(1.0f, 0);
		//Notify game that powerup was taken and that it needs to respawn later
		MessageDispatcher::instance().dispatchMessage(Game::instance().getTime() + 15.0f, self.getID(), -2, MSG_POWERUP_SHIELD_RESPAWN, NULL);
	}
	else if (telegram.msg == MSG_POWERUP_KINETIC_ACQUIRE)
	{
		//Pick up missile item if you don't have it already
		if (isItemInInventory(IT_KINETIC))
			return;
		//self.mItems.push_back(IT_KINETIC);
		self.mItems = self.mItems | IT_KINETIC;
		MessageDispatcher::instance().dispatchMessage(0, self.getID(), telegram.sender, MSG_POWERUP_PICKUP, NULL);

		//Notify game that powerup was taken and that it needs to respawn later
		MessageDispatcher::instance().dispatchMessage(Game::instance().getTime() + 15.0f, self.getID(), -2, MSG_POWERUP_KINETIC_RESPAWN, NULL);
	}
	else if (telegram.msg == MSG_POWERUP_KINETIC_USE)
	{
		if (!(self.mItems & IT_KINETIC))
			return;

		self.mItems = self.mItems - (self.mItems & IT_KINETIC);

		Vec2* v = telegram.getInfo<Vec2>();
		EntityManager::instance().spawn(new KineticStrike(*v, self.getID()));
	}
};

bool Tank::PlayerThink::isItemInInventory(int it)
{
	//return std::find(self.mItems.begin(), self.mItems.end(), it) != self.mItems.end();
	return self.mItems & it;
}
///// THINK COMMAND OBJECT (PLAYER) /////



///// THINK COMMAND OBJECT ------------------- DEATH THINK /////
Tank::DeathThink::DeathThink(Tank& o) : self(o), deathTime(Game::instance().getTime()) {};
Tank::DeathThink::~DeathThink(void) {}
void Tank::DeathThink::think()
{
	float explosionTime = 1.4f;
	self.nextthink = Game::instance().getTime() + 0.01f;
	float t = Game::instance().getTime() - this->deathTime;

	if (t > explosionTime)
	{
		if (self.getID() == 1)
		{
			//This is NOT a good spot for this, but it's easier.
			GUIController::instance().setMonitoring(NULL);
			MessageDispatcher::instance().dispatchMessage(Game::instance().getTime() + 5.0f, self.getID(), -2, MSG_PLAYER_DEATH, NULL); //player died
		}
		else
			MessageDispatcher::instance().dispatchMessage(1, self.getID(), -2, MSG_AI_DEATH, NULL); //ai died

		self.nextthink = 0; //stop thinking
		EntityManager::instance().remove(self.getID()); //remove the tank
		return;
	}

	t = sqrt(1.0f - (t / explosionTime));
	float rad = t * 50.0f;
	((sf::CircleShape*)self.mDrawable)->setRadius(rad);
	((sf::CircleShape*)self.mDrawable)->setOrigin(Vec2(rad, rad));
	((sf::CircleShape*)self.mDrawable)->setFillColor(sf::Color((int)(255 * t), 0, 0, (int)(255 * t)));
}
///// THINK COMMAND OBJECT ------------------- DEATH THINK /////


///// THINK COMMAND OBJECT -------------------- STUN THINK /////
Tank::StunThink::StunThink(Tank& o, IThink* th, TIME stunTime) : 
	self(o), 
	lastThink(th), 
	stunEndTime(Game::instance().getTime() + stunTime)
{}
Tank::StunThink::~StunThink(void)
{
	self.think = lastThink;
	self.nextthink = 1; //restart thinking
}
void Tank::StunThink::think()
{
	if (Game::instance().getTime() > stunEndTime)
	{
		delete(this);
		return;
	}

	self.mButtons = 0;
	self.mWishVelocity = Vec2();

	self.nextthink = Game::instance().getTime() + 0.1f;
}
///// THINK COMMAND OBJECT -------------------- STUN THINK /////



///// TANK LASER PROJECTILE /////
Tank::TankLaser::TankLaser(Tank* owner, Vec2 org, Vec2 vel) :
	removeTime(Game::instance().getTime() + 15.0f) //expires after 15 seconds
{
	this->owner = owner;
	this->mOrigin = org + vel.normalize() * (2.0f); //spawn a little inside tank's body
	this->mVelocity = vel;

	//Useless
	//this->flags = this->flags | FL_NONSOLID | FL_MOVETYPE_FLY;

	//Create the box2d physics representation
	this->createPhysBodyCirc(0.1f, false, SOLID_PROJECTILE, SOLID_ENTITY);
	this->physBody->SetLinearDamping(0.0f);
	this->physBody->SetLinearVelocity(vel.toB2Vec2());
}

void Tank::TankLaser::update(TIME delta)
{
	if (Game::instance().getTime() > removeTime)
	{
		EntityManager::instance().remove(this->getID());
		return;
	}
	//std::stringstream ss;
	//ss << this->mVelocity.vlen();
	//DebugDraw::draw(this->mOrigin, ss.str());
}

bool Tank::TankLaser::handleMessage(Telegram const &telegram)
{
	if (telegram.msg == MessageType::MSG_COLLISION)
	{
		Vec2* v = telegram.getInfo<Vec2>();
		struct t_damage* t = new struct t_damage(5, this->getOwner()->getID(), *v);
		//Send a damage message to the entity we've hit
		MessageDispatcher::instance().dispatchMessage(0, this->getID(), telegram.sender, MessageType::MSG_DAMAGE, t);

		//We've collided, remove self and play explosion sound
		if (random() < 1.0/3.0f)
			Game::Sound().get("resources/sound/explosion_distortion_muffled.wav")->play(0.7f, 0);
		else if (random() < 1.0/3.0f)
			Game::Sound().get("resources/sound/explosion_distortion_muffled2.wav")->play(0.7f, 0);
		else
			Game::Sound().get("resources/sound/explosion_distortion_muffled3.wav")->play(0.7f, 0);

		EntityManager::instance().remove(this->getID()); //remove bolt
	}


	return true;
}

void Tank::TankLaser::draw(sf::RenderTarget& target)
{
	/*sf::Vertex line[2];
	line[0] = sf::Vertex(this->mOrigin.toScreenVec());
	line[0].color = sf::Color(255, 0, 0, 255);
	Vec2 end = this->mOrigin + this->mVelocity.normalize() * -12.0f;
	line[1] = sf::Vertex(end.toScreenVec());
	line[1].color = sf::Color(255, 0, 0, 50);
	target.draw(line, 2, sf::Lines);*/

	sf::ConvexShape laser;

	laser.setPointCount(4);
	laser.setPoint(0, Vec2(-8, -1));
	laser.setPoint(1, Vec2(6, 0));
	laser.setPoint(2, Vec2(-8, 1));
	laser.setPoint(3, Vec2(-9, 0));
	laser.setOrigin(Vec2(-4, 0));
	laser.setPosition(this->mOrigin.toScreenVec());
	laser.setRotation(-this->mVelocity.toAngle() + 180.0f);

	target.draw(laser);

	/*sf::CircleShape s(6, 10);
	s.setOrigin(3, 3);
	s.setPosition(mOrigin.toScreenVec());
	target.draw(s);*/
}

Shape Tank::TankLaser::getCollisionShape()
{
	//lasers are point entities
	return Shape(0, this->mOrigin, this->mVelocity, 0);
}

///// TANK LASER PROJECTILE /////