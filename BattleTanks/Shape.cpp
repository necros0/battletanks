#include "Shape.h"
#include "Collision.h"
#include "DebugDraw.h"
#include "Defs.h"


#include <cmath>
#include <iostream>

Shape::Shape(float radius, Vec2 org, Vec2 vel, float delta) :
	radius(radius),
	oldOrigin(org),
	velocity(vel),
	delta(delta),
	newOrigin(org + vel * delta)
{
}


Shape::~Shape(void)
{
}

//Huge thanks to this:
//http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2
bool Shape::getCollision(Shape& other, Collision& col)
{
	Shape& self = *this;

	Vec2 v = self.velocity * self.delta;
	
	// Early Escape test: if the length of the movevec is less
	// than distance between the centers of these circles minus
	// their radii, there's no way they can hit.
	float dist = (other.newOrigin - self.oldOrigin).vlen();
	float sumRadii = (other.radius + self.radius);
	dist -= sumRadii;
	if(v.vlen() < dist) //velocity is shorter than current distance (minus radius of the circles)
		return false;			//can't hit

	// Normalize the movevec
	Vec2 n = v.normalize();

	// Find C, the vector from the center of the moving
	// circle A to the center of B
	Vec2 c = other.newOrigin - self.oldOrigin;

	// D = N . C = ||C|| * cos(angle between N and C)
	float D = n * c;

	// Another early escape: Make sure that A is moving
	// towards B! If the dot product between the movevec and
	// B.center - A.center is less that or equal to 0,
	// A isn't isn't moving towards B
	if(D <= 0)
		return false;

	// Find the length of the vector C
	float lengthC = c.vlen();

	float F = (lengthC * lengthC) - (D * D);

	// Escape test: if the closest that A will get to B
	// is more than the sum of their radii, there's no
	// way they are going collide
	float sumRadiiSquared = sumRadii * sumRadii;
	if(F >= sumRadiiSquared)
		return false;


	// We now have F and sumRadii, two sides of a right triangle.
	// Use these to find the third side, sqrt(T)
	float T = sumRadiiSquared - F;

	// If there is no such right triangle with sides length of
	// sumRadii and sqrt(f), T will probably be less than 0.
	// Better to check now than perform a square root of a
	// negative number.
	if(T < 0)
		return false;

	// Therefore the distance the circle has to travel along
	// movevec is D - sqrt(T)
	float distance = D - sqrtf(T);

	// Get the magnitude of the movement vector
	float mag = v.vlen();

	// Finally, make sure that the distance A has to move
	// to touch B is not greater than the magnitude of the
	// movement vector.
	if(mag < distance)
		return false;

	// Set the length of the movevec so that the circles will just
	// touch
	v = v.normalize();
	v = v * distance;

	col.minimumTranslation = v;
	Vec2 safeOrigin = self.oldOrigin + v;
	col.origin = safeOrigin + (other.newOrigin - safeOrigin).normalize() * (self.radius + COLLISION_POSITION_EPSILON);
	col.normal = (other.newOrigin - col.origin).normalize() * self.radius;


	
	//DebugDraw::clear();
	//DebugDraw::draw(other.newOrigin, other.newOrigin + col.normal);

	//std::cout << "Shape Collision: '" << col.normal.x << ", " << col.normal.y << "'" << std::endl;
	//std::cout << "'" << col.normal.x << ", " << col.normal.y << "'" << std::endl;
	return true;
}
