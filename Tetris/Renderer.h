#pragma once
#include "Game.h"
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 750
#define TETRIS_AREA (SCREEN_WIDTH / 3)*2
#define FRAME_WIDTH SCREEN_WIDTH / 32
#define FRAME_HEIGHT SCREEN_HEIGHT / 32
#define BRICK_PADDING 0

class Renderer
{
public:
	Renderer(Game * g);
	~Renderer();

	void RenderBlock(Block* b);
	void RenderBlock(Block* b, bool isGhost);
	void RenderBlock(Block* b, int tile_x, int tile_y, int offset_x, int offset_y, bool isGhost);
	void RenderBrick(int tile_x, int tile_y, int color, int marginLeft, int marginTop, int padding, int offset_x, int offset_y, bool ghost);
	void RenderTexture(SDL_Texture* texture, int x, int y, int w, int h);
	void RenderFrame();
	void RenderPlayfield();
	void ToggleHighlightLine(int time, int * lines);
	int MessageBox(const char* msg, const char* title);
	
	void Clear();
	void Update();
	void SetGame(Game * g);

	SDL_Texture* PrintTextToTexture(char * msg, SDL_Texture * texture);

private:
	void CreateBrickTextures(int bevel);
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

	typedef struct color {
		int R;
		int G;
		int B;
	} Color;

	int ColorToInt(Color c);

};

