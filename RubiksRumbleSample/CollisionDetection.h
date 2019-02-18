#ifndef _COLLISIONDETECTION_H
#define _COLLISIONDETECTION_H

#include "VitaGefDummies.hpp"
#include "GameObject.h"
#include "Ray.h"

class CollisionDetection
{
public:
	static bool SphereToSphere(GameObject* object1, GameObject* object2);
	static bool AABBToAABB(GameObject* object1, GameObject* object2); // Not Implemented
	static bool RayToOBB(GameObject* object, Ray* ray);
};

#endif
