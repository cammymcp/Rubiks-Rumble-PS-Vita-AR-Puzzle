#include "ARCamera.h"
#include "VitaGefDummies.hpp"

// Constructor for AR camera feed app
ARCamera::ARCamera()
{
	// Set up orthographic matrix for sprite rendering
	orthographicProjMatrix = Dummy::GLOrthoFrustum(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

ARCamera::~ARCamera()
{
}

void ARCamera::Init(Dummy::Mat4x4 view, Dummy::Mat4x4 projection, float scaleFactor)
{
	// Set up scale matrix
	Dummy::Mat4x4 scaleMatrix;
	scaleMatrix.SetIdentity();
	scaleMatrix.SetEntry(1, 1, scaleFactor);

	// Scale projection matrix 
	Dummy::Mat4x4 scaledProjectionMatrix = Dummy::Mat4x4::Mult(projection, scaleMatrix);

	// Set matrices
	SetProjectionMatrix(scaledProjectionMatrix);
	SetViewMatrix(view);

	// Set up camera feed sprite
	feedSprite.SetWidth(2.0f);
	feedSprite.SetHeight(2.0f*scaleFactor);
	feedSprite.SetPosition(Dummy::Vec4(0.0f, 0.0f, 1.0f));
}

// Gets the camera image data from the device
void ARCamera::UpdateFeed()
{
	feedTexture = Dummy::GetCameraImageTexture();
	feedSprite.SetTexture(&feedTexture);
}

// Returns translation vector of view matrix
Dummy::Vec4 ARCamera::EyePosition()
{
	Dummy::Vec4 translation = viewMatrix.GetTranslation();
	return translation;
}