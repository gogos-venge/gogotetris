#include "stdafx.h"
#include "Renderer.h"
#include "Audio.h"
#include <algorithm>
#include <vector>

void CreateScoreTextures();
void CheckKeys();
void PrintDebug();

Game*		g;
Renderer*	renderer;
Audio*		audio;

int c = 0;

/*These variables are controlled by the keys. 
They are used to override the initial keyboard
delay lock and implement a custom one*/
int move_x = 0;
int move_y = 0;
int rotate = 0;

/*These textures are used for optimization.
Generating these textures once per frame
is a bit costly cpu wise. Instead we update them
OnUpdateScore and just render them once per frame.
The rendering is not cpu consuming*/
SDL_Texture* score;
SDL_Texture* level;
SDL_Texture* lines;

int main(const int argc, char* args[])
{
	//Start a new game. Level as parameter
	g = new Game(0);

	//Start audio
	audio = new Audio(g);

	//Start the renderer wrapper class
	renderer = new Renderer(g);

	while (g->GameStatus != Game::EXITBYUSER) {
		CreateScoreTextures();

		audio->StartMusic();
		g->OnUpdateScore([&] {
			CreateScoreTextures();
		});

		while (g->GameStatus != Game::GAMEOVER && g->GameStatus != Game::EXITBYUSER) {
			SDL_Delay(16);

			//Keypress
			CheckKeys();

			if (g->GameStatus == Game::PAUSED) {
				continue;
			}

			//Logic
			if (rotate) {
				if (rotate == 1) {
					g->RotateCurrentBlockCW();
				}
				else {
					g->RotateCurrentBlockCCW();
				}
				rotate = 0;
			}

			c++;

			if (move_x && !(c % 4)) {
				audio->PlaySound(0);
				g->MoveCurrentBlockX(move_x);
			}

			if (!move_y) {
				if (c % g->SpeedFps == 0) {
					g->MoveCurrentBlockY(1);
				}
			}
			else {
				if (!(c % (3 > g->SpeedFps ? g->SpeedFps : 3))) {
					g->MoveCurrentBlockY(1);
					g->Score++;
				}
			}

			//Clear Back
			renderer->Clear();

			//Draw the purple frame aroud Playfield
			renderer->RenderFrame();

			//Draw bricks, static and moving ones
			renderer->RenderPlayfield();
			renderer->RenderBlock(g->GetCurrentBlock());
			renderer->RenderBlock(g->GetNextBlock(), 11, 35, 30, 0, false);

			//Render Text textures
			renderer->RenderTexture(score, TETRIS_AREA + 20, 50, 180, 30);
			renderer->RenderTexture(level, TETRIS_AREA + 20, 200, 150, 28);
			renderer->RenderTexture(lines, TETRIS_AREA + 20, 350, 150, 28);

			//Update Screen
			renderer->Update();
		}

		if (g->GameStatus == Game::GAMEOVER) {
			audio->StopMusic();
			audio->PlaySound(6);
			if (renderer->MessageBox("Play again?", "Sorry you lost") == 0) {
				g->GameStatus = Game::EXITBYUSER;
			}
			else {
				delete g;
				g = new Game(0);
				renderer->SetGame(g);
				audio->SetGame(g);
			}
		}
	}
	
	delete renderer;
	delete audio;
	delete g;
	
    return 0;
}

/*Prints the metrics to textures*/
void CreateScoreTextures() {
	char disp[80];
	sprintf_s(disp, "Score %04d", g->Score);
	score = renderer->PrintTextToTexture(disp, score);
	sprintf_s(disp, "Level %d", g->Level);
	level = renderer->PrintTextToTexture(disp, level);
	sprintf_s(disp, "Lines %d", g->TotalLines);
	lines = renderer->PrintTextToTexture(disp, lines);
}

/*Checks pressed keys. Implements a different keyboard delay system than windows*/
void CheckKeys() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
		{
			g->GameStatus = g->EXITBYUSER;
		}

		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				break;

			case SDLK_DOWN:
				move_y = 1;
				break;

			case SDLK_LEFT:
				move_x = -1;
				break;

			case SDLK_RIGHT:
				move_x = 1;
				break;

			case SDLK_a:
				rotate = 1;
				break;

			case SDLK_s:
				rotate = -1;
				break;

			case SDLK_RETURN:
				g->GameStatus = g->GameStatus == g->PLAYING ? g->PAUSED : g->PLAYING;
				break;
			}
			
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				break;

			case SDLK_DOWN:
				move_y = 0;
				break;

			case SDLK_LEFT:
				if (move_x == -1) //fast toggling from left to right
					move_x = 0;
				break;

			case SDLK_RIGHT:
				if (move_x == 1)
					move_x = 0;
				break;
			}
		}
	}
}

void PrintDebug() {
	system("cls");
	Block * b = g->GetCurrentBlock();
	for (int i = 20; i < Game::HEIGHT; i++) {
		for (int j = 0; j < Game::WIDTH; j++) {
			if (j >= b->x && j < b->x + b->GetWidth() && i >= b->y && i < b->y + b->GetHeight()) {
				printf("| %d |", b->GetBrick(j - b->x, i - b->y));
			}
			else {
				printf("| %d |", g->Playfield[j][i]);
			}
			
		}
		printf("\n");
	}
}