#include "CollisionDetection.h"

// Returns true if the two meshes' bounding spheres' are intersecting, false if not
bool CollisionDetection::SphereToSphere(GameObject* object1, GameObject* object2)
{
	// Translate bounding spheres to world pos of the game object's mesh instance
	Dummy::Sphere sphere1 = object1->GetBoundingSphere();
	Dummy::Sphere sphere2 = object2->GetBoundingSphere();
	sphere1 = sphere1.Transform(object1->transform_);
	sphere2 = sphere2.Transform(object2->transform_);

	// Find distance between the two objects (squared so we can avoid a square root calculation)
	Dummy::Vec4 direction = Dummy::Vec4::Subtract(sphere2.position, sphere1.position);
	float distanceSqr = direction.LengthSqr();

	// Get squared sum of two radii
	float radiiSqr = (sphere1.radius * sphere1.radius) + (sphere2.radius * sphere2.radius);

	// If dist < radii then objects are intersecting
	if (distanceSqr < radiiSqr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Checks if the given ray intersects the transform-oriented AABB of the gameobject
// From Real-Time Rendering
bool CollisionDetection::RayToOBB(GameObject* object, Ray* ray)
{
	// Initialise variables
	float tMin = 0.0f; // The farthest "near" intersection found
	float tMax = 100000.0f; // nearest "far" intersection found
	Dummy::Vec4 aabbMin = object->Mesh()->AABBMin();
	Dummy::Vec4 aabbMax = object->Mesh()->AABBMax();

	// Apply game object scale to AABBs
	aabbMin.x = (aabbMin.x * object->Scale().x);
	aabbMin.y = (aabbMin.y * object->Scale().y);
	aabbMin.z = (aabbMin.z * object->Scale().z);
	aabbMax.x = (aabbMax.x * object->Scale().x);
	aabbMax.y = (aabbMax.y * object->Scale().y);
	aabbMax.z = (aabbMax.z * object->Scale().z);

	Dummy::Vec4 obbWorldPosition = object->transform_.GetTranslation();
	Dummy::Vec4 delta = Dummy::Vec4::Subtract(obbWorldPosition, ray->StartPoint());

#pragma region Check X axis
	// Compute intersections with vertical delimiters of OBB X axis. 
	{
		Dummy::Vec4 xAxis(object->transform_.GetRow(0).x, object->transform_.GetRow(0).y, object->transform_.GetRow(0).z, 1.0f);
		xAxis.Normalise();
		float e = xAxis.DotProduct(delta);
		float f = ray->Direction().DotProduct(xAxis);

		if (fabs(f) > 0.001f) // Standard case
		{
			float t1 = (e + aabbMin.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabbMax.x) / f; // Intersection with the "right" plane

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1 > t2)
			{
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// Set the near and far intersection values
			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;

			// If "far" is closer than "near", then there is NO intersection.
			if (tMax < tMin)
				return false;
		}
		else
		{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabbMin.x > 0.0f || -e + aabbMax.x < 0.0f)
				return false;
		}
	}
#pragma endregion

#pragma region Check Y Axis
	// Compute intersections with horizontal delimiters of OBB Y axis. Same as above
	{
		Dummy::Vec4 yAxis(object->transform_.GetRow(1).x, object->transform_.GetRow(1).y, object->transform_.GetRow(1).z, 1.0f);
		yAxis.Normalise();
		float e = yAxis.DotProduct(delta);
		float f = ray->Direction().DotProduct(yAxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabbMin.y) / f;
			float t2 = (e + aabbMax.y) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;
		}
		else
		{
			if (-e + aabbMin.y > 0.0f || -e + aabbMax.y < 0.0f)
				return false;
		}
	}
#pragma endregion

#pragma region Check Z Axis 
	// Compute intersections with inward delimiters of the OBB Z axis. Same again
	{
		Dummy::Vec4 zAxis(object->transform_.GetRow(2).x, object->transform_.GetRow(2).y, object->transform_.GetRow(2).z, 1.0f);
		zAxis.Normalise();
		float e = zAxis.DotProduct(delta);
		float f = ray->Direction().DotProduct(zAxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabbMin.z) / f;
			float t2 = (e + aabbMax.z) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabbMin.z > 0.0f || -e + aabbMax.z < 0.0f)
				return false;
		}
	}
#pragma endregion

	// If we get this far, there is an intersection
	return true;
}