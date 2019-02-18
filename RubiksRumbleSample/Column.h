#ifndef WALL_COLUMN_H
#define WALL_COLUMN_H

#include <list>
#include <algorithm>
#include <vector>
#include <time.h>
#include "GameObject.h"
#include "Block.h"
#include "VitaGefDummies.hpp"

#define DEFAULT_HEIGHT 4
#define NUM_COLUMNS 3
#define BLOCK_SIZE 0.0275F
#define BLOCK_OFFSET 0.03F
#define BLOCK_FALL_SPEED 0.075F
#define BLOCK_SPAWN_HEIGHT 0.35F
#define WALL_ORIGIN_OFFSET 0.05F
#define WALL_CENTRE_OFFSET ((NUM_COLUMNS / 2) * BLOCK_SIZE) + BLOCK_OFFSET
#define NUM_COLOURS 5
#define DEG_TO_RAD 0.01745329252

class Column
{
public:
	Column();
	~Column();

	void Init(int wallIndex, int columnIndex, Dummy::Platform& platform, Dummy::Mesh* cubeMesh);
	void Update(float frameTime);
	void Render(Dummy::Renderer3D* renderer);
	void RemoveBlock(Block* blockToRemove, int wallIndex);
	void CollapseColumn(Block* blockToRemove, int wallIndex);
	void AddBlockToTop(int wallIndex, Dummy::Mesh* cubeMesh);
	Block* GetBlockAtRow(int index);
	bool IsSelectionValid(Block* blockToRemove);
	void SetOrigin(Dummy::Mat4x4 origin);
	void CleanUp();

	std::list<Block*>& Blocks() { return blocks; }

protected:
	Block * NewBlock(Dummy::Vec4 position, uint32_t colourBelow, int wallIndex, Dummy::Mesh* cubeMesh);

	std::list<Block*> blocks;
	std::vector<uint32_t> colours;
	int columnIndex; // Denotes which column this is within the parent Wall. Used for offsetting positions

	// Origins
	std::vector<Dummy::Vec4> wallOrigins;
	Dummy::Mat4x4 columnOrigin;
};

#endif