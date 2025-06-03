#pragma once

#include "Entity.hpp"
#include "Components.hpp"

struct Intersect
{
	bool intersected = false;
	Vec2f point;
	float t;
};

class Physics
{
public:
	Physics() = default;

	Intersect static LineIntersect(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d)
	{
		Vec2f r = (b - a);
		Vec2f s = (d - c);
		float rxs = r.crossProduct(s);
		Vec2f cma = (c - a);
		float t = (cma.crossProduct(s)) / rxs;
		float u = (cma.crossProduct(r)) / rxs;
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		{
			return { true, Vec2f(a.x + t*r.x, a.y + t*r.y), t };
		}
		else
		{
			return { false, Vec2f(0, 0), 1 };
		}
	}
};