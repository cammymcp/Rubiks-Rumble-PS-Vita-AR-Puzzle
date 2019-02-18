#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <vector>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "ARCamera.h"
#include "PlayerInput.h"
#include "Tower.h"
#include "HUD.h"
#include "VitaGefDummies.hpp"

#define TURNS_BETWEEN_SPAWNS 4 // How many times can the player destroy a block before more spawn
#define MAX_TOWER_HEIGHT 10 // The height at which the player loses

enum ARMarker
{
	MarkerOne, MarkerTwo, MarkerThree, MarkerFour, MarkerFive, MarkerSix, NumMarkers
};

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class Renderer3D;
	class InputManager;
}

class ARApp : public Dummy::Application
{
public:
	ARApp(Dummy::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frameTime);
	void Render();
	
private:
	// Initialisation
	void InitMenuState();
	void InitPlayState();
	void SetupLights();

	// State Updates
	void UpdateMenuState(float frameTime);
	void UpdatePlayState(float frameTime);
	void UpdateGameOverState(float frameTime);

	// Clean Up
	void CleanUpMenuState();
	void CleanUpPlayState();

	// Game behaviour
	void ProcessTouchInput();
	void CheckForMatches();
	void CheckLoseCondition();
	void SpawnNewBlocks();
	void ResetGame();
	bool AreTapsDisabled();
	void GetFirstMarker();

	// Rendering
	void RenderTower();
	void DrawCameraFeed();

	// Managers
	PlayerInput* playerInput;
	GameState currentGameState;
	HUD* hud;
	Dummy::SpriteRenderer* spriteRenderer;
	class Dummy::Renderer3D* renderer3d;

	// Ar elements
	ARCamera* camera;
	bool markerVisible = false;
	ARMarker currentMarker;

	// 3D objects
	Tower* tower;

	// Flags
	bool gameWasStarted = false;
	bool waitingForMatches = false;
	bool waitingForSpawns = false;
	bool blocksSpawning = false;

	// User hint flags. They are set to true after:
	bool firstBlockDestroyed = false; 
	bool firstMatchFound = false;
	bool tapAfterFirstMatch = false;
	bool twoTapsAfterFirstMatch = false;

	// Counters
	float fps_;
	int score = 0;
	int turnsUntilSpawn = TURNS_BETWEEN_SPAWNS;
};

#endif // _RENDER_TARGET_APP_H