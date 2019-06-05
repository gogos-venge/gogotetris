#pragma once
#include "Block.h"
#include <functional>
#define CBLENGTH 10
#define GAME_HEIGHT 40
#define GAME_WIDTH 10
#define BAG_SIZE 14
#define BLOCK_SET_SIZE 7

typedef std::function<void()> Callback;

class Game
{
public:
	Game(int Level);
	~Game();
	
	//Metric properties
	int		Level = 0;
	int		Score = 0;
	int		TotalLines = 0;
	int		SpeedFps = 60;

	//Board
	int		Playfield[GAME_WIDTH][GAME_HEIGHT] = {};

	//Blocks
	Block*	BlockBag[BAG_SIZE];
	Block*	ProduceBlock(int index);
	Block*	GetCurrentBlock();
	Block*	GetNextBlock();
	Block*	GetGhostBlock();
	Block*	GetHeldBlock();
	Block*	PopBlock();
	void	EngageBlock(bool useNext);
	void	ImpressCurrentBlock();
	void	HoldCurrentBlock();
	void	FillBlockBag();

	//Move methods
	bool	MoveCurrentBlockX(int offset_x);
	bool	MoveCurrentBlockY(int offset_y);
	void	RotateCurrentBlockCW();
	void	RotateCurrentBlockCCW();
	void	HardDropCurrentBlock();

	//Event Methods
	void	OnCollisionY(Callback);
	void	OnCollisionX(Callback);
	void	OnCheckLine(Callback);
	void	OnClearLine(Callback);
	void	OnRotateBlock(Callback);
	void	OnLevelIncrease(Callback);
	void	OnTetrisCheck(Callback);
	void	OnTetrisClear(Callback);
	void	OnUpdateScore(Callback);
	void	AddCallback(Callback* pool, Callback cb, int poolLength);
	void	TriggerCallbacks(Callback* pool, int poolLength);

	//Line Stuff
	int		CheckLines(int* lines);
	int		ClearLines(int* lines);
	int		Lines[4];

	//Misc
	bool	DetectCollision(int x, int y, Block * b);
	int		UpdateMetrics(int LineNumber);
	int		FrameSpeed(int Level);
	int		RandomBlockIndex();

	enum State {
		PAUSED,
		EXITBYUSER,
		PLAYING,
		GAMEOVER
	};

	State GameStatus = PLAYING;

private:
	//Block Stuff
	Block*	CurrentBlock;
	Block*	GhostBlock;
	Block*	HeldBlock;
	bool	CanHold;
	bool	MoveGhostBlockY();
	void	GreedyMoveGhostBlockY();
	void	PredictGhostBlock();
	int		BlockBagCount = 0;

	//Event callbacks (non-thread)
	Callback CollisionYCallbacks[CBLENGTH] = {};
	Callback CollisionXCallbacks[CBLENGTH] = {};
	Callback CheckLineCallbacks[CBLENGTH] = {};
	Callback ClearLineCallbacks[CBLENGTH] = {};
	Callback RotateBlockCallbacks[CBLENGTH] = {};
	Callback LevelIncreaseCallbacks[CBLENGTH] = {};
	Callback TetrisCheckCallbacks[CBLENGTH] = {};
	Callback TetrisClearCallbacks[CBLENGTH] = {};
	Callback UpdateScoreCallbacks[CBLENGTH] = {};
};