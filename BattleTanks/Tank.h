#pragma once

#include "AEntity.h"
#include "MouseListener.h"
#include "IThink.h"
#include "IPhysObject.h"



#define TANK_MAX_SPEED (100.0f)
#define TANK_MAX_AI_SPEED (40.0f)
#define TANK_RADIUS (10.0f)
#define TANK_ACCEL_SPEED (700.0f)
#define TANK_DECEL_RATE (5.0f)
#define TANK_LASER_SPEED (525.0f)

#include <string>
#include <list>

class Shape;
class Sector;
class Path;

class Tank : public AEntity
{
	friend class ThinkTankAI;
	friend class GUIController; //allow easy monitoring of player
public:
	Tank(bool isPlayer);
	~Tank(void);

	virtual void update(TIME delta);
	virtual bool handleMessage(Telegram const &telegram);
	virtual void draw(sf::RenderTarget& target);

	void setOrigin(Vec2 o);
	std::string getOriginString();
	
	virtual Shape getCollisionShape();

private:
	float mRadius;
	Vec2 mWishVelocity;
	float mTurretAngle;
	Vec2 mTurretTargetVec;

	float mFusionPower;
	float mShieldPower; //this would be part of the subsystem stuff, but we'll just haxor it in like this for now
	//std::list<int> mItems;
	int mItems;
	sf::ConvexShape mTurretShape;

	//State* mState;
	//std::vector<ASubSystem*> mSubSystems;

	int mButtons;
	static const int BUTTON_UP = 1;
	static const int BUTTON_DOWN = 2;
	static const int BUTTON_LEFT = 4;
	static const int BUTTON_RIGHT = 8;

	float mHealth;

	void shoot(Vec2 targetOrigin);
	bool canSee(EID);
	bool canSee(Vec2);

	enum ItemFlags
	{
		IT_SHIELD = 1,
		IT_KINETIC = 2
	};

	//Implementation for the tank mouse listener
	class MouseListenerTank : public MouseListener
	{
	public:
		MouseListenerTank(Tank& self);
		~MouseListenerTank() {};

		virtual void mousePressed(sf::Event& e) override;
		virtual void mouseReleased(sf::Event& e) override;
		virtual void mouseClick(sf::Event& e) override;
		virtual void mouseContextClick(sf::Event& e) override;
		virtual void mouseHover(sf::Event& e) override;
		virtual void mouseDrag(sf::Event& e) override;
		virtual void mouseMWheelUp(sf::Event& e) override;
		virtual void mouseMWheelDown(sf::Event& e) override;

		void setAttack(bool);
		void updatePosition(int, int);

	private:
		Tank& self;
	};

	//Think object for AIs
	class AIPursue : public IThink
	{
	public:
		AIPursue(Tank& o) : self(o), path(NULL), nextPathCheckTime(0.0f) {};
		~AIPursue(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram) {};

	private:
		Tank& self;
		Path* path;
		TIME nextPathCheckTime;
	};

	class AIAttack : public IThink
	{
	public:
		AIAttack(Tank& o) : self(o), nextAttackTime(0.0f), targetRefinementVec(), numShots(0) {};
		~AIAttack(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram) {};

	private:
		Tank& self;
		TIME nextAttackTime;
		Vec2 targetRefinementVec;
		int numShots;
	};

	class AIWander : public IThink
	{
	public:
		AIWander(Tank& o) : self(o), path(NULL), goal(NULL) {};
		~AIWander(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram) {};

	private:
		Tank& self;
		Path* path;
		Sector* goal;
	};

	//Think object for players
	class PlayerThink : public IThink
	{
	public:
		PlayerThink(Tank& o) : self(o), mIsFiring(false), nextAttackTime(0), mAttackPosition() {};
		~PlayerThink(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram);

	private:
		Tank& self;
		bool mIsFiring;
		TIME nextAttackTime;
		Vec2 mAttackPosition;
		
		bool isItemInInventory(int it);
	};

	//Think object for death
	class DeathThink : public IThink
	{
	public:
		DeathThink(Tank& o);
		~DeathThink(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram) {};

	private:
		Tank& self;
		TIME deathTime;
	};

	//Think object for stun effect
	class StunThink : public IThink
	{
	public:
		StunThink(Tank& o, IThink* th, TIME stunTime);
		~StunThink(void);

		virtual void think();
		virtual void handleMessage(Telegram const &telegram) {};

	private:
		Tank& self;
		TIME stunEndTime;
		IThink* lastThink;
	};

	//Tank Laser
	class TankLaser : public AEntity
	{
	public:
		TankLaser(Tank* owner, Vec2 org, Vec2 vel);

		virtual void update(TIME delta);
		virtual bool handleMessage(Telegram const &telegram);
		virtual void draw(sf::RenderTarget& target);
		virtual Shape getCollisionShape();

	public:
		TIME removeTime;
	};
};
