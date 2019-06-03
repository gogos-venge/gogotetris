#include "stdafx.h"
#include "Renderer.h"
#include "Audio.h"

void Initialize();
void CreateScoreTextures();
void CheckKeys();
void OnCollisionLogic();
void RenderScreen();
void MovementX();

Game*		g;
Renderer*	renderer;
Audio*		audio;
Stars*		stars;

//control counter
int c = 0;

//logic counter
int l = 0;

/*These variables are controlled by the keys. 
They are used to override the initial keyboard
delay lock and implement a custom one*/
int move_x = 0;
int move_y = 0;
int lock_delay = 4;
int lock_time = 0;
int poll = 0;

/*These textures are used for optimization.
Generating these textures once per frame
is a bit costly cpu wise. Instead we update them
OnUpdateScore and just render them once per frame.
The rendering is not cpu consuming*/
SDL_Texture* score;
SDL_Texture* level;
SDL_Texture* lines;
SDL_Texture* next;
SDL_Texture* held;
SDL_Texture* floating;

/*Misc*/
int linesToBeCleared[4];
char disp[80];

int main(const int argc, char* args[])
{
	Initialize();

	while (g->GameStatus != Game::EXITBYUSER) {
		//Call this once to write 0 score etc.
		CreateScoreTextures();

		//start Meta Crystal Theme!
		audio->StartMusic();

		//Event Listeners
		g->OnUpdateScore(CreateScoreTextures);
		g->OnCollisionY(OnCollisionLogic);

		while (g->GameStatus != Game::GAMEOVER && g->GameStatus != Game::EXITBYUSER) {
			/*So about SDL_Delay(16), if you want to achieve an fps stable game this is NOT the proper way.
			SDL delay relies upon system's timer resolution which is roughly 15ms on Windows.
			Again it is not guaranteed this will be 15ms, mainly because the scheduler may or may
			not switch to this thread on time cause of heavy load, but in our case since this isn't
			an official or tournament tetris, being super accurate would be an overkill. In any case
			if you want to change this to something more accurate, I suggest you do spinwaits after
			determining the cpu speed or sync to vsync. But bear in mind cpu spins = more cpu time used = more cpu load*/
			SDL_Delay(16);

			/*KeyPress*/
			CheckKeys();

			if (g->GameStatus == Game::PAUSED) {
				continue;
			}

			MovementX();

			l++;

			if (!move_y) {
				if (l % g->SpeedFps == 0) {
					g->MoveCurrentBlockY(1);
				}
			}
			else {
				if (!(l % (3 > g->SpeedFps ? g->SpeedFps : 3))) {
					g->MoveCurrentBlockY(1);
					g->Score++;
				}
			}

			/*Rendering*/
			renderer->Clear();
			RenderScreen();
			renderer->Update();
		}

		/*Check Game Status*/
		if (g->GameStatus == Game::GAMEOVER) {
			audio->StopMusic();
			audio->PlaySound(6);
			if (renderer->MessageBox("Play again?", "Sorry you lost") == 0) {
				g->GameStatus = Game::EXITBYUSER;
			}
			else {
				audio->StopSound();

				delete g;
				delete audio;
				g = new Game(0);
				audio = new Audio(g);
				renderer->SetGame(g);
			}
		}
	}
	
	delete renderer;
	delete audio;
	delete g;
	
    return 0;
}

/*Initializes some stuff*/
void Initialize() {
	 
	//Start a new game. Level as parameter
	g = new Game(0);

	//Start audio
	audio = new Audio(g);

	//Start the renderer wrapper class
	renderer = new Renderer(g);

	//Starts pretty stars
	stars = new Stars(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 80);

	//Print label textures
	next = renderer->PrintTextToTexture("Next:", next);
	held = renderer->PrintTextToTexture("Held:", held);
}

/*Prints the metrics to textures*/
void CreateScoreTextures() {

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
				g->RotateCurrentBlockCW();
				break;

			case SDLK_s:
				g->RotateCurrentBlockCCW();
				break;

			case SDLK_RETURN:
				g->GameStatus = g->GameStatus == g->PLAYING ? g->PAUSED : g->PLAYING;
				break;

			case SDLK_SPACE:
				if (poll++ == 0) {
					g->HardDropCurrentBlock();
				}
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

				lock_delay = 1;
				lock_time = 0;
				c = 0;
				break;

			case SDLK_RIGHT:
				if (move_x == 1)
					move_x = 0;

				lock_delay = 1;
				lock_time = 0;
				c = 0;
				break;

			case SDLK_DELETE:
				g->HoldCurrentBlock();
				break;

			case SDLK_SPACE:
				poll = 0;
				break;
			}
		}
	}
}

/*Essentially all the game's logic*/
void OnCollisionLogic() {

	Block* b = g->GetCurrentBlock();
	
	g->ImpressCurrentBlock();

	//Line flashing
	if (int lines = g->CheckLines(linesToBeCleared)) {
		sprintf_s(disp, lines == 4 ? "Tetris! +%04d": "Score +%04d", g->UpdateMetrics(lines));
		floating = renderer->PrintTextToTexture(disp, floating);

		for (int i = 0; i < 30; i++) {
			renderer->Clear();
			RenderScreen();

			//animate line
			renderer->ToggleHighlightLine(i, linesToBeCleared);

			//calculate cool floating text
			int x = (b->x * SCREEN_WIDTH / 10) - 250;
			x = x < 0 ? 0 : x > TETRIS_AREA ? TETRIS_AREA : x;
			int y = b->y * (SCREEN_HEIGHT / 40) - 50;
			renderer->RenderTexture(floating , x, y - i, 250, 30);

			//sleep a bit
			SDL_Delay(10);
			renderer->Update();
		}
	}


	g->ClearLines(linesToBeCleared);
	g->EngageBlock(true);
	CreateScoreTextures();

	//Set logic counter to zero
	l = 0;
}

/*Renders Everything*/
void RenderScreen() {
	//Renders pretty stars
	renderer->RenderAnimateStars(stars);

	//Draw bricks, static and moving ones
	renderer->RenderPlayfield();
	renderer->RenderBlock(g->GetGhostBlock(), true);
	renderer->RenderBlock(g->GetCurrentBlock());

	if (g->GetHeldBlock() != NULL) { renderer->RenderBlock(g->GetHeldBlock(), 0, 0, TETRIS_AREA + 20, SCREEN_HEIGHT + HELD_TEXT_HEIGHT + 10, false); }
	renderer->RenderBlock(g->GetNextBlock(), 0, 0, TETRIS_AREA + 20, SCREEN_HEIGHT + NEXT_TEXT_HEIGHT + 10, false);
	
	//Render Text textures
	renderer->RenderTexture(score, TETRIS_AREA + 20, SCORE_TEXT_HEIGHT, 180, 30);
	renderer->RenderTexture(level, TETRIS_AREA + 20, LEVEL_TEXT_HEIGHT, 150, 28);
	renderer->RenderTexture(lines, TETRIS_AREA + 20, LINES_TEXT_HEIGHT, 150, 28);

	renderer->RenderTexture(held, TETRIS_AREA + 20, HELD_TEXT_HEIGHT, 90, 28);
	renderer->RenderTexture(next, TETRIS_AREA + 20, NEXT_TEXT_HEIGHT, 90, 28);

	//Draw the purple frame around Playfield
	renderer->RenderFrame();
}

void MovementX() {
	/*Movement*/
	if (move_x != 0) {
		if (!(c % lock_delay)) {
			lock_delay = 4;
			if (lock_time++ == 0) {
				lock_delay = 10;
			}
			audio->PlaySound(0);
			g->MoveCurrentBlockX(move_x);
		}
		c++;
	}
}