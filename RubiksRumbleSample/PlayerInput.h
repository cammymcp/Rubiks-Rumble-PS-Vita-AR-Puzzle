#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include "Ray.h"
#include "ARCamera.h"
#include "Tower.h"
#include "CollisionDetection.h"
#include "VitaGefDummies.hpp"

class PlayerInput
{
public:
	PlayerInput();
	void Init(Dummy::Platform& platform);
	void Update();
	bool CheckForTaps(ARCamera* camera, Tower* tower, bool tapsDisabled);
	bool ProcessTouchInput(ARCamera* camera, Tower* tower);
	const Dummy::Controller* GetController();
	void GetTouchRay(Ray& outRay, const Dummy::Mat4x4& projection, const Dummy::Mat4x4& view);

protected:
	Dummy::Input* inputManager;
	uint32_t width, height;
	Dummy::Vec2 touchPosition;
};

#endif