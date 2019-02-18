#include "PlayerInput.h"

PlayerInput::PlayerInput()
	: inputManager(NULL)
{}

void PlayerInput::Init(Dummy::Platform& platform)
{
	inputManager = Dummy::Input::Create();
	width = platform.width;
	height = platform.height;

	Dummy::EnableFrontTouchPanel();

	// Init touch values
	touchPosition = Dummy::Vec2(0.0F, 0.0F);
} 

void PlayerInput::Update()
{
	// Updates input manager
	inputManager->Update();
}

// Informs the application if the user performed a valid move or not
bool PlayerInput::CheckForTaps(ARCamera* camera, Tower* tower, bool tapsDisabled)
{
	// Process touches if allowed
	if (tapsDisabled == false)
		return ProcessTouchInput(camera, tower);
	else
		return false;
}

// Returns true if the user tapped on a block that is in play
bool PlayerInput::ProcessTouchInput(ARCamera* camera, Tower* tower)
{
	// Check for touch input
	if (inputManager)
	{
		// Get touch manager
		const Dummy::TouchInput* touchInput = inputManager->GetTouchInput();

		// Iterate through all detected touches
		const std::vector<Dummy::Touch> touches = touchInput->Touches();
		for (std::vector<Dummy::Touch>::const_iterator touch = touches.begin(); touch != touches.end(); touch++)
		{
			// Check for a new touch
			if (touch->type == Dummy::TOUCH_NEW)
			{
				// Store touch position
				touchPosition = touch->position;

				// Create a world-space ray from touch position
				Ray outRay;
				GetTouchRay(outRay, camera->ProjectionMatrix(), camera->ViewMatrix());

				// Check if ray intersects with blocks
				Block* touchedBlock = NULL;
				Column* parentColumn = NULL;
				int parentWall = 0;
				int wallIndex = 0;
				for (auto wall : tower->Walls())
				{
					for (auto column : wall->Columns())
					{
						for (auto block : column->Blocks())
						{ 
							if (CollisionDetection::RayToOBB(block, &outRay))
							{
								// Store block if there isn't another positive intersection yet
								if (touchedBlock == NULL)
								{
									touchedBlock = block;
									parentColumn = column;
									parentWall = wallIndex;
								}
								// If there is, check if this one is closer to the camera
								else if (block->DistanceFrom(camera->EyePosition()) > touchedBlock->DistanceFrom(camera->EyePosition()))
								{
									touchedBlock = block;
									parentColumn = column;
									parentWall = wallIndex;
								}
							}
						}
					}
					wallIndex++;
				}
				// Check if the selection is a valid move
				if (touchedBlock != NULL && parentColumn != NULL && parentColumn->IsSelectionValid(touchedBlock))
				{
					// Remove block from the list and collapse the blocks above it
					parentColumn->RemoveBlock(touchedBlock, parentWall);
					return true;
				}
			}
		}
	}
	return false;
}

const Dummy::Controller* PlayerInput::GetController()
{
	return inputManager->GetController();
}

// Takes a touch-point in screen space and constructs a world space ray for object intersection tests
void PlayerInput::GetTouchRay(Ray& outRay, const Dummy::Mat4x4& projection, const Dummy::Mat4x4& view)
{
	if (inputManager && inputManager->GetTouchInput())
	{
		Dummy::Vec2 touchPos = touchPosition;
		Dummy::Vec2 ndc(0.0f, 0.0f);

		float halfW = width * 0.5f;
		float halfH = height * 0.5f;

		ndc.x = (static_cast<float>(touchPosition.x) - halfW) / halfW;
		ndc.y = (halfH - static_cast<float>(touchPosition.y)) / halfH;

		Dummy::Mat4x4 projectionInverse;
		projectionInverse.Inverse(Dummy::Mat4x4::Mult(view, projection));

		Dummy::Vec4 nearPoint, farPoint;

		nearPoint = Dummy::Vec4(ndc.x, ndc.y, -1.0f, 1.0f).TransformW(projectionInverse);

		farPoint = Dummy::Vec4(ndc.x, ndc.y, 1.0f, 1.0f).TransformW(projectionInverse);

		nearPoint.DivideBy(nearPoint.w);
		farPoint.DivideBy(farPoint.w);

		Ray ray(Dummy::Vec4(nearPoint.x, nearPoint.y, nearPoint.z), Dummy::Vec4::Subtract(farPoint, nearPoint));
		outRay = ray;
	}
}