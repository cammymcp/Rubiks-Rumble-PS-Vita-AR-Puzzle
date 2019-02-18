#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"

class Block : public GameObject
{
public:
	Block(Dummy::Vec4 position, Dummy::Vec4 scale, Dummy::Vec4 eulerRotation);
	virtual ~Block();

	// Getters
	const uint32_t Colour() { return colour; }

	// Setters
	void SetColour(uint32_t col) { colour = col; }

	// Behaviours
	void MoveTo(Dummy::Vec4 destination, float speed);
	void Update(float frameTime) override;

protected:

	// Behaviour
	Dummy::Vec4 moveDestination;
	bool movingToPosition = false;

	// Rendering
	uint32_t colour;
};

#endif