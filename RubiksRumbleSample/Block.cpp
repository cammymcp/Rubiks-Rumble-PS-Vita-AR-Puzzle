#include "Block.h"

Block::Block(Dummy::Vec4 position, Dummy::Vec4 scale, Dummy::Vec4 eulerRotation) :
	GameObject(position, scale, eulerRotation)
{}

Block::~Block()
{}

void Block::Update(float frameTime)
{
	GameObject::Update(frameTime);

	if (movingToPosition)
	{
		// Check if block is at the destination (within a small range)
		float distanceFromDestination = (Dummy::Vec4::Subtract(moveDestination, position)).Length();
		if (distanceFromDestination < 0.002) // 2cm
		{
			SetPosition(moveDestination);
			SetVelocity(Dummy::Vec4());
			movingToPosition = false;
		}
	}
}

// Sets velocity towards given destination and sets flag to be checked in Update() function
void Block::MoveTo(Dummy::Vec4 destination, float speed)
{
	Dummy::Vec4 direction = Dummy::Vec4::Subtract(destination, position);
	direction.Normalise();
	SetVelocity(Dummy::ScaleVector(direction, speed));
	moveDestination = destination;
	movingToPosition = true;
}