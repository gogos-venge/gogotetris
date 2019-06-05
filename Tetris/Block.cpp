#include "stdafx.h"
#include "Block.h"

/*Generates a new Block. 
0) Line
1) Blue J
2) Orange L
3) Yellow Square
4) Green S
5) Purple T
6) Red Z*/
Block* Block::Factory(int type) {

	Block* b = new Normal();
	
	// I (Light Blue)
	switch (type) {
		case 0:
			b = new Line();
			break;
		case 1:
			// J (Blue)
			b->SetBrick(2, 0, 2);
			b->SetBrick(1, 0, 2);
			b->SetBrick(1, 1, 2);
			b->SetBrick(1, 2, 2);
			break;
		case 2:
			// L (Orange)
			b->SetBrick(0, 0, 3);
			b->SetBrick(1, 0, 3);
			b->SetBrick(1, 1, 3);
			b->SetBrick(1, 2, 3);
			break;
		case 3:
			// T (Purple)
			b->SetBrick(0, 1, 6);
			b->SetBrick(1, 1, 6);
			b->SetBrick(2, 1, 6);
			b->SetBrick(1, 2, 6);
			break;
		case 4:
			// O (Yellow)
			b = new Square();
			break;
		case 5:
			// S (Green)
			b->SetBrick(1, 0, 5);
			b->SetBrick(2, 0, 5);
			b->SetBrick(0, 1, 5);
			b->SetBrick(1, 1, 5);
			break;
		case 6:
			// Z (Red)
			b->SetBrick(0, 0, 7);
			b->SetBrick(1, 0, 7);
			b->SetBrick(1, 1, 7);
			b->SetBrick(2, 1, 7);
			break;
	}

	b->type = type;
	return b;
}

/*Normal blocks, are 3x3 blocks: Z,S,L,J,T*/
int Normal::GetWidth() {
	return WIDTH;
}

int Normal::GetHeight() {
	return HEIGHT;
}

int Normal::GetBrick(int x, int y) {
	return Bricks[x][y];
}

void Normal::SetBrick(int x, int y, int color) {
	Bricks[x][y] = color;
}

void Normal::RotateCW() {
	int N = WIDTH;
	for (int x = 0; x < N / 2; x++)
	{
		for (int y = x; y < N - x - 1; y++)
		{
			int temp = Bricks[x][y];
			Bricks[x][y] = Bricks[y][N - 1 - x];
			Bricks[y][N - 1 - x] = Bricks[N - 1 - x][N - 1 - y];
			Bricks[N - 1 - x][N - 1 - y] = Bricks[N - 1 - y][x];
			Bricks[N - 1 - y][x] = temp;
		}
	}
}

void Normal::RotateCCW() {
	int N = WIDTH;
	for (int x = 0; x < N / 2; x++)
	{
		for (int y = x; y < N - x - 1; y++)
		{
			int temp = Bricks[N - 1 - y][x];
			Bricks[N - 1 - y][x] = Bricks[N - 1 - x][N - 1 - y];
			Bricks[N - 1 - x][N - 1 - y] = Bricks[y][N - 1 - x];
			Bricks[y][N - 1 - x] = Bricks[x][y];
			Bricks[x][y] = temp;
		}
	}
}


/*Square is the O block. It's 4x3 and features no rotation*/
Square::Square() {
	Bricks[0][0] = 4;
	Bricks[1][0] = 4;
	Bricks[0][1] = 4;
	Bricks[1][1] = 4;
}

int Square::GetWidth() {
	return WIDTH;
}

int Square::GetHeight() {
	return HEIGHT;
}

int Square::GetBrick(int x, int y) {
	return Bricks[x][y];
}

void Square::SetBrick(int x, int y, int color) {
}

void Square::RotateCW() {
	
}

void Square::RotateCCW() {

}

/*Line is the I block. Features 4*/
Line::Line() {
	Bricks[1][0] = 1;
	Bricks[1][1] = 1;
	Bricks[1][2] = 1;
	Bricks[1][3] = 1;
}

int Line::GetWidth() {
	return WIDTH;
}

int Line::GetHeight() {
	return HEIGHT;
}

int Line::GetBrick(int x, int y) {
	return Bricks[x][y];
}

void Line::SetBrick(int x, int y, int color) {
}

void Line::RotateCW() {
	int N = WIDTH;
	for (int x = 0; x < N / 2; x++)
	{
		for (int y = x; y < N - x - 1; y++)
		{
			int temp = Bricks[x][y];
			Bricks[x][y] = Bricks[y][N - 1 - x];
			Bricks[y][N - 1 - x] = Bricks[N - 1 - x][N - 1 - y];
			Bricks[N - 1 - x][N - 1 - y] = Bricks[N - 1 - y][x];
			Bricks[N - 1 - y][x] = temp;
		}
	}
}

void Line::RotateCCW() {
	int N = WIDTH;
	for (int x = 0; x < N / 2; x++)
	{
		for (int y = x; y < N - x - 1; y++)
		{
			int temp = Bricks[N - 1 - y][x];
			Bricks[N - 1 - y][x] = Bricks[N - 1 - x][N - 1 - y];
			Bricks[N - 1 - x][N - 1 - y] = Bricks[y][N - 1 - x];
			Bricks[y][N - 1 - x] = Bricks[x][y];
			Bricks[x][y] = temp;
		}
	}
}