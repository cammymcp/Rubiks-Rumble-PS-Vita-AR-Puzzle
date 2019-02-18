#include "Wall.h"

Wall::Wall()
{
}

Wall::~Wall()
{
}

void Wall::Init(Dummy::Platform& platform, int wallIndex, Dummy::Mesh* cubeMesh)
{
	// Set up columns
	mWallIndex = wallIndex;
	for (int columnIndex = 0; columnIndex < NUM_COLUMNS; columnIndex++)
	{
		Column* column = new Column();
		column->Init(wallIndex, columnIndex + 1, platform, cubeMesh);
		columns.push_back(column);
	}
}

void Wall::Render(Dummy::Renderer3D* renderer)
{
	// Render each column
	for (auto column : columns)
	{
		column->Render(renderer);
	}
}

void Wall::Update(float frameTime)
{
	// Update each column
	for (auto column : columns)
	{
		column->Update(frameTime);
	}
}

void Wall::CleanUp()
{
	// Clean up each column
	while (!columns.empty())
	{
		columns.back()->CleanUp();
		delete columns.back();
		columns.pop_back();
	}
}

// Checks for a horizontal sequence of matching colours anywhere on the wall, and destroys them if found
MatchResult Wall::CheckForMatch()
{
	MatchResult result = MatchResult();
	for (int row = 0; row < MaxColumnHeight(); row++)
	{
		uint32_t matchColour = 0x000000ff;
		std::vector<BlockReference> matchingBlocks;
		for (int column = 0; column < NUM_COLUMNS; column++)
		{
			Block* currentBlock = columns[column]->GetBlockAtRow(row);
			if (currentBlock == nullptr)
			{
				// If we have an empty cell, check sequence and clear list
				if (matchingBlocks.size() >= MIN_MATCH)
				{
					// Found a sequence of 3 or more
					RemoveMatchedBlocks(matchingBlocks);
					result.matchFound = true;
					result.sequenceLength = matchingBlocks.size();
					return result;
				}
				matchColour = 0x000000ff;
				matchingBlocks.clear();
				continue;
			}
			
			uint32_t currentColour = currentBlock->Colour();
			if (currentColour == matchColour)
			{
				matchingBlocks.push_back(BlockReference(currentBlock, column));
			}
			if (currentColour != matchColour || column == NUM_COLUMNS - 1)
			{
				// Check if sequence is at the required length when we find a non-matching colour or when we're at the end of the row
				if (matchingBlocks.size() >= MIN_MATCH)
				{
					// Found a sequence of 3 or more
					RemoveMatchedBlocks(matchingBlocks);
					result.matchFound = true;
					result.sequenceLength = matchingBlocks.size();
					return result;
				}
				matchColour = currentColour;
				matchingBlocks.clear();
				matchingBlocks.push_back(BlockReference(currentBlock, column));
			}
		}
		matchingBlocks.clear(); // Clear matches after each row
	}
	return result;
}

// Adds a new block to the top of each column
void Wall::AddNewRow(Dummy::Mesh* cubeMesh)
{
	for (auto column : columns)
	{
		column->AddBlockToTop(mWallIndex, cubeMesh);
	}
}

// Calls the parent column's remove function for each block in the match sequence
void Wall::RemoveMatchedBlocks(std::vector<BlockReference> matchingBlocks)
{
	for (auto matchedBlock : matchingBlocks)
	{
		for (int column = 0; column < NUM_COLUMNS; column++)
		{
			if (matchedBlock.columnIndex == column)
			{
				columns[column]->RemoveBlock(matchedBlock.block, mWallIndex);
			}
		}
	}
}

bool Wall::AllBlocksStationary()
{
	for (auto column : columns)
	{
		for (auto block : column->Blocks())
		{
			if (block->Velocity().LengthSqr() != 0.0f)
			{
				return false;
			}
		}
	}
	return true;
}

// Returns the height of the tallest column
int Wall::MaxColumnHeight()
{
	int maxHeight = 0;
	for (auto column : columns)
	{
		if (column->Blocks().size() > maxHeight)
		{
			maxHeight = column->Blocks().size();
		}
	}
	return maxHeight;
}