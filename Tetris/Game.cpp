#include "stdafx.h"
#include "Game.h"

Game::Game(int Level)
{
	//Set Level
	this->Level = Level;

	//init update metrics
	UpdateMetrics(0);

	//init random
	srand((Uint32)time(0));

	//engage the first block here
	FillBlockBag();
	EngageBlock(true);

	CanHold = true;
}

Game::~Game()
{
	delete CurrentBlock;
	delete GhostBlock;
	delete HeldBlock;

	for (int i = 0; i < BAG_SIZE; i++) {
		delete BlockBag[i];
	}
}

/*Returns the current block*/
Block* Game::GetCurrentBlock() {
	return CurrentBlock;
}

/*Returns the next block*/
Block* Game::GetNextBlock() {
	return BlockBag[0];
}

/*Returns the Ghost block*/
Block* Game::GetGhostBlock() {
	return GhostBlock;
}

/*Returns the Held block*/
Block* Game::GetHeldBlock() {
	return HeldBlock;
}

Block* Game::PopBlock(){
	Block* b = BlockBag[0];
	for (int i = 1; i < BAG_SIZE; i++) {
		BlockBag[i - 1] = BlockBag[i];
	}
	BlockBag[BAG_SIZE - 1] = NULL;
	BlockBagCount--;
	return b;
}

/*Uses the Block factory to produce a block. Numbers 0 for Line to 6 for Z*/
Block* Game::ProduceBlock(int index) {
	return Block::Factory(index);
}

/*Generates a number from 1 to 7. Best used to pick a Block*/
int Game::RandomBlockIndex() {
	return rand() % BLOCK_SET_SIZE;
}

/*Next block becomes the current block, Next block uses factory to get a new one, and current block is positioned to the top most center of the playfield
Also if the engaged block collides, it means you lost*/
void Game::EngageBlock(bool useNext) {
	if (useNext) {
		delete CurrentBlock;
		CurrentBlock = PopBlock();
		if (BlockBagCount <= BLOCK_SET_SIZE) {
			FillBlockBag();
		}
	}
	
	CurrentBlock->x = 4;
	CurrentBlock->y = 20;

	delete GhostBlock;
	GhostBlock = ProduceBlock(CurrentBlock->type);
	PredictGhostBlock();

	if (DetectCollision(0, 0, CurrentBlock)) {
		GameStatus = GAMEOVER;
	}
}

/*Impresses the current moving block to the playfield. Best used after a Y collision*/
void Game::ImpressCurrentBlock() {
	for (int i = 0; i < CurrentBlock->GetWidth(); i++) {
		for (int j = 0; j < CurrentBlock->GetHeight(); j++) {
			if (CurrentBlock->GetBrick(i, j)) {
				Playfield[CurrentBlock->x + i][CurrentBlock->y + j] = CurrentBlock->GetBrick(i, j);
			}
		}
	}
}

/*Hold (or swap) current block to/with held block*/
void Game::HoldCurrentBlock() {
	if (!CanHold) return;
	CanHold = false;

	//If hold block is NULL we don't have to swap.
	if (HeldBlock == NULL) {
		HeldBlock = ProduceBlock(CurrentBlock->type);
		EngageBlock(true);
		return;
	}

	//Hold the type
	int type = CurrentBlock->type;
	delete CurrentBlock;
	CurrentBlock = ProduceBlock(HeldBlock->type);

	//Produce the same type (notice how we don't hold the state of the block but produce a new one instead)
	delete HeldBlock;
	HeldBlock = ProduceBlock(type);
	EngageBlock(false);
}

/*Shuffles a set of 7 blocks for 20 times and puts them in the Block Bag*/
void Game::FillBlockBag(){
	int pick[7] = { 0,1,2,3,4,5,6 };

	for (int i = 0; i < 20; i++) {
		int swap1 = RandomBlockIndex();
		int swap2 = RandomBlockIndex();
		int tmp = pick[swap1];
		pick[swap1] = pick[swap2];
		pick[swap2] = tmp;
	}

	for (int i = 0, j = 0; i < BAG_SIZE && j < BLOCK_SET_SIZE; i++) {
		if (BlockBag[i] == NULL) {
			BlockBag[i] = ProduceBlock(pick[j++]);
			BlockBagCount++;
		}
	}
}

/*Moves the current block to the X axis, and also sends the Ghost Block to bottom. Returns true if it moved*/
bool Game::MoveCurrentBlockX(int offset_x) {
	if (DetectCollision(offset_x, 0, CurrentBlock)) {
		TriggerCallbacks(CollisionXCallbacks, CBLENGTH);
		return false;
	}
	else {
		CurrentBlock->x += offset_x;
		PredictGhostBlock();
		return true;
	}
}

/*Moves the current block to the Y axis. An event is also triggered after the block collides. Returns true if it moved*/
bool Game::MoveCurrentBlockY(int offset_y) {
	if (DetectCollision(0, offset_y, CurrentBlock)) {
		TriggerCallbacks(CollisionYCallbacks, CBLENGTH);
		//We now allow the game to hold blocks
		CanHold = true;
		return false;
	}
	else {
		CurrentBlock->y += offset_y;
		return true;
	}
}

/*Rotates the current block clockwise. Will not rotate if collision happens*/
void Game::RotateCurrentBlockCW() {
	CurrentBlock->RotateCW();
	if (DetectCollision(0, 0, CurrentBlock)) {
		//Undo Rotation. Implement Wall Kick for SRS
		CurrentBlock->RotateCCW();
	}
	else {
		TriggerCallbacks(RotateBlockCallbacks, CBLENGTH);
		GhostBlock->RotateCW();
		PredictGhostBlock();
	}
}

/*Rotates the current block counter-clockwise. Will not rotate if collision happens*/
void Game::RotateCurrentBlockCCW() {
	CurrentBlock->RotateCCW();
	if (DetectCollision(0, 0, CurrentBlock)) {
		//Undo Rotation. Implement Wall Kick for SRS
		CurrentBlock->RotateCW();
	}
	else  {
		TriggerCallbacks(RotateBlockCallbacks, CBLENGTH);
		GhostBlock->RotateCCW();
		PredictGhostBlock();
	}
}

/*Moves the current block down to the furthest possible position*/
void Game::HardDropCurrentBlock() {
	while (MoveCurrentBlockY(1)) {
		Score += 2;
	}
}

/*Returns true if a block collides on a static brick of the playfield, or the borders of the tetris area*/
bool Game::DetectCollision(int offset_x, int offset_y, Block * b) {
	for (int i = 0; i < b->GetWidth(); i++) {
		for (int j = 0; j < b->GetHeight(); j++) {
			//check block
			int x = b->x + i + offset_x; //future position to check
			int y = b->y + j + offset_y;
			if (b->GetBrick(i, j)) {
				if (Playfield[x][y] != 0 || y == GAME_HEIGHT || x == GAME_WIDTH || x < 0) {
					//collision
					return true;
				}
			}
		}
	}
	return false;
}

/*Checks which lines are going to be removed. Returns true if found complete lines. This also triggers an event*/
int Game::CheckLines(int* Lines) {
	for (int i = 0; i < 4; i++) {
		Lines[i] = 0;
	}

	int c = 0;
	for (int i = 0; i < GAME_HEIGHT; i++) {
		bool full = true;
		for (int j = 0; j < GAME_WIDTH; j++) {
			if (Playfield[j][i] == 0) {
				full = false;
				break;
			}
		}
		if (full) {
			Lines[c++] = i;
		}
	}
	if (c == 4) {
		TriggerCallbacks(TetrisCheckCallbacks, CBLENGTH);
	}
	else if (c < 4 && c > 0) {
		TriggerCallbacks(CheckLineCallbacks, CBLENGTH);
	}
	return c;
}

/*Clears the previously checked lines. This triggers an event*/
int Game::ClearLines(int* lines) {
	int CurrentLines = 0;
	for (int i = 0; i < 4; i++) {
		if (lines[i] > 0) {
			for (int j = 0; j < GAME_WIDTH; j++) {
				//Clears the line (fills with zeroes)
				Playfield[j][lines[i]] = 0;
			}
			for (int j = lines[i]; j >= 0; j--) { //height
				for (int k = 0; k < GAME_WIDTH; k++) {
					if (j > 0) {
						//drops all the bricks from above
						Playfield[k][j] = Playfield[k][j - 1];
					}
					else {
						Playfield[k][j] = 0;
					}
				}
			}
			CurrentLines++;
		}
	}
	if (CurrentLines > 0) {
		TriggerCallbacks(ClearLineCallbacks, CBLENGTH);
		if (CurrentLines == 4) {
			TriggerCallbacks(TetrisClearCallbacks, CBLENGTH);
		}
	}
	return CurrentLines;
}

/*Updates metrics respecting the official Tetris scoring system. Calls event*/
int Game::UpdateMetrics(int LineNumber) {
	int Multiplier = 0;
	switch (LineNumber) {
	case 0:
		Multiplier = 0;
		break;
	case 1:
		Multiplier = 40;
		break;
	case 2:
		Multiplier = 100;
		break;
	case 3:
		Multiplier = 300;
		break;
	case 4:
		Multiplier = 1200;
		break;
	}

	int CurLineLvl = TotalLines / 10;

	TotalLines += LineNumber;
	int AddedScore = Multiplier * (Level + 1);
	Score += AddedScore;
	if (TotalLines / 10 > CurLineLvl) {
		Level++;
		TriggerCallbacks(LevelIncreaseCallbacks, CBLENGTH);
	}
	SpeedFps = FrameSpeed(Level);

	TriggerCallbacks(UpdateScoreCallbacks, CBLENGTH);

	return AddedScore;
}

/*Returns the speed in frames of each level based on the official Tetris guidelines*/
int Game::FrameSpeed(int Level) {
	int spd = (int)(60 * pow(0.8f - ((Level - 1)*0.007f), Level));
	return spd == 0 ? 1 : spd;
}

void Game::OnCollisionY(Callback cb) {
	AddCallback(CollisionYCallbacks, cb, CBLENGTH);
}

void Game::OnCollisionX(Callback cb) {
	AddCallback(CollisionXCallbacks, cb, CBLENGTH);
}

void Game::OnCheckLine(Callback cb) {
	AddCallback(CheckLineCallbacks, cb, CBLENGTH);
}

void Game::OnClearLine(Callback cb) {
	AddCallback(ClearLineCallbacks, cb, CBLENGTH);
}

void Game::OnRotateBlock(Callback cb) {
	AddCallback(RotateBlockCallbacks, cb, CBLENGTH);
}

void Game::OnLevelIncrease(Callback cb) {
	AddCallback(LevelIncreaseCallbacks, cb, CBLENGTH);
}

void Game::OnTetrisCheck(Callback cb) {
	AddCallback(TetrisCheckCallbacks, cb, CBLENGTH);
}

void Game::OnTetrisClear(Callback cb) {
	AddCallback(TetrisClearCallbacks, cb, CBLENGTH);
}

void Game::OnUpdateScore(Callback cb) {
	AddCallback(UpdateScoreCallbacks, cb, CBLENGTH);
}

/*Adds a callback to a specific callback pool*/
void Game::AddCallback(Callback* pool, Callback cb, int poolLength) {
	for (int i = 0; i < poolLength; i++) {
		if (pool[i] == NULL) {
			pool[i] = cb;
			break;
		}
	}
}

/*Calls all the callbacks of a callback pool*/
void Game::TriggerCallbacks(Callback* pool, int poolLength) {
	for (int i = 0; i < poolLength; i++) {
		if (pool[i] != NULL) {
			pool[i]();
		}
	}
}

/*Moves the Ghost Block 1 position down. Returns true if moved*/
bool Game::MoveGhostBlockY() {
	if (!DetectCollision(0, 1, GhostBlock)) {
		GhostBlock->y++;
		return true;
	}
	return false;
}

/*Moves the Ghost Block down to the furthest position possible*/
void Game::GreedyMoveGhostBlockY() {
	while (MoveGhostBlockY());
}

/*Predicts the landing position of the ghost block*/
void Game::PredictGhostBlock() {
	GhostBlock->x = CurrentBlock->x;
	GhostBlock->y = CurrentBlock->y;
	GreedyMoveGhostBlockY();
}