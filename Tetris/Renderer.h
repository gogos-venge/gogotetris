#pragma once
#include "Game.h"
#include "Stars.h"
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750
#define TETRIS_AREA SCREEN_WIDTH / 3
#define FRAME_WIDTH 15
#define FRAME_HEIGHT 15
#define TOP_OFFSET 0
#define LEFT_OFFSET TETRIS_AREA
#define BRICK_WIDTH (TETRIS_AREA - FRAME_WIDTH * 2) / GAME_WIDTH
#define BRICK_HEIGHT (SCREEN_HEIGHT  - FRAME_HEIGHT * 2) / (GAME_HEIGHT / 2)
#define SCORE_TEXT_HEIGHT 50
#define LEVEL_TEXT_HEIGHT 100
#define LINES_TEXT_HEIGHT 150
#define NEXT_TEXT_HEIGHT 500

#define HELD_TEXT_HEIGHT 50

class Renderer
{
public:
	Renderer(Game * g);
	~Renderer();

	typedef struct color {
		int A;
		int R;
		int G;
		int B;
	} Color;

	void RenderBlock(Block* b);
	void RenderBlock(Block* b, bool isGhost);
	void RenderBlock(Block* b, int tile_x, int tile_y, int offset_x, int offset_y, bool isGhost);
	void RenderBlock(Block* b, int tile_x, int tile_y, int tile_w, int tile_h, int offset_x, int offset_y, bool isGhost);
	void RenderBrick(int tile_x, int tile_y, int width_x, int width_y, int color, int marginLeft, int marginTop, int padding, int offset_x, int offset_y, bool ghost);
	void RenderTexture(SDL_Texture* texture, int x, int y, int w, int h);
	void RenderFrame();
	void RenderPlayfield();
	void RenderAnimateStars(Stars*);
	void ToggleHighlightLine(int time, int * lines);
	int	 MessageBox(const char* msg, const char* title);
	
	void Clear();
	void Update();
	void SetGame(Game * g);

	SDL_Texture* PrintTextToTexture(const char * msg, SDL_Texture * texture);

private:
	void CreateBrickTextures(int bevel);
	void CreateArikaTextures(int bevel);
	void CreateFrame();
	
	SDL_Window*		gWindow;
	SDL_Surface*	gScreenSurface;
	SDL_Renderer*	gRenderer;
	SDL_Texture*	Frame;
	SDL_Rect		TetrisRect;
	SDL_Texture*	Bricks[8];
	SDL_Texture*	GhostBricks[8];

	TTF_Font*		OldStyle;

	Game * g;

	int ColorToInt(Color c);
};

