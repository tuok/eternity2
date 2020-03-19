#ifndef PIECE_H
#define PIECE_H

class Piece
{
	private:
		int side[4];
		int id;
		int rotation;

	public:
		Piece();
		Piece(int up, int right, int down, int left);
		Piece(int up, int right, int down, int left, int id);

		int UP();
		int DOWN();
		int LEFT();
		int RIGHT();

		void UP(int val);
		void DOWN(int val);
		void LEFT(int val);
		void RIGHT(int val);

		int ID();
		void ID(int val);
};


#endif