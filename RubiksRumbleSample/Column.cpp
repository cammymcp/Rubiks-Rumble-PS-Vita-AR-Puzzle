#include "Column.h"

Column::Column()
{
	wallOrigins.resize(4);
	wallOrigins[0] = Dummy::Vec4(0.0f, WALL_ORIGIN_OFFSET, 0.0f, 0.0f);
	wallOrigins[1] = Dummy::Vec4(WALL_ORIGIN_OFFSET, 0.0f, 0.0f, 0.0f);
	wallOrigins[2] = Dummy::Vec4(0.0f, -WALL_ORIGIN_OFFSET, 0.0f, 0.0f);
	wallOrigins[3] = Dummy::Vec4(-WALL_ORIGIN_OFFSET, 0.0f, 0.0f, 0.0f);
	columnOrigin.SetIdentity();
}

Column::~Column()
{
}

void Column::Init(int wallIndex, int colIndex, Dummy::Platform& platform, Dummy::Mesh* cubeMesh)
{
	// Init colours
	colours.resize(NUM_COLOURS);
	colours[0] = 0xff0000ff; // Red
	colours[1] = 0xff00ff00; // Green
	colours[2] = 0xffff0000; // Blue
	colours[3] = 0xffffff00; // Yellow
	colours[4] = 0xff00ffff; // Cyan

	// Initialise each block's game object
	columnIndex = colIndex;
	for (int blockIndex = 1; blockIndex < DEFAULT_HEIGHT + 1; blockIndex++)
	{
		// Create empty colour
		uint32_t colourBelow(0x00000000);
		if (blockIndex > 1)
		{
			// Not first block, get colour of the previous block
			colourBelow = GetBlockAtRow(blockIndex - 2)->Colour();
		}
		// Create a new block with a colour that does not match the previous block
		Dummy::Vec4 localPosition = (wallIndex % 2 == 0) ? Dummy::Vec4(columnIndex * BLOCK_OFFSET, WALL_CENTRE_OFFSET, blockIndex * BLOCK_OFFSET) :
															Dummy::Vec4(WALL_CENTRE_OFFSET, columnIndex * BLOCK_OFFSET, blockIndex * BLOCK_OFFSET);
		Dummy::Vec4 wallPosition = wallOrigins[wallIndex];
		blocks.push_back(NewBlock(Dummy::AddVectors(wallPosition, localPosition), colourBelow, wallIndex, cubeMesh));
	}
}

void Column::Update(float frameTime)
{
	// Call each block's update function
	for (auto block : blocks)
	{
		if (block->IsActive())
		{
			block->Update(frameTime);
		}
	}
}

// Renders each block if it is active
void Column::Render(Dummy::Renderer3D* renderer)
{
	Dummy::Material overrideMat;
	for (auto block : blocks)
	{
		if (block->IsActive())
		{
			// Set renderer override material to the block's colour
			overrideMat.SetColour(block->Colour());
			renderer->SetOverrideMaterial(&overrideMat);
			renderer->DrawMesh(*block);
		}
	}
}

// Frees memory of block and removes from list container, also calls CollapseColumn
void Column::RemoveBlock(Block* blockToRemove, int wallIndex)
{
	// Add velocity to blocks above
	CollapseColumn(blockToRemove, wallIndex);

	// Now delete the block
	delete blockToRemove;
	blocks.remove(blockToRemove);
}

// Tells all blocks above blockToRemove to move to a lower position
void Column::CollapseColumn(Block* blockToRemove, int wallIndex)
{
	// Get iterator of block to remove so we can collapse the blocks above it
	auto itor = blocks.end(); int blockIndex = 1;
	for (auto i = blocks.begin(); i != blocks.end(); ++i)
	{
		if (*i == blockToRemove)
		{
			itor = i;
			break;
		}
		blockIndex++;
	}
	if (itor != blocks.end())
	{
		// Iterate over higher blocks and set new position based on index
		itor++;
		for (auto block = itor; block != blocks.end(); block++)
		{
			Dummy::Vec4 localPosition = (wallIndex % 2 == 0) ? Dummy::Vec4(columnIndex * BLOCK_OFFSET, WALL_CENTRE_OFFSET, blockIndex * BLOCK_OFFSET) :
																Dummy::Vec4(WALL_CENTRE_OFFSET, columnIndex * BLOCK_OFFSET, blockIndex * BLOCK_OFFSET);
			Dummy::Vec4 wallPosition = wallOrigins[wallIndex];
			(*block)->MoveTo(Dummy::AddVectors(wallPosition, localPosition), BLOCK_FALL_SPEED);
			blockIndex++;
		}
	}
}

void Column::AddBlockToTop(int wallIndex, Dummy::Mesh* cubeMesh)
{
	// Get colour of current top block (if there is one)
	uint32_t colour = 0x00000000;
	if (blocks.size() > 0)
	{
		std::list<Block*>::const_iterator itor = blocks.end();
		itor--;
		Block* topBlock = *itor;
		colour = topBlock->Colour();
	}

	// Create a new block with a colour that does not match the previous block
	Dummy::Vec4 localPosition = (wallIndex % 2 == 0) ? Dummy::Vec4(columnIndex * BLOCK_OFFSET, WALL_CENTRE_OFFSET, BLOCK_SPAWN_HEIGHT) :
														Dummy::Vec4(WALL_CENTRE_OFFSET, columnIndex * BLOCK_OFFSET, BLOCK_SPAWN_HEIGHT);
	Dummy::Vec4 wallPosition = wallOrigins[wallIndex];
	Block* newBlock = NewBlock(Dummy::AddVectors(wallPosition, localPosition), colour, wallIndex, cubeMesh);

	// Add to list
	blocks.push_back(newBlock);

	// Move block into position
	localPosition = (wallIndex % 2 == 0) ? Dummy::Vec4(columnIndex * BLOCK_OFFSET, WALL_CENTRE_OFFSET, blocks.size() * BLOCK_OFFSET) :
										   Dummy::Vec4(WALL_CENTRE_OFFSET, columnIndex * BLOCK_OFFSET, blocks.size() * BLOCK_OFFSET);
	newBlock->MoveTo(Dummy::AddVectors(wallPosition, localPosition), BLOCK_FALL_SPEED);
}

// Sets local origin and sets world transform of each block
void Column::SetOrigin(Dummy::Mat4x4 origin)
{
	columnOrigin = origin;
	for (auto block : blocks)
	{
		block->SetTransform(Dummy::Mat4x4::Mult(block->LocalTransform(), origin));
	}
}

// Frees dynamic memory allocations
void Column::CleanUp()
{
	// Delete Blocks
	while (!blocks.empty())
	{
		delete blocks.front();
		blocks.pop_front();
	}
}

Block* Column::NewBlock(Dummy::Vec4 position, uint32_t colourBelow, int wallIndex, Dummy::Mesh* cubeMesh)
{	
	std::vector<uint32_t> availableColours; // colours minus colourBelow	
	for (int i = 0; i < NUM_COLOURS; i++)
	{
		// Add colour to available colours if it doesn't match the block below
		if (colours[i] != colourBelow)
		{
			availableColours.push_back(colours[i]);
		}
	}	

	// Select a colour from available colours and create the block
	uint32_t randomColour = availableColours[rand() % (NUM_COLOURS - 1)];
	Block* newBlock = new Block(Dummy::Vec4(), Dummy::Vec4(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE), Dummy::Vec4(0.0f, 0.0f, 0.0f));
	newBlock->SetMesh(cubeMesh);
	newBlock->SetColour(randomColour);

	// Rotate block based on wall index, then set to local column position
	newBlock->SetPosition(position);
	float cubeRotation = (wallIndex % 2) != 0 ? 0.0f : 90.0f * DEG_TO_RAD;
	newBlock->SetRotationEuler(0.0f, 0.0f, cubeRotation);

	// Now set global transform using column origin
	newBlock->SetTransform(Dummy::Mat4x4::Mult(newBlock->LocalTransform(), columnOrigin));
	return newBlock;
}

bool Column::IsSelectionValid(Block* blockToRemove)
{
	// You cannot destroy a block when it is the only one in the column
	bool singleBlock = (blocks.size() == 1);

	// You cannot destroy the top block of a column
	auto itor = blocks.end();
	itor--;
	bool topBlock = (*itor == blockToRemove);

	return !topBlock && !singleBlock;
}

// Result should be null checked!
Block* Column::GetBlockAtRow(int index)
{
	if (index < 0 || index >= blocks.size())
		return nullptr;

	std::list<Block*>::const_iterator itor = blocks.begin();
	std::advance(itor, index);
	return *itor;
}