#pragma once
class Block
{
public:
	int x;
	int y;
	int type;

	virtual int GetBrick(int pos_x, int pos_y) = 0;
	virtual void SetBrick(int pos_x, int pos_y, int color) = 0;

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual void RotateCW() = 0;
	virtual void RotateCCW() = 0;

	static Block* Factory(int type);

};

class Normal : public Block {
public:
	int GetWidth();
	int GetHeight();
	int GetBrick(int x, int y);
	void SetBrick(int x, int y, int color);

	void RotateCW();
	void RotateCCW();

private:
	static const int WIDTH = 3;
	static const int HEIGHT = 3;

	int Bricks[WIDTH][HEIGHT] = {};
};

class Square : public Block {
public:
	Square();
	int GetWidth();
	int GetHeight();
	int GetBrick(int x, int y);
	void SetBrick(int x, int y, int color);

	void RotateCW();
	void RotateCCW();

private:
	static const int WIDTH = 4;
	static const int HEIGHT = 3;

	int Bricks[WIDTH][HEIGHT] = {};
};

class Line: public Block {
public:
	Line();
	int GetWidth();
	int GetHeight();
	int GetBrick(int x, int y);
	void SetBrick(int x, int y, int color);

	void RotateCW();
	void RotateCCW();

private:
	static const int WIDTH = 4;
	static const int HEIGHT = 4;

	int Bricks[WIDTH][HEIGHT] = {};
};
