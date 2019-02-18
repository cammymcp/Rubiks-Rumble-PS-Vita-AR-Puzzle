#include "HUD.h"
#include <iostream>
#include "VitaGefDummies.hpp"

HUD::HUD(Dummy::SpriteRenderer* spRenderer) :
	font(NULL)
{
	spriteRenderer = spRenderer;
}

HUD::~HUD()
{}

void HUD::Draw(Dummy::Platform& platform, GameState currentGameState, int towerHeight, int score, int remainingTurns, bool gameOver, bool markerVisible, bool blocksSpawning)
{
	Dummy::Mat4x4 proj_matrix2d;
	proj_matrix2d = Dummy::GLOrthoFrustum(0.0f, platform.width, 0.0f, platform.height, -1.0f, 1.0f);
	spriteRenderer->SetProjectionMatrix(proj_matrix2d);
	spriteRenderer->Begin();
	DrawHUD(currentGameState, towerHeight, score, remainingTurns, gameOver, markerVisible, blocksSpawning);
	spriteRenderer->End();
}

void HUD::Init(Dummy::Platform& platform, int maxTowerHeight, int minimumMatch)
{
	InitFont(platform);

	// Setup sprites
	menuBGSprite.SetWidth(platform.width);
	menuBGSprite.SetHeight(platform.height);
	menuBGSprite.SetPosition(Dummy::Vec4(platform.width / 2, platform.height / 2, 1.0f));
	Dummy::Texture* loadedTexture = LoadTextureFromPNG(platform, "title.png");
	if (loadedTexture != NULL)
		menuBGSprite.SetTexture(loadedTexture);
	else
	{
		std::cout << "Texture is null" << std::endl;
	}

	screenFade.SetWidth(platform.width);
	screenFade.SetHeight(platform.height);
	screenFade.SetPosition(Dummy::Vec4(platform.width / 2, platform.height / 2, 1.0f));
	screenFade.SetColour(0x915b5b5b);

	// Create hints for user (first hint will be added to vector last)
	UserHint rotateMarkerHint;
	rotateMarkerHint.message = "Spin the marker card to plan your next move!";
	hints.push_back(rotateMarkerHint);

	UserHint loseConditionHint;
	std::ostringstream loseMessageStream;
	loseMessageStream << "Keep the tower below a height of " << maxTowerHeight;
	loseConditionHint.message = loseMessageStream.str();
	hints.push_back(loseConditionHint);

	UserHint createMatchesHint;
	std::ostringstream matchesMessageStream;
	matchesMessageStream << "Create sequences of " << minimumMatch << " matching blocks to destroy them";
	createMatchesHint.message = matchesMessageStream.str();
	hints.push_back(createMatchesHint);

	UserHint popBlocksHint;
	popBlocksHint.message = "Tap on a block to destroy it";
	hints.push_back(popBlocksHint);
	currentHint = hints.back();
}

void HUD::NextHint()
{
	hints.pop_back();
	if (!hints.empty())
	{
		currentHint = hints.back();
	}
}

void HUD::InitFont(Dummy::Platform& platform)
{
	font= new Dummy::Font(platform);
	font->Load("gill_sans_mt");
}

void HUD::DrawHUD(GameState currentGameState, int towerHeight, int score, int remainingTurns, bool gameOver, bool markerVisible, bool blocksSpawning)
{
	if (font)
	{
		if (gameOver)
			DrawGameOverText(score);
		else if (currentGameState == GameState::PLAY)
		{
			if (markerVisible)
			{
				DrawTurnsRemainingText(remainingTurns, blocksSpawning);
				DrawScoreText(score);
				DrawHeightText(towerHeight);
				if (!hints.empty())
					DrawHint();
			}
			else
			{
				spriteRenderer->DrawSprite(screenFade);
				DrawLostMarkerText();
			}
		}
		else if (currentGameState == GameState::MENU)
		{
			DrawMenu();
		}
	}
}

void HUD::CleanUp()
{
	delete font;
	font = NULL;
}

void HUD::DrawMenu()
{
	spriteRenderer->DrawSprite(menuBGSprite);
}


void HUD::DrawScoreText(int score)
{
	font->RenderText("Score: %d");
}

void HUD::DrawTurnsRemainingText(int remainingTurns, bool blocksSpawning)
{
	if (!blocksSpawning)
		font->RenderText("Turns Until Blocks Spawn: %d");
	else
		font->RenderText("New blocks spawning!");
}

void HUD::DrawHeightText(int towerHeight)
{
	uint32_t textColour = 0xffffffff;
	if (towerHeight <= MAX_GREEN_HEIGHT)
		textColour = 0xff4eff57;
	else if (towerHeight > MAX_GREEN_HEIGHT && towerHeight <= MAX_AMBER_HEIGHT)
		textColour = 0xff2cb0e0;
	else if (towerHeight > MAX_AMBER_HEIGHT)
		textColour = 0xff3c27db;
	font->RenderText("Tower Height: %d", towerHeight);
}

void HUD::DrawGameOverText(int score)
{
	font->RenderText("Game Over! Your Score: %d", score);
	font->RenderText("The tower got too high. Press X to restart");
}

void HUD::DrawLostMarkerText()
{
	font->RenderText("Please make sure the marker card is in view!");
}

void HUD::DrawHint()
{
	const char* hintMessage = currentHint.message.c_str();
	font->RenderText(hintMessage);
}

Dummy::Texture* HUD::LoadTextureFromPNG(Dummy::Platform& platform, const char* filename)
{
	// Create a structure to hold the image data
	Dummy::PixelData imageData;

	// Load image
	Dummy::LoadFromPNG(filename, platform, imageData);

	if (imageData.data == NULL)
	{
		std::cout << "Image failed to load." << std::endl;
		return NULL;
	}
	else
	{
		return Dummy::Texture::Create(platform, imageData);
	}
}