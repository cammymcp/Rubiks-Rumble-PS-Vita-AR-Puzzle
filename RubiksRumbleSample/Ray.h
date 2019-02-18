#ifndef RAY_H
#define RAY_H

#include "VitaGefDummies.hpp"

class Ray
{
public:
	Ray();
	Ray(Dummy::Vec4 start, Dummy::Vec4 dir);
	~Ray();

	Dummy::Vec4 StartPoint() { return startPoint; }
	Dummy::Vec4 Direction() { return direction; }

protected:
	Dummy::Vec4 startPoint;
	Dummy::Vec4 direction;
};

#endif