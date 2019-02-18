#include "Tower.h"

Tower::Tower()
{
	// Set up translation offset
	towerOffset.SetIdentity();
	towerOffset.SetTranslation(offsetTranslation);
}

Tower::~Tower()
{
}

void Tower::Init(Dummy::Platform& platform)
{
	// Set up tower base
	towerBase = new GameObject(Dummy::Vec4(TOWER_BASE_OFFSET, TOWER_BASE_OFFSET, 0.0f), Dummy::Vec4(TOWER_BASE_SIZE, TOWER_BASE_SIZE, TOWER_BASE_SIZE), Dummy::Vec4(90.0f * DEG_TO_RAD, 0.0f, 0.0f));

	// Load base block mesh that all spawned blocks will use
	blockSceneFile = new Dummy::LoadedModel();
	blockSceneFile->LoadFromFile(platform, "bevelledcubesmall.scn");

	// Load tower base mesh 
	towerBaseSceneFile = new Dummy::LoadedModel();
	towerBaseSceneFile->LoadFromFile(platform, "towermiddle.scn");

	// Create the materials from the material data present in the scene files
	blockSceneFile->LoadMaterials(platform);
	towerBaseSceneFile->LoadMaterials(platform);

	// If there is mesh data in the scene, create a mesh to draw from the first mesh
	baseBlockMesh = GetFirstMesh(platform, blockSceneFile);
	towerBase->SetMesh(GetFirstMesh(platform, towerBaseSceneFile));

	// Set up walls
	for (int i = 0; i < NUM_WALLS; i++)
	{
		Wall* wall = new Wall();
		wall->Init(platform, i, baseBlockMesh);
		walls.push_back(wall);
	}
}

void Tower::CleanUp()
{
	delete baseBlockMesh;
	baseBlockMesh = NULL;

	delete towerBase;
	towerBase = NULL;

	delete blockSceneFile;
	blockSceneFile = NULL;

	delete towerBaseSceneFile;
	towerBaseSceneFile = NULL;

	// Clean up walls
	while (!walls.empty())
	{
		walls.back()->CleanUp();
		delete walls.back();
		walls.pop_back();
	}
}

// Used for updating individual blocks so they can move per frame
void Tower::Update(float frameTime)
{
	for (auto wall : walls)
	{
		wall->Update(frameTime);
	}
}

void Tower::Render(Dummy::Renderer3D* renderer)
{
	// Set override colour for tower
	Dummy::Material towerMaterial;
	towerMaterial.SetColour(towerColour);
	renderer->SetOverrideMaterial(&towerMaterial);

	// Render tower base
	renderer->DrawMesh(*towerBase);

	// Now render tower walls
	for (auto wall : walls)
	{
		wall->Render(renderer);
	}
}

// Checks if all blocks have finished moving to their positions
bool Tower::AllBlocksStationary()
{
	return walls[0]->AllBlocksStationary() && 
		   walls[1]->AllBlocksStationary() && 
		   walls[2]->AllBlocksStationary() && 
		   walls[3]->AllBlocksStationary();
}

// Adds a new layer of blocks to the top of the tower
void Tower::NewBlocks()
{
	for (auto wall : walls)
	{
		wall->AddNewRow(baseBlockMesh);
	}
}

// Checks for a horizontal colour sequence of 3 or more in each wall
MatchResult Tower::CheckForMatches()
{
	for (auto wall : walls)
	{
		MatchResult result = wall->CheckForMatch();
		if (result.matchFound)
			return result;
	}
	return MatchResult();
}
 
// Sets the origin transform of each column and apply constant offset
void Tower::SetTowerOrigin(Dummy::Mat4x4 transform)
{
	Dummy::Mat4x4 origin = Dummy::Mat4x4::Mult(towerOffset, transform);

	// Set base origin
	towerBase->SetTransform(Dummy::Mat4x4::Mult(towerBase->LocalTransform(), origin));

	// Set walls origin
	for (auto wall : walls)
	{
		for (auto column : wall->Columns())
		{
			column->SetOrigin(origin);
		}
	}
}

// Returns the height of the tallest column in the tower
int Tower::CurrentHeight()
{
	int height = 0;
	for (auto wall : walls)
	{
		int wallHeight = wall->MaxColumnHeight();
		if (wallHeight > height)
		{
			height = wallHeight;
		}
	}
	return height;
}

Dummy::Mesh* Tower::GetFirstMesh(Dummy::Platform& platform, Dummy::LoadedModel* scene)
{
	Dummy::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->meshData.size() > 0)
			mesh = blockSceneFile->CreateMesh(platform, scene->meshData.front());
	}

	return mesh;
}