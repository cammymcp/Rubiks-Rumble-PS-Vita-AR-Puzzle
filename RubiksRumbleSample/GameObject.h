#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "VitaGefDummies.hpp"

// Class that adds additional functionality and helper functions for interacting with mesh instances
class GameObject : public Dummy::MeshInstance
{
public:
	GameObject(Dummy::Vec4 position, Dummy::Vec4 scale, Dummy::Vec4 eulerRotation);
	GameObject();
	virtual ~GameObject();

	// Getters
	Dummy::Vec4 Position() { return position; }
	Dummy::Vec4 Scale() { return scale; }
	Dummy::Quaternion Rotation() { return rotation; }
	Dummy::Vec4 EulerRotation();
	Dummy::Vec4 Velocity() { return velocity; }
	Dummy::Mat4x4 LocalTransform() { return localTransform; }
	bool IsActive() { return active; }

	// Setters
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotationEuler(float xRad, float yRad, float zRad);
	void SetPosition(Dummy::Vec4 pos);
	void SetScale(Dummy::Vec4 sca);
	void SetRotation(Dummy::Quaternion rot);
	// Sets the per-second velocity of this game object
	// This object's update function will move the object at this velocity when called each frame.
	void SetVelocity(Dummy::Vec4 vel) { velocity = vel; }
	void SetLocalTransform(Dummy::Mat4x4 newTransform) { localTransform = newTransform; }
	void SetActive(bool newActive) { active = newActive; }

	// Behaviours
	virtual void Update(float frameTime);
	void BuildTransform();
	void BuildLocalTransform();
	float DistanceFrom(Dummy::Vec4 testPos);

protected:
	Dummy::Vec4 position;
	Dummy::Vec4 scale;
	Dummy::Quaternion rotation;
	Dummy::Vec4 velocity;
	Dummy::Mat4x4 localTransform;
	bool active = true;
};
#endif