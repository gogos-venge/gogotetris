#pragma once
class Audio
{
public:
	Audio(Game * g);
	~Audio();
	void PlaySound(int index);
	void StopSound();
	void StartMusic();
	void StopMusic();
	void SetGame(Game * g);

private:

	Game * g;
	Mix_Chunk * Pool[10];
	Mix_Music * theme;
	int Position;
};

