#include "stdafx.h"
#include "Audio.h"
#include <functional>

Audio::Audio(Game * g)
{
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	theme = Mix_LoadMUS("music\\music.wav");
	Pool[0] = Mix_LoadWAV("music\\move.wav");
	Pool[1] = Mix_LoadWAV("music\\collision.wav");
	Pool[2] = Mix_LoadWAV("music\\line.wav");
	Pool[3] = Mix_LoadWAV("music\\rotate.wav");
	Pool[4] = Mix_LoadWAV("music\\levelup.wav");
	Pool[5] = Mix_LoadWAV("music\\tetris.wav");
	Pool[6] = Mix_LoadWAV("music\\gameover.wav");
	Pool[7] = Mix_LoadWAV("music\\drop.wav");
	SetGame(g);
}

Audio::~Audio()
{
	for (int i = 0; i < 10; i++) {
		if (Pool[i] != NULL) {
			Mix_FreeChunk(Pool[i]);
		}
	}

	Mix_FreeMusic(theme);
	Mix_CloseAudio();
}

/*Plays a sound from the sound pool*/
void Audio::PlaySound(int index) {
	Mix_PlayChannel(-1, Pool[index], 0);
}

/*Stops a channel*/
void Audio::StopSound() {
	Mix_Pause(-1);
}

/*Starts the theme music*/
void Audio::StartMusic() {
	Mix_PlayMusic(theme, -1);
}

/*Stops the theme music*/
void Audio::StopMusic() {
	Mix_PauseMusic();
}

/*Sets the event handlers for all sounds*/
void Audio::SetGame(Game * g) {
	this->g = g;

	g->OnCollisionY([this] {
		PlaySound(1);
	});

	g->OnCheckLine([this] {
		PlaySound(2);
	});

	g->OnRotateBlock([this] {
		PlaySound(3);
	});

	g->OnLevelIncrease([this] {
		PlaySound(4);
	});

	g->OnTetrisCheck([this] {
		PlaySound(5);
	});

	g->OnClearLine([this] {
		PlaySound(7);
	});
}