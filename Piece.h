#ifndef Piece_H
#define Piece_H

class Piece {
public:
    Piece();
    Piece(int Color, int X, int Y);
    Piece(const Piece& piece);
    Piece& operator=(const Piece &piece);
    
	int x,y,color;
	bool state[4];
	bool visited;
    int frame;	
};

#endif
