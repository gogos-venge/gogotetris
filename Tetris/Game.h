#pragma once
#include "Block.h"
#include <functional>

typedef std::function<void()> Callback;

class Game
{
public:
	Game(int Level);
	~Game();

	static const int HEIGHT = 40;
	static const int WIDTH = 10;
	
	//Metric properties
	int		Level = 0;
	int		Score = 0;
	int		TotalLines = 0;
	int		SpeedFps = 60;

	//Board
	int		Playfield[WIDTH][HEIGHT] = {};

	//Blocks
	Block*	GetCurrentBlock();
	Block*	GetNextBlock();
	
	//Move methods
	void	MoveCurrentBlockX(int offset_x);
	void	MoveCurrentBlockY(int offset_y);
	void	RotateCurrentBlockCW();
	void	RotateCurrentBlockCCW();

	//Event Methods
	void	OnCollisionY(Callback cb);
	void	OnCollisionX(Callback cb);
	void	OnCheckLine(Callback cb);
	void	OnClearLine(Callback cb);
	void	OnRotateBlock(Callback cb);
	void	OnLevelIncrease(Callback cb);
	void	OnTetris(Callback cb);
	void	OnUpdateScore(Callback cb);

	enum State {
		PAUSED,
		EXITBYUSER,
		PLAYING,
		GAMEOVER
	};

	State GameStatus = PLAYING;


private:
	//Block Stuff
	void	EngageBlock();
	void	ImpressCurrentBlock();
	void	ResetCurrentBlock();
	Block*	ProduceBlock(int index);
	Block*	NextBlock;
	Block*	CurrentBlock;

	//Line Stuff
	int*	CheckLines();
	int		ClearLines(int* lines);
	int		Lines[4];
	
	//Misc
	bool	DetectCollision(int x, int y, Block * b);
	void	UpdateMetrics(int LineNumber);
	int		FrameSpeed(int Level);
	int		RandomBlockIndex();

	//Event callbacks
	Callback CollisionYCallback;
	Callback CollisionXCallback;
	Callback CheckLineCallback;
	Callback ClearLineCallback;
	Callback RotateBlockCallback;
	Callback LevelIncreaseCallback;
	Callback TetrisCallback;
	Callback UpdateScoreCallback;
};

