#include "piece.h"

Piece::Piece()
{
	side[0] = side[1] = side[2] = side[3] = id = rotation = 0;
}

Piece::Piece(int up, int right, int down, int left)
{
	side[0] = up;
	side[1] = right;
	side[2] = down;
	side[3] = left;
	rotation = 0;
}

Piece::Piece(int up, int right, int down, int left, int id)
{
	side[0] = up;
	side[1] = right;
	side[2] = down;
	side[3] = left;
	this->id = id;
	rotation = 0;
}

int Piece::UP()
{
	return side[rotation % 4];
}

int Piece::RIGHT()
{
	return side[(rotation + 1) % 4];
}

int Piece::DOWN()
{
	return side[(rotation + 2) % 4];
}

int Piece::LEFT()
{
	return side[(rotation + 3) % 4];
}

int Piece::ID()
{
	return id;
}

void Piece::ID(int val)
{
	id = val;
}

void Piece::UP(int val)
{
	side[rotation % 4] = val;
}

void Piece::RIGHT(int val)
{
	side[(rotation + 1 ) % 4] = val;
}

void Piece::DOWN(int val)
{
	side[(rotation + 2) % 4] = val;
}

void Piece::LEFT(int val)
{
	side[(rotation + 3 ) % 4] = val;
}