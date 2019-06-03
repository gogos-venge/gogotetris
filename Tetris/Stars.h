#pragma once

typedef struct vector {
	double x;
	double y;
	int angle;
	int speed;
} Vector;

class Stars {
public:
	Stars(int spawn_x, int spawn_y, int max_x, int max_y, int num);
	~Stars();

	Vector * stars;
	int		 Number;

	void	 AdvanceAllStars();

private:
	Vector	 spawn;
	int		 max_x;
	int		 max_y;

	void	 InitStar(Vector*);
	bool	 AdvanceStar(Vector*);
};

