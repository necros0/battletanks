#include "Edge.h"
#include "Sector.h"
#include "DebugDraw.h"

#include <iostream>

Edge::Edge(void) :
	mPoint1(Vec2()),
	mPoint2(Vec2()),
	mIsSolid(true), //default edges to solid...
	mAdjacentSector(NULL) //default unconnected
{
}

Edge::Edge(Vec2 p1, Vec2 p2) :
	mPoint1(p1),
	mPoint2(p2),
	mIsSolid(true), //default edges to solid...
	mAdjacentSector(NULL) //default unconnected
{
}


Edge::~Edge(void)
{
}

float Edge::getLength()
{
	return (this->mPoint2 - this->mPoint1).vlen();
}





// http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool Edge::checkOnSegment(Vec2 p, Vec2 q, Vec2 r) const
{
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
       return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int Edge::getOrientation(Vec2 p, Vec2 q, Vec2 r) const
{
    // See 10th slides from following link for derivation of the formula
    // http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1 : 2; // clock or counterclock wise
}


// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool Edge::checkIntersection(const Edge& other) const
{
	Vec2 p1 = mPoint1;
	Vec2 q1 = mPoint2;
	Vec2 p2 = other.mPoint1;
	Vec2 q2 = other.mPoint2;

    // Find the four orientations needed for general and
    // special cases
    int o1 = getOrientation(p1, q1, p2);
    int o2 = getOrientation(p1, q1, q2);
    int o3 = getOrientation(p2, q2, p1);
    int o4 = getOrientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && checkOnSegment(p1, p2, q1)) return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && checkOnSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && checkOnSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && checkOnSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}





//Doesn't work: well, it does, but it does intersections as if the lines were infinite, which is wrong.
bool Edge::getIntersection(const Edge& e, Vec2& result) const
{
	float d = (this->mPoint1.x-this->mPoint2.x)*(e.mPoint1.y-e.mPoint2.y) 
		- (this->mPoint1.y-this->mPoint2.y)*(e.mPoint1.x-e.mPoint2.x);
	
	//if (fabsf(d) < 0.1f) //allow some play on what is considered 0, otherwise we can get insanely huge numbers if d is small
	//	return false;

	float xi = ((e.mPoint1.x-e.mPoint2.x)*(this->mPoint1.x*this->mPoint2.y-this->mPoint1.y*this->mPoint2.x)
		- (this->mPoint1.x-this->mPoint2.x)*(e.mPoint1.x*e.mPoint2.y-e.mPoint1.y*e.mPoint2.x))/d;
	float yi = ((e.mPoint1.y-e.mPoint2.y)*(this->mPoint1.x*this->mPoint2.y-this->mPoint1.y*this->mPoint2.x)
		- (this->mPoint1.y-this->mPoint2.y)*(e.mPoint1.x*e.mPoint2.y-e.mPoint1.y*e.mPoint2.x))/d;

	result = Vec2(xi,yi);

	return true;
}