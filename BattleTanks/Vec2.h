#pragma once

#include "Defs.h" 

#include <SFML/System/Vector2.hpp>
#include <Box2D/Common/b2Math.h>

#include <cmath>
#include <sstream>

/*
SFML Doesn't actually have any math stuff.
Extend the normal vector class to add in all that good stuff.
*/

template<typename T> class Vector2Ext : public sf::Vector2<T>
{
public:
	Vector2Ext() : sf::Vector2<T>(0, 0) {};
	Vector2Ext(T x, T y) : sf::Vector2<T>(x, y) {};
	Vector2Ext(sf::Vector2<T> const &v) : sf::Vector2<T>(v) {};
	Vector2Ext(b2Vec2 const &v) : sf::Vector2<T>(v.x, v.y) {}; //build vector from b2vec2

	//Normalize vector
	Vector2Ext<T> normalize()
	{
		float len = this->vlen();

		if (len == 0)
			return Vector2Ext<T>();

		return *this * (1 / len);
	}

	//Get vector length
	float vlen() const
	{
		return sqrt(this->x * this->x + this->y * this->y);
	}

	float toAngle() const
	{
		//http://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector
		return (float)(atan2(this->y, this->x) * 180 / MATH_PI);
	}

	
	Vector2Ext<T> getNormal() const;

	static Vector2Ext<T> toVector(float angle);
	static float crossProduct(Vector2Ext<T>, Vector2Ext<T>);

	Vector2Ext<T> toScreenVec() const
	{
		Vector2Ext<T> v = *this;
		v.y = WINDOW_HEIGHT - v.y;
		return v;
	}

	Vector2Ext<T> toScreenVecAlt() const
	{
		Vector2Ext<T> v = *this;
		v.y = v.y - WINDOW_HEIGHT;
		v.x = v.x * -1;
		return v;
	}

	std::string toString() const
	{
		std::stringstream ss;
		ss << "[" << this->x << ", " << this->y << "]";
		return ss.str();
	}

	b2Vec2 toB2Vec2() const
	{
		return b2Vec2(this->x, this->y);
	}

	//Negation operator
	Vector2Ext<T> operator - () const { return Vector2Ext<T>(-this->x, -this->y); }

};

//For convenience, let's just use floats and call it something cool.
typedef Vector2Ext<float> Vec2;

template<typename T> Vector2Ext<T> operator + (const Vector2Ext<T>& left, const Vector2Ext<T>& right) 
{
	Vector2Ext<T> v(left);
	v.x = v.x + right.x;
	v.y = v.y + right.y;
	return v;
}

template<typename T> Vector2Ext<T> operator - (const Vector2Ext<T>& left, const Vector2Ext<T>& right)
{
	Vector2Ext<T> v(left);
	v.x = v.x - right.x;
	v.y = v.y - right.y;
	return v;
}

//Dot product *
template<typename T> float operator * (const Vector2Ext<T>& left, const Vector2Ext<T>& right)
{ 
	return left.x * right.x + left.y * right.y; 
}

//Comparison operator, compare both elements
template<typename T> bool operator == (const Vector2Ext<T>& left, const Vector2Ext<T>& right)
{
	return left.x == right.x && left.y == right.y;
}

template<typename T> Vector2Ext<T> Vector2Ext<T>::toVector(float angle)
{
	return Vector2Ext<T>(cos(angle), sin(angle));
}

template<typename T> float Vector2Ext<T>::crossProduct(Vector2Ext<T> v1, Vector2Ext<T> v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

template<typename T> Vector2Ext<T> Vector2Ext<T>::getNormal() const
{
	return Vector2Ext<T>(this->y, -this->x);
}

