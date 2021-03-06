#include "stdafx.h"
#include "Renderer.h"
#include "Audio.h"

void Initialize();
void CreateScoreTextures();
void CheckKeys();
void OnCollisionLogic();
void RenderScreen();
void MovementX();
void LevelUp();

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
bool harddrop = false;

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
		g->OnLevelIncrease(LevelUp);

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

			if (harddrop) {
				harddrop = false;
				g->HardDropCurrentBlock();
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
				delete stars;
				g = new Game(0);
				audio = new Audio(g);
				stars = new Stars(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 50);
				renderer->SetGame(g);
			}
		}
	}
	
	delete stars;
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
	stars = new Stars(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 10);

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
				if (!poll) {
					harddrop = true;
					poll = 1;
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
		sprintf_s(disp, lines == 4 ? "Tetris! +%04d": "Clear! +%02d", g->UpdateMetrics(lines));
		floating = renderer->PrintTextToTexture(disp, floating);

		for (int i = 0; i < 30; i++) {
			renderer->Clear();
			RenderScreen();

			//animate line
			renderer->ToggleHighlightLine(i, linesToBeCleared);

			//calculate cool floating text
			int x = (b->x * BRICK_WIDTH) + LEFT_OFFSET;
			x = x < LEFT_OFFSET ? LEFT_OFFSET : x > TETRIS_AREA + LEFT_OFFSET - 250 ? TETRIS_AREA + LEFT_OFFSET - 250 : x;
			int y = (b->y - 22) * BRICK_HEIGHT;
			renderer->RenderTexture(floating , x, y - i, 250, 30);

			//sleep a bit
			SDL_Delay(15);
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

	if (g->GetHeldBlock() != NULL) { renderer->RenderBlock(g->GetHeldBlock(), 0, 0, 200, SCREEN_HEIGHT, false); }
	renderer->RenderBlock(g->BlockBag[0], 0, 0, LEFT_OFFSET + TETRIS_AREA, SCREEN_HEIGHT + NEXT_TEXT_HEIGHT + 10, false);
	
	for (int i = 0; i < 2; i++) {
		renderer->RenderBlock(g->BlockBag[i + 1], 0, 0, 20, 20, LEFT_OFFSET + TETRIS_AREA + 160 + i * 100, SCREEN_HEIGHT + NEXT_TEXT_HEIGHT - 300, false);
	}
	
	//Render Text textures
	renderer->RenderTexture(score, LEFT_OFFSET + TETRIS_AREA + 20, SCORE_TEXT_HEIGHT, 250, 30);
	renderer->RenderTexture(level, LEFT_OFFSET + TETRIS_AREA + 20, LEVEL_TEXT_HEIGHT, 150, 28);
	renderer->RenderTexture(lines, LEFT_OFFSET + TETRIS_AREA + 20, LINES_TEXT_HEIGHT, 150, 28);

	renderer->RenderTexture(held, 100, HELD_TEXT_HEIGHT, 90, 28);
	renderer->RenderTexture(next, LEFT_OFFSET + TETRIS_AREA + 20, NEXT_TEXT_HEIGHT, 90, 28);

	//Draw the purple frame around Playfield
	renderer->RenderFrame();
}

void MovementX() {
	/*Movement*/
	if (move_x != 0) {
		if (!(c % lock_delay)) {
			lock_delay = 3;
			if (lock_time++ == 0) {
				lock_delay = 14;
			}
			audio->PlaySound(0);
			g->MoveCurrentBlockX(move_x);
		}
		c++;
	}
}

void LevelUp() {
	delete stars;
	stars = new Stars(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, (float)g->Level / 2 * 75);
}