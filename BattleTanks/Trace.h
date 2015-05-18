#pragma once

struct trace
{
	trace(void) : 
		startPoint(Vec2()),
		endPoint(Vec2()),
		frac(0.0f)
	{};
	~trace(void) {};

	Vec2 startPoint;
	Vec2 endPoint;
	float frac;

} typedef Trace;

