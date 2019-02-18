#ifndef TOWER_H
#define TOWER_H

#include "Wall.h"
#include "VitaGefDummies.hpp"

#define NUM_WALLS 4
#define TOWER_COLOUR 0xff5b5b5b;
#define TOWER_BASE_OFFSET 0.0575F
#define TOWER_BASE_SIZE 0.05F

class Tower
{
public:
	Tower();
	~Tower();

	void Init(Dummy::Platform& platform);
	void CleanUp();
	void Update(float frameTime);
	void Render(Dummy::Renderer3D* renderer);
	bool AllBlocksStationary();
	void NewBlocks();
	void SetTowerOrigin(Dummy::Mat4x4 transform);
	MatchResult CheckForMatches();
	std::vector<Wall*>& Walls() { return walls; } 
	int CurrentHeight();

protected:
	Dummy::Mesh* GetFirstMesh(Dummy::Platform& platform, Dummy::LoadedModel* scene);

	// Handles and Containers
	std::vector<Wall*> walls;

	// Transforms
	Dummy::Mat4x4 towerOffset;
	const Dummy::Vec4 offsetTranslation = Dummy::Vec4(-0.05f, -0.08f, 0.0f, 0.0f);

	// 3D Objects
	Dummy::Mesh* baseBlockMesh;
	Dummy::LoadedModel* blockSceneFile;
	GameObject* towerBase;
	Dummy::LoadedModel* towerBaseSceneFile;
	const uint32_t towerColour = TOWER_COLOUR;
};

#endif