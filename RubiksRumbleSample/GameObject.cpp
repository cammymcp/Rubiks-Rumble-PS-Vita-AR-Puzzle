#include "GameObject.h"
#include <math.h>

#pragma region Constructor/Destructor
GameObject::GameObject(Dummy::Vec4 position, Dummy::Vec4 scale, Dummy::Vec4 eulerRotation)
{
	localTransform.SetIdentity();
	SetScale(scale);
	SetRotationEuler(eulerRotation.x, eulerRotation.y, eulerRotation.z);
	SetPosition(position);
	SetVelocity(Dummy::Vec4());
}

GameObject::GameObject()
{
	localTransform.SetIdentity();
	SetVelocity(Dummy::Vec4());
}

GameObject::~GameObject()
{}
#pragma endregion

#pragma region Getters
Dummy::Vec4 GameObject::EulerRotation()
{
	// Quaternion to Euler. Not implemented
	return Dummy::Vec4();
}
#pragma endregion

#pragma region Setters
void GameObject::SetPosition(float x, float y, float z)
{
	// Rebuild the mesh instance transform with new translation values
	position = Dummy::Vec4(x, y, z);
	BuildTransform();
	BuildLocalTransform();
}

void GameObject::SetPosition(Dummy::Vec4 pos)
{
	// Rebuild the mesh instance transform with new translation values
	position = pos;
	BuildTransform();
	BuildLocalTransform();
}

void GameObject::SetScale(float x, float y, float z)
{
	// Rebuild the mesh instance transform with new scale values
	scale = Dummy::Vec4(x, y, z);
	BuildTransform();
	BuildLocalTransform();
}

void GameObject::SetScale(Dummy::Vec4 sca)
{
	// Rebuild the mesh instance transform with new scale values
	scale = sca;
	BuildTransform();
	BuildLocalTransform();
}

void GameObject::SetRotationEuler(float xRad, float yRad, float zRad)
{
	// Set up quaternion component calculation
	float c1 = cos(yRad / 2);
	float s1 = sin(yRad / 2);
	float c2 = cos(zRad / 2);
	float s2 = sin(zRad / 2);
	float c3 = cos(xRad / 2);
	float s3 = sin(xRad / 2);
	float c1c2 = c1 * c2;
	float s1s2 = s1 * s2;

	// Create quaternion and calculate components
	Dummy::Quaternion newRotation;
	newRotation.w = (c1c2 * c3) + (s1s2 * s3);
	newRotation.x = (c1c2 * s3) + (s1s2 * c3);
	newRotation.y = (s1 * c2*c3) + (c1 * s2*s3);
	newRotation.z = (c1 * s2*c3) - (s1 * c2*s3);

	rotation = newRotation;
	BuildTransform();
	BuildLocalTransform();
}

void GameObject::SetRotation(Dummy::Quaternion rot)
{
	rotation = rot;
	BuildTransform();
	BuildLocalTransform();
}
#pragma endregion

#pragma region Behaviours
void GameObject::Update(float frameTime)
{
	// Resolve velocity
	Dummy::Vec4 newPosition = Dummy::AddVectors(position, (Dummy::ScaleVector(velocity, frameTime)));
	SetPosition(newPosition);
}

void GameObject::BuildTransform()
{
	Dummy::Mat4x4 newTransform, newScale, newRotation;
	newScale.SetScale(scale);
	newRotation.SetRotation(rotation);
	newTransform = Dummy::Mat4x4::Mult(newScale, newRotation);
	newTransform.SetTranslation(position);
	transform_ = newTransform;
}

void GameObject::BuildLocalTransform()
{
	Dummy::Mat4x4 newTransform, newScale, newRotation;
	newScale.SetScale(scale);
	newRotation.SetRotation(rotation);
	newTransform = Dummy::Mat4x4::Mult(newScale, newRotation);
	newTransform.SetTranslation(position);
	localTransform = newTransform;
}

float GameObject::DistanceFrom(Dummy::Vec4 testPos)
{
	Dummy::Vec4 direction = Dummy::Vec4::Subtract(testPos, position);
	return direction.LengthSqr();
}
#pragma endregion