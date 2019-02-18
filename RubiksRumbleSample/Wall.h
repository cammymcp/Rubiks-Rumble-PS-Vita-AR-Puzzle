#ifndef WALL_H
#define WALL_H

#include "Column.h"

#define MIN_MATCH 3

struct BlockReference
{
	BlockReference(Block* blockPtr, int colIndex)
	{
		block = blockPtr;
		columnIndex = colIndex;
	}

	Block* block;
	int columnIndex;
};

struct MatchResult
{
	MatchResult()
	{
		matchFound = false;
		sequenceLength = 0;
	}

	bool matchFound;
	int sequenceLength;
};

class Wall
{
public:
	Wall();
	~Wall();

	void Init(Dummy::Platform& platform, int wallIndex, Dummy::Mesh* cubeMesh);
	void Render(Dummy::Renderer3D* renderer);
	void Update(float frameTime);
	void CleanUp();

	MatchResult CheckForMatch();
	void AddNewRow(Dummy::Mesh* cubeMesh);
	void RemoveMatchedBlocks(std::vector<BlockReference> matchingBlocks);
	bool AllBlocksStationary();
	int MaxColumnHeight();

	std::vector<Column*>& Columns() { return columns; }

protected:
	std::vector<Column*> columns;
	int mWallIndex; // This value denotes which wall this is within the tower. Used for offsetting new block positions
};

#endif