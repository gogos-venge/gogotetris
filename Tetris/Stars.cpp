#include "stdafx.h"
#include "Stars.h"


Stars::Stars(int spawn_x, int spawn_y, int max_x, int max_y, int num)
{
	stars = (Vector*)malloc(num * sizeof(Vector));
	Number = num;
	this->max_x = max_x;
	this->max_y = max_y;

	this->spawn.x = spawn_x;
	this->spawn.y = spawn_y;

	for (int i = 0; i < num; i++) {
		InitStar(&stars[i]);
	}
}


Stars::~Stars()
{
	free(stars);
}

void Stars::AdvanceAllStars()
{
	for (int i = 0; i < Number; i++) {
		if (!AdvanceStar(&stars[i])) {
			InitStar(&stars[i]);
		}
	}
}

void Stars::InitStar(Vector* star) {
	star->angle = rand() % 361;
	star->x = spawn.x;
	star->y = spawn.y;
	star->speed = (rand() % 4) + 4;

	for (int i = 0; i < 20 + (rand() % 200); i++) {
		AdvanceStar(star);
	}
}

bool Stars::AdvanceStar(Vector* star) {
	if (star->x > max_x || star->y > max_y || star->x < 0 || star->y < 0) return false;
	star->x = star->x + cos(star->angle) * star->speed;
	star->y = star->y + sin(star->angle) * star->speed;

	return true;
}
