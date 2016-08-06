#ifndef __PLANE_H__
#define __PLANE_H__
#include "vector3.h"


class Plane
{
public:
	Plane()
		:normal(0, 0, 0), d(0)
	{}
	Plane(vector3df _normal, float _d)
		:normal(_normal), d(_d)
	{}

	vector3df normal;
	float d;
};
#endif