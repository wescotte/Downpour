#include "Piece.h"

Piece::Piece()
{
    color=0;
    x=0;
    y=0;
    frame=0;
    for (int i=0;i < 4;++i)
        state[i]=false;
    return;
}

Piece::Piece(int Color, int X, int Y)
{
    color=Color;
    x=X;
    y=Y;
    frame=0;
    for (int i=0; i < 4;++i)
        state[i]=false;
    return;
}

Piece::Piece(const Piece& piece)
{
	this->color=piece.color;
	this->x=piece.x;
	this->y=piece.y;
	this->visited=false;
	for (int i=0; i < 4;++i)
		this->state[i]=piece.state[i];
	this->frame=piece.frame;
}

Piece& Piece::operator=(const Piece &piece) 
{
	this->color=piece.color;
	this->x=piece.x;
	this->y=piece.y;
	this->visited=false;
	for (int i=0; i < 4;++i)
		this->state[i]=piece.state[i];
	this->frame=piece.frame;
	return *this;
}

