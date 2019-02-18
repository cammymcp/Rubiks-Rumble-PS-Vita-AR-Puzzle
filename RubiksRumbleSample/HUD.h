#ifndef HUD_H
#define HUD_H

#include "VitaGefDummies.hpp"
#include "GameState.h"
#include <vector>
#include <sstream>
#include <string>

#define MAX_GREEN_HEIGHT 6
#define MAX_AMBER_HEIGHT 9

struct UserHint
{
	std::string message = "";
};

class HUD 
{
public: 
	HUD(Dummy::SpriteRenderer* spriteRenderer);
	~HUD();

	void Draw(Dummy::Platform& platform, GameState currentGameState, int towerHeight = 0, int score = 0, int remainingTurns = 0, bool gameOver = false, bool markerVisible = false, bool blocksSpawning = false);
	void Init(Dummy::Platform& platform, int maxTowerHeight, int minimumMatch);
	void CleanUp();
	void NextHint();

protected:
	void InitFont(Dummy::Platform& platform);
	void DrawHUD(GameState currentGameState, int towerHeight, int score, int remainingTurns, bool gameOver, bool markerVisible, bool blocksSpawning);
	void DrawScoreText(int score);
	void DrawTurnsRemainingText(int remainingTurns, bool blocksSpawning);
	void DrawHeightText(int towerHeight);
	void DrawGameOverText(int score);
	void DrawLostMarkerText();
	void DrawHint();
	void DrawMenu();
	Dummy::Texture* LoadTextureFromPNG(Dummy::Platform& platform, const char* filename);

	Dummy::Font* font;
	Dummy::SpriteRenderer* spriteRenderer; // Local handle to ARApp sprite_renderer_

	// Images and textures
	Dummy::Sprite menuBGSprite;
	Dummy::Sprite screenFade;

	// Hints for the user to complete
	std::vector<UserHint> hints;
	UserHint currentHint;
};

#endif