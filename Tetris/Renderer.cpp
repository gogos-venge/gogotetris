#include "stdafx.h"
#include "Renderer.h"
#include <exception>

Renderer::Renderer(Game* g)
{
	SetGame(g);

	bool success = true;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		success = false;
	}
	else {
		gWindow = SDL_CreateWindow("GoGo Tetris!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL){
			success = false;
		}
		else {
			gScreenSurface = SDL_GetWindowSurface(gWindow);
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

			if (gRenderer == NULL) {
				success = false;
			}
		}
	}

	if (TTF_Init() < 0) {
		success = false;
	}
	else {
		OldStyle = TTF_OpenFont("PressStart2P.ttf", 36);
		if (OldStyle == NULL) {
			success = false;
		}
	}

	CreateFrame();
	CreateBrickTextures(6);

	if (!success) {
		throw SDL_GetError();
	}
}

Renderer::~Renderer()
{
	for (int i = 0; i < 7; i++) {
		SDL_DestroyTexture(Bricks[i]);
		SDL_DestroyTexture(GhostBricks[i]);
	}

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_DestroyTexture(Frame);
	SDL_Quit();
	TTF_Quit();
}

/*Renders a Block based on the current Block->x and Block->y position*/
void Renderer::RenderBlock(Block* b) {
	RenderBlock(b, false);
}

/*Renders a Block based on the current Block->x and Block->y position*/
void Renderer::RenderBlock(Block* b, bool isGhost) {
	RenderBlock(b, b->x, b->y, 0, 0, isGhost);
}

/*Renders a moving Block by using the RenderBrick method to a determined position on the Grid. Block's x/y tile position must be set by the user.*/
void Renderer::RenderBlock(Block* b, int tile_x, int tile_y, int offset_x, int offset_y, bool isGhost) {
	for (int i = 0; i < b->GetWidth(); i++) {
		for (int j = 0; j < b->GetHeight(); j++) {
			RenderBrick(tile_x + i, tile_y + j - 20, b->GetBrick(i, j), FRAME_WIDTH, FRAME_HEIGHT, 0, offset_x, offset_y, isGhost);
		}
	}
}

/*Renders a generated texture (Brick) to a 10x20 Grid. Tile Grid snapping is determined by the screen's width and height divided by the Game->Playfield's width and height.
This method also offers a margin, a padding and an offset parameter to control where the Grid starts*/
void Renderer::RenderBrick(int tile_x, int tile_y, int color, int marginLeft, int marginTop, int padding, int offset_x, int offset_y, bool ghost) {
	if (!color) return;

	int blockWidth = (TETRIS_AREA - marginLeft * 2) / g->WIDTH;
	int blockHeight = ((SCREEN_HEIGHT - marginTop * 2) * 2) / g->HEIGHT;

	int pos_x = tile_x * blockWidth;
	int pos_y = tile_y * blockHeight;

	RenderTexture(!ghost ? Bricks[color - 1] : GhostBricks[color - 1],
		pos_x + padding + marginLeft + offset_x,
		pos_y + padding + marginTop + offset_y,
		blockWidth - padding * 2,
		blockHeight - padding * 2
	);
}

/*Renders a texture over the given coordinates of a rectangle*/
void Renderer::RenderTexture(SDL_Texture* texture, int x, int y, int w, int h) {
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderCopy(gRenderer, texture, NULL, &rect);
}

/*Renders text to a texture*/
SDL_Texture* Renderer::PrintTextToTexture(char * msg, SDL_Texture * texture) {
	SDL_DestroyTexture(texture);
	SDL_Color c = { 255, 255, 0 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(OldStyle, msg, c);
	texture = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	return texture;
}

/*Renders the frame around the playfield*/
void Renderer::RenderFrame() {
	SDL_RenderCopy(gRenderer, Frame, NULL, &TetrisRect);
}

/*Renderes the static bricks*/
void Renderer::RenderPlayfield() {
	for (int i = 0; i < g->WIDTH; i++) {
		for (int j = 0; j < g->HEIGHT; j++) {
			if (g->Playfield[i][j]) {
				RenderBrick(i, j - 20, g->Playfield[i][j], FRAME_WIDTH, FRAME_HEIGHT, 0, 0, 0, false);
			}
		}
	}
}

/*Toggle a line from its normal color to white. Time is counter*/
void Renderer::ToggleHighlightLine(int time, int * lines) {
	for (int k = 0; k < 4; k++) {
		if (lines[k]) {
			for (int j = 0; j < g->WIDTH; j++) {
				int c = time % 2 ? g->Playfield[j][lines[k]] : 8;
				RenderBrick(j, lines[k] - 20, c, FRAME_WIDTH, FRAME_HEIGHT, 0, 0, 0, false);
			}
		}
	}
}

/*Generates a Yes/No message box*/
int Renderer::MessageBox(const char* msg, const char* title) {
	const SDL_MessageBoxButtonData buttons[] = {
		{ 0, 0, "No" },
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" }
	};
	const SDL_MessageBoxColorScheme colorScheme = {
		{
			{ 0xFF,0x00,0x00 },
			{ 0x00,0xFF,0x00 },
			{ 0xFF,0xFF,0x00 },
			{ 0x00,0x00,0xFF },
			{ 0xFF,0x00,0xFF }
		}
	};
	const SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_INFORMATION,
		gWindow,
		title,
		msg,
		SDL_arraysize(buttons),
		buttons,
		&colorScheme
	};
	int buttonid;
	SDL_ShowMessageBox(&messageboxdata, &buttonid);
	return buttonid;
}

/*Clears the back of the renderer*/
void Renderer::Clear() {
	SDL_SetRenderDrawColor(gRenderer, 0x10, 0, 0x10, 0xFF);
	SDL_RenderClear(gRenderer);
}

/*Updates the current frame using RenderPresent. Must be used after all changes done to the renderer*/
void Renderer::Update() {
	SDL_RenderPresent(gRenderer);
}

/*Sets a new game*/
void Renderer::SetGame(Game * g) {
	this->g = g;
}

/*Generates the beveled pseudo-3d textures for each color*/
void Renderer::CreateBrickTextures(int bevel) {

	int blockWidth = (TETRIS_AREA - FRAME_WIDTH * 2) / g->WIDTH;
	int blockHeight = blockWidth;
	Color c[8] = {
		{ 0xFF,0x00,0xF0,0xF0 },
		{ 0xFF,0x00,0x00,0xF0 },
		{ 0xFF,0xF0,0xA0,0x00 },
		{ 0xFF,0xFF,0xFF,0x00 },
		{ 0xFF,0x00,0xF0,0x00 },
		{ 0xFF,0xF0,0x00,0xF0 },
		{ 0xFF,0xF0,0x00,0x00 },
		{ 0xFF,0xFF,0xFF,0xFF } //<-- white for brick flashing
	};

	for (int b = 0; b < 8; b++) {
		Uint32* pixels = (Uint32*)malloc(blockWidth * blockHeight * sizeof(Uint32));
		for (int i = 0; i < blockHeight; i++) {
			for (int j = 0; j < blockWidth; j++) {
				Color tmp = { 255,0,0,0 };
				if (i < bevel && j > i && j < blockWidth - i) {
					tmp.R = c[b].R | 0xA0;
					tmp.G = c[b].G | 0xA0;
					tmp.B = c[b].B | 0xA0;
					pixels[blockWidth * i + j] = ColorToInt(tmp);
				}
				else if (i >= blockHeight - bevel && j >= blockHeight - i && j <= i) {
					tmp.R = c[b].R / 3;
					tmp.G = c[b].G / 3;
					tmp.B = c[b].B / 3;
					pixels[blockWidth * i + j] = ColorToInt(tmp);
				}
				else if (j < bevel) {
					tmp.R = c[b].R / 2;
					tmp.G = c[b].G / 2;
					tmp.B = c[b].B / 2;
					pixels[blockWidth * i + j] = ColorToInt(tmp);
				}
				else if (j > blockWidth - bevel) {
					tmp.R = c[b].R / 2;
					tmp.G = c[b].G / 2;
					tmp.B = c[b].B / 2;
					pixels[blockWidth * i + j] = ColorToInt(tmp);
				}
				else {
					pixels[blockWidth * i + j] = ColorToInt(c[b]);
				}
			}
		}

		Bricks[b] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, blockWidth, blockHeight);
		SDL_UpdateTexture(Bricks[b], NULL, pixels, blockWidth * sizeof(Uint32));
		SDL_SetTextureBlendMode(Bricks[b], SDL_BLENDMODE_BLEND);

		

		for (int k = 0; k < blockWidth * blockHeight; k++) {
			if (pixels[k]) {
				pixels[k] -= 0xE0000000; //alpha
			}
		}

		GhostBricks[b] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, blockWidth, blockHeight);
		SDL_UpdateTexture(GhostBricks[b], NULL, pixels, blockWidth * sizeof(Uint32));
		SDL_SetTextureBlendMode(GhostBricks[b], SDL_BLENDMODE_BLEND);
		
		free(pixels);
	}
}

/*Generates the frame around the playfield*/
void Renderer::CreateFrame() {
	const int WIDTH = TETRIS_AREA;
	int * pixels = (int*)malloc(TETRIS_AREA * SCREEN_HEIGHT * sizeof(Uint32));
	memset(pixels, 0x20, TETRIS_AREA*SCREEN_HEIGHT * sizeof(Uint32));

	for (int i = 0; i < TETRIS_AREA; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			if (j < FRAME_HEIGHT || i < FRAME_WIDTH || i > WIDTH - FRAME_WIDTH || j > SCREEN_HEIGHT - FRAME_HEIGHT) {
				pixels[TETRIS_AREA * j + i] = 0x98317e;
			}
		}
	}
	Frame = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, TETRIS_AREA, SCREEN_HEIGHT);
	SDL_UpdateTexture(Frame, NULL, pixels, TETRIS_AREA * sizeof(Uint32));

	TetrisRect = { 0,0,TETRIS_AREA, SCREEN_HEIGHT };

	free(pixels);
}

/*Translates an RGB Color to an int (with alpha)*/
int Renderer::ColorToInt(Color c) {
	return c.A << 24 | c.R << 16 | c.G << 8 | c.B;
}