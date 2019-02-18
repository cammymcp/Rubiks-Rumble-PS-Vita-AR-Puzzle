#include "Ray.h"

Ray::Ray(Dummy::Vec4 start, Dummy::Vec4 dir)
{
	startPoint = start;
	direction = dir;
	direction.Normalise();
}

Ray::Ray()
{
}

Ray::~Ray()
{
}