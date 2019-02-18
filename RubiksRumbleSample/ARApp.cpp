#include "ARApp.h"
#include <iostream> // cout

ARApp::ARApp(Dummy::Platform& platform) :
	Application(platform),
	spriteRenderer(NULL),
	playerInput(NULL),
	hud(NULL),
	camera(NULL),
	tower(NULL),
	renderer3d(NULL)
{
}

void ARApp::Init()
{
	InitMenuState();
}

void ARApp::InitMenuState()
{
	// Set up game state
	currentGameState = GameState::MENU;

	// Call ARCamera constructor here to ensure the orthographic proj matrix is initialised for menu rendering
	camera = new ARCamera();

	// Renderers
	spriteRenderer = Dummy::SpriteRenderer::Init();
	hud = new HUD(spriteRenderer);
	hud->Init(platform_, (int)MAX_TOWER_HEIGHT, (int)MIN_MATCH);		

	// Input manager
	playerInput = new PlayerInput();
	playerInput->Init(platform_);
}

void ARApp::InitPlayState()
{
	Dummy::InitARFramework();

	// Renderering
	renderer3d = Dummy::Renderer3D::Init();
	SetupLights();

	// Init Camera
	Dummy::Mat4x4 projectionMatrix;
	Dummy::Mat4x4 viewMatrix;
	float yScaleFactor = (960.0f / 544.0f) / ((float)Dummy::CAMERA_IMAGE_WIDTH / (float)Dummy::CAMERA_IMAGE_HEIGHT);
	projectionMatrix = Dummy::GetProjectionMatrix(Dummy::CAMERA_IMAGE_FOV, (float)Dummy::CAMERA_IMAGE_WIDTH / Dummy::CAMERA_IMAGE_HEIGHT, 0.1f, 10.0f);
	viewMatrix = Dummy::GetCameraViewMatrix();
	camera->Init(viewMatrix, projectionMatrix, yScaleFactor);

	// Seed rand for block colour selection
	srand(time(NULL));

	// Set up 3D objects
	tower = new Tower();
	tower->Init(platform_);

	// Start scene mapping
	Dummy::ResetARSampling();
}

void ARApp::CleanUp()
{
	CleanUpMenuState();
	if (gameWasStarted) CleanUpPlayState();
}

void ARApp::CleanUpMenuState()
{
	delete spriteRenderer;
	spriteRenderer = NULL;

	delete camera;
	camera = NULL;

	hud->CleanUp();
	delete hud;
	hud = NULL;

	delete playerInput;
	playerInput = NULL;
}

void ARApp::CleanUpPlayState()
{
	Dummy::CleanUpARFramework();

	delete renderer3d;
	renderer3d = NULL;

	tower->CleanUp();
	delete tower;
	tower = NULL;
}

bool ARApp::Update(float frameTime)
{
	fps_ = 1.0f / frameTime;

	// Update the input manager
	playerInput->Update();
	
	// Call the state-appropriate update function
	switch (currentGameState)
	{
	case GameState::MENU :
		UpdateMenuState(frameTime);
		break;
	case GameState::PLAY:
		UpdatePlayState(frameTime);
		break;
	case GameState::GAMEOVER:
		UpdateGameOverState(frameTime);
		break;
	}

	return true;
}

void ARApp::UpdateMenuState(float frameTime)
{
	// Check for appropriate controller input
	const Dummy::Controller* controller = playerInput->GetController();
	if (controller)
	{
		if (controller->ButtonsDown() & Dummy::BUTTON_CROSS)
		{
			// Initialise and start game
			InitPlayState();
			currentGameState = GameState::PLAY;
			gameWasStarted = true;
		}
	}
}

void ARApp::UpdatePlayState(float frameTime)
{
	ProcessTouchInput();

	CheckForMatches();

	SpawnNewBlocks(); // internally checks if the user is out of turns

	// Update the tower
	tower->Update(frameTime);

	// Update AR Tracking and move tower to marker position
	GetFirstMarker();

	Dummy::UpdateARSampling();
}

void ARApp::UpdateGameOverState(float frameTime)
{
	// Check for appropriate controller input
	const Dummy::Controller* controller = playerInput->GetController();
	if (controller)
	{
		if ((currentGameState == GAMEOVER) && (controller->ButtonsDown() & Dummy::BUTTON_CROSS))
			ResetGame();
	}

	// Update the tower (to finish any block movements)
	tower->Update(frameTime);

	// We still want to update the sample to continue tracking while in gameover state
	GetFirstMarker();
	Dummy::UpdateARSampling();
}

void ARApp::Render()
{
	// Set orthographic projection matrix for sprite renderer (required for all game states)
	spriteRenderer->SetProjectionMatrix(camera->OrthoProjMatrix());
	spriteRenderer->Begin();

	// Render state specific objects
	if (currentGameState == PLAY || currentGameState == GAMEOVER)
	{
		Dummy::BeginARSampleRender();

		DrawCameraFeed();

		RenderTower();

		hud->Draw(platform_, currentGameState, tower->CurrentHeight(), score, turnsUntilSpawn, currentGameState == GAMEOVER, markerVisible, blocksSpawning);
	}
	else if (currentGameState == GameState::MENU)
		hud->Draw(platform_, currentGameState);
}

// Handles player taps, we pass a flag to the PlayerInput function to disable taps until a move is valid
void ARApp::ProcessTouchInput()
{
	if (playerInput->CheckForTaps(camera, tower, AreTapsDisabled()))
	{
		// A valid tap was made, we need to wait for matches to be checked and new blocks to
		// be spawned before the player can make another move
		waitingForMatches = true;
		if (turnsUntilSpawn > 0)
			turnsUntilSpawn--;
		if (turnsUntilSpawn == 0)
			waitingForSpawns = true;

		// Check for final hint completion
		if (!twoTapsAfterFirstMatch && tapAfterFirstMatch && firstMatchFound && firstBlockDestroyed)
		{
			twoTapsAfterFirstMatch = true;
			hud->NextHint();
		}

		// Check for third hint completion
		if (!tapAfterFirstMatch && firstMatchFound && firstBlockDestroyed)
		{
			tapAfterFirstMatch = true;
			hud->NextHint();
		}

		// Check for first hint completion
		if (!firstBlockDestroyed)
		{
			firstBlockDestroyed = true;
			hud->NextHint();
		}
	}
}

// Checks for all blocks to be stationary then check for any colour match sequences
void ARApp::CheckForMatches()
{
	if (waitingForMatches && tower->AllBlocksStationary())
	{
		bool matchFound = true;
		while (matchFound)
		{
			MatchResult matchResult = tower->CheckForMatches();
			matchFound = matchResult.matchFound;
			if (matchFound)
			{
				score += matchResult.sequenceLength;

				// Check for second hint completion
				if (!firstMatchFound && firstBlockDestroyed && turnsUntilSpawn != (TURNS_BETWEEN_SPAWNS - 1))
				{
					firstMatchFound = true;
					hud->NextHint();
				}
			}
		}

		// User has finished their turn and there are no more matches
		CheckLoseCondition();
	}
}

void ARApp::CheckLoseCondition()
{
	if (tower->CurrentHeight() >= MAX_TOWER_HEIGHT)
	{
		// The player loses
		currentGameState = GAMEOVER;
	}
	else
		waitingForMatches = false;
}

void ARApp::SpawnNewBlocks()
{
	// Spawn new blocks onto the tower once matches are found and the player hasn't lost
	if (!waitingForMatches && waitingForSpawns && tower->AllBlocksStationary() && (turnsUntilSpawn == 0))
	{
		// No more matches found, add new blocks
		tower->NewBlocks();
		waitingForSpawns = false;
		blocksSpawning = true;
		turnsUntilSpawn = TURNS_BETWEEN_SPAWNS;
	}
	if (blocksSpawning && tower->AllBlocksStationary())
		blocksSpawning = false;
}

void ARApp::ResetGame()
{
	// Delete current tower and initialise a new default one
	tower->CleanUp();
	delete tower;
	tower = new Tower();
	tower->Init(platform_);

	// Reset flags and counters
	currentGameState = PLAY;
	waitingForMatches = false;
	waitingForSpawns = false;
	score = 0;
	turnsUntilSpawn = TURNS_BETWEEN_SPAWNS;
}

bool ARApp::AreTapsDisabled()
{
	return (currentGameState != GameState::PLAY) || waitingForMatches || waitingForSpawns || !tower->AllBlocksStationary() || !markerVisible;
}

// Selects the first found ARMarker from camera image
void ARApp::GetFirstMarker()
{
	for (int marker = MarkerOne; marker < NumMarkers; marker++)
	{
		if (Dummy::ARMarkerDetected(marker))
		{
			// Marker is found and tracked
			Dummy::Mat4x4 markerTransform;
			markerTransform = Dummy::GetARMarkerTransform(marker);

			// Move the tower to the marker
			tower->SetTowerOrigin(markerTransform);

			markerVisible = true;
			currentMarker = static_cast<ARMarker>(marker);
			break;
		}
		else
		{
			markerVisible = false;
			currentMarker = NumMarkers;
		}
	}
}

void ARApp::DrawCameraFeed()
{
	if (Dummy::IsCameraImageReady())
	{
		camera->UpdateFeed();
		spriteRenderer->DrawSprite(camera->FeedSprite());
	}

	spriteRenderer->End();
}

void ARApp::RenderTower()
{
	// Update view matrix from sample library
	Dummy::Mat4x4 sampleView;
	sampleView = Dummy::GetCameraViewMatrix();
	camera->SetViewMatrix(sampleView);

	// Set matrices
	renderer3d->SetProjectionMatrix(camera->ProjectionMatrix());
	renderer3d->SetViewMatrix(camera->ViewMatrix());

	// Begin rendering tower, don't clear the frame buffer
	renderer3d->Begin();

	if (Dummy::ARMarkerDetected(static_cast<int>(currentMarker)))
	{
		tower->Render(renderer3d);
	}

	renderer3d->End();
}

void ARApp::SetupLights()
{
	Dummy::PointLight defaultPointLight;
	defaultPointLight.SetColour(Dummy::Vec4(0.7f, 0.7f, 1.0f, 1.0f));
	defaultPointLight.SetPosition(Dummy::Vec4(-300.0f, -500.0f, 100.0f));

	Dummy::ShaderData defaultShaderData = renderer3d->DefaultShaderData();
	defaultShaderData.SetAmbientColour(Dummy::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	defaultShaderData.AddPointLight(defaultPointLight);
}