#ifndef Board_H
#define Board_H

#include <vector>
#include <stdlib.h>

#include "Piece.h"
#include "Settings.h"
#include "Timer.h"

using namespace std;

class GameData;

class Board {
public:
    Board(GameData* GD,int pnum);
    ~Board();
    
	void BoardCheck();
	void ComboCheck(int X,int y);
	void RemoveCombo();
	void ComboCalc();
	void ClearStates();
	void ClearPieceCheck(int x,int y);
	void FillBoardWClears();
	void ClearBoard();
	
	void MovePiece();
	void NextPiece();	
	void DropPieces(bool check=true);	
    void RotateRight();
    void RotateLeft();
    void MoveRight();
    void MoveLeft();
    
    // Specials related functions
    void ClearSpecials();
    void UseSpecial(int special, int to, int from,int randnum);
    void AddSpecial(int special);
    void DropClears(int num2drop, int randnum);
    void ClearLine();
    void NukeBoard();
    void SwitchBoards(int from);
    void FloatingPieces();
    
    GameData* Gamedata;
    int Pnum; // Player Number of this board
    bool IsPlaying;
	Piece *board[6][13]; // 13th row is filled with -1 color so pieces don't
			// fall off the board
	int specials[10]; 
	Timer FloatTime; // Floating Pieces timer
	Timer ClearDropRate; // Time delay until all clears are dropped
	int Num2Drop;      // Number of Clears coming!
	Piece CurPiece[2], NxtPiece[2];
	
	vector <Piece*> ComboPieces;
	vector <int> ComboCount;
};

int RandNum(int start, int finish);

#endif
