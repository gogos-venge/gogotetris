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
	NextBlock = ProduceBlock(RandomBlockIndex());
	EngageBlock();

}

Game::~Game()
{
	delete CurrentBlock;
	delete NextBlock;
}

/*Returns the current block*/
Block* Game::GetCurrentBlock() {
	return CurrentBlock;
}

/*Returns the next block*/
Block* Game::GetNextBlock() {
	return NextBlock;
}

/*Uses the Block factory to produce a block. Numbers 0 for Line to 6 for Z*/
Block* Game::ProduceBlock(int index) {
	return Block::Factory(index);
}

/*Generates a number from 1 to 7. Best used to pick a Block*/
int Game::RandomBlockIndex() {
	return (int)(7.0 * rand() / (RAND_MAX + 1.0));
}

/*Next block becomes the current block, Next block uses factory to get a new one, and current block is positioned to the top most center of the playfield
Also if the engaged block collides, it means you lost*/
void Game::EngageBlock() {
	delete CurrentBlock;
	CurrentBlock = NextBlock;

	int index = RandomBlockIndex();
	NextBlock = ProduceBlock(index);
	CurrentBlock->x = 4;
	CurrentBlock->y = 20;

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

/*Takes the the current block to the unseen topmost of the playfield*/
void Game::ResetCurrentBlock() {
	CurrentBlock->x = 0;
	CurrentBlock->y = 0;
}

/*Moves the current block to the X axis*/
void Game::MoveCurrentBlockX(int offset_x) {
	if (!DetectCollision(offset_x, 0, CurrentBlock)) {
		CurrentBlock->x += offset_x;
	}
	else {
		if (this->CollisionXCallback != NULL) {
			this->CollisionXCallback();
		}
	}
}

/*Moves the current block to the Y axis. If a collision is detected, the block is locked down, clears any new lines, updates metrics and engages a new block.
Better not use negative values here. An event is also triggered after the block collides*/
void Game::MoveCurrentBlockY(int offset_y) {
	if (DetectCollision(0, offset_y, CurrentBlock)) {
		if (this->CollisionYCallback != NULL) {
			this->CollisionYCallback();
		}

		ImpressCurrentBlock();
		UpdateMetrics(
			ClearLines(
				CheckLines()
			)
		);
		ResetCurrentBlock();
		EngageBlock();
	}
	else {
		CurrentBlock->y += offset_y;
	}
}

/*Rotates the current block clockwise. Will not rotate if collision happens*/
void Game::RotateCurrentBlockCW() {
	CurrentBlock->RotateCW();
	if (DetectCollision(0, 0, CurrentBlock)) {
		//Undo Rotation. Implement Wall Kick for SRS
		CurrentBlock->RotateCCW();
	}
	else if (RotateBlockCallback != NULL) {
		RotateBlockCallback();
	}
}

/*Rotates the current block counter-clockwise. Will not rotate if collision happens*/
void Game::RotateCurrentBlockCCW() {
	CurrentBlock->RotateCCW();
	if (DetectCollision(0, 0, CurrentBlock)) {
		//Undo Rotation. Implement Wall Kick for SRS
		CurrentBlock->RotateCW();
	}
	else if (RotateBlockCallback != NULL) {
		RotateBlockCallback();
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
				if (Playfield[x][y] != 0 || y == HEIGHT || x == WIDTH || x < 0) {
					//collision
					return true;
				}
			}
		}
	}
	return false;
}

/*Checks which lines are going to be removed. This also triggers an event*/
int* Game::CheckLines() {
	for (int i = 0; i < 4; i++) {
		Lines[i] = 0;
	}

	int c = 0;
	for (int i = 0; i < HEIGHT; i++) {
		bool full = true;
		for (int j = 0; j < WIDTH; j++) {
			if (Playfield[j][i] == 0) {
				full = false;
				break;
			}
		}
		if (full) {
			Lines[c++] = i;
		}
	}
	if (c > 0) {
		if (CheckLineCallback != NULL) {
			CheckLineCallback();
		}
	}
	return Lines;
}

/*Clears the previously checked lines. This triggers an event*/
int Game::ClearLines(int* lines) {
	int CurrentLines = 0;
	for (int i = 0; i < 4; i++) {
		if (lines[i] > 0) {
			for (int j = 0; j < WIDTH; j++) {
				Playfield[j][lines[i]] = 0;
			}
			for (int j = lines[i]; j >= 0; j--) { //height
				for (int k = 0; k < WIDTH; k++) {
					if (j > 0) {
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
		if (CurrentLines == 4) {
			if (TetrisCallback != NULL) {
				TetrisCallback();
			}
		}
		else if (ClearLineCallback != NULL) {
			ClearLineCallback();
		}
	}
	return CurrentLines;
}

/*Updates metrics respecting the official Tetris scoring system. Calls event*/
void Game::UpdateMetrics(int LineNumber) {
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
	Score += Multiplier * (Level + 1);
	if (TotalLines / 10 > CurLineLvl) {
		Level++;
		if (LevelIncreaseCallback != NULL) {
			LevelIncreaseCallback();
		}
	}
	SpeedFps = FrameSpeed(Level);

	if (UpdateScoreCallback != NULL) {
		UpdateScoreCallback();
	}
}

/*Returns the speed in frames of each level based on the official Tetris guidelines*/
int Game::FrameSpeed(int Level) {
	int spd = (int)(60 * pow(0.8f - ((Level - 1)*0.007f), Level));
	return spd == 0 ? 1 : spd;
}

void Game::OnCollisionY(Callback cb) {
	CollisionYCallback = cb;
}

void Game::OnCollisionX(Callback cb) {
	CollisionXCallback = cb;
}

void Game::OnCheckLine(Callback cb) {
	CheckLineCallback = cb;
}

void Game::OnClearLine(Callback cb) {
	ClearLineCallback = cb;
}

void Game::OnRotateBlock(Callback cb) {
	RotateBlockCallback = cb;
}

void Game::OnLevelIncrease(Callback cb) {
	LevelIncreaseCallback = cb;
}

void Game::OnTetris(Callback cb) {
	TetrisCallback = cb;
}

void Game::OnUpdateScore(Callback cb) {
	UpdateScoreCallback = cb;
}