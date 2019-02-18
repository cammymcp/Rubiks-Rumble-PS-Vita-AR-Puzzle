#ifndef _CAMERA_H
#define _CAMERA_H

#include "VitaGefDummies.hpp"

// Camera class, can be used for 3D scene viewing or AR feed display and scene viewing
class ARCamera
{
public:
	ARCamera();
	~ARCamera();

	void Init(Dummy::Mat4x4 view, Dummy::Mat4x4 projection, float scaleFactor);
	
	// Getters
	Dummy::Mat4x4 ProjectionMatrix() { return projMatrix; }
	Dummy::Mat4x4 ViewMatrix() { return viewMatrix; }
	Dummy::Vec4 EyePosition();
	Dummy::Mat4x4& OrthoProjMatrix() { return orthographicProjMatrix; }
	Dummy::Sprite& FeedSprite() { return feedSprite; }

	// Setters
	void SetProjectionMatrix(Dummy::Mat4x4 newProj) { projMatrix = newProj; }
	void SetViewMatrix(Dummy::Mat4x4 newView) { viewMatrix = newView; }

	// Behaviours
	void UpdateFeed();

protected:
	// 3D Viewing Members
	Dummy::Mat4x4 projMatrix;
	Dummy::Mat4x4 viewMatrix;

	// Feed drawing members
	Dummy::Mat4x4 orthographicProjMatrix;
	Dummy::CameraTexture feedTexture;
	Dummy::Sprite feedSprite;
	float yScaleFactor;
};

#endif 
