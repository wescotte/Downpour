#include "Board.h"
#include "GameData.h"

Board::Board(GameData *GD,int pnum)
{
    Gamedata=GD;
    Pnum=pnum;
    IsPlaying=false;
    // Create a seed for the random function
    srand(time(NULL));
    
    for (int y=0; y < 13;++y)
        for (int x=0; x < 6;++x) 
                board[x][y]=NULL;
    
    // Set bottom boundry so pieces don't fall off the board
    for (int i=0; i < 6;++i)
        board[i][12]=new Piece(0,i,12);
    
    // Clear specials
    ClearSpecials();
    
    NextPiece();
    ClearDropRate.ChangeDelay(Gamedata->settings->cleardroprate);  
    FloatTime.ChangeDelay(Gamedata->settings->floattime);  
    Num2Drop=0;
    return;
}

Board::~Board()
{
    for (int y=0; y < 13;++y)
        for (int x=0; x < 6;++x) {
            delete board[x][y];
            board[x][y]=NULL;
        }
    
    vector<Piece*>::iterator iter1;
    for (iter1=ComboPieces.begin();iter1 != ComboPieces.end();++iter1)
        delete *iter1;
    ComboPieces.clear();
    
    ComboCount.clear();
	return;
}

void Board::BoardCheck()
{
    bool drop=false;
    ClearStates();
	for (int y=0; y < 12;y++) 
		for (int x=0;x < 6;x++) {
            if (board[x][y] != NULL) {
              ComboCheck(x,y);
              if (ComboPieces.size() > 3) {
                drop=true;
                }
              RemoveCombo();
			}
		}
	// If there is nothing to drop don't because after the drop it has to search for combos
	//     again via BoardCheck().. An infinite loop occurs if it's always called
	if (drop)  
        DropPieces();  
	ComboCalc();
	return;
}

void Board::ComboCheck(int x, int y)
{
	board[x][y]->visited=true;
	
	if (board[x][y]->color < 1)	// Don't check clear pieces or bottom pieces
		return;

	if (x < 5) {
		if (board[x+1][y] != NULL) {
			if (board[x][y]->color == board[x+1][y]->color) {
				board[x][y]->state[0] = true;	
				if (board[x+1][y]->visited == false)
					ComboCheck(x+1,y);
			}
		}
	}
	if (x > 0) {
		if (board[x-1][y] != NULL) {
			if (board[x][y]->color == board[x-1][y]->color) {
				board[x][y]->state[2] = true;
				if (board[x-1][y]->visited == false)
					ComboCheck(x-1,y);
			}
		}
	}
	if (y < 11) {
		if (board[x][y+1] != NULL) {
			if (board[x][y]->color == board[x][y+1]->color) {
				board[x][y]->state[1] = true;
				if (board[x][y+1]->visited == false)
					ComboCheck(x,y+1);
			}
		}
	}
	if (y > 0) {
		if (board[x][y-1] != NULL) {
			if (board[x][y]->color == board[x][y-1]->color) {
				board[x][y]->state[3] = true;
				if (board[x][y-1]->visited == false)
					ComboCheck(x,y-1);
			}
		}
	}
	// Logic here: If any of the states are true add it into the combo list for possible combos
	bool addp=false;
	for (int i=0;i <4;i++)
		if (board[x][y]->state[i]==true)
			addp=true;

	// Logic here: If the piece is in the check list already don't add it again!
	for (int i=0; i < ComboPieces.size();++i)
		if (board[x][y] == ComboPieces[i]) 
			addp=false;
			
    if (addp) 
        ComboPieces.insert(ComboPieces.end(),board[x][y]);     
	return;
}

void Board::RemoveCombo() 
{
	if (ComboPieces.size() > 3) {
		ComboCount.insert(ComboCount.end(),ComboPieces.size());
		for (int i=0;i < ComboPieces.size();++i) {
			ClearPieceCheck(ComboPieces[i]->x,ComboPieces[i]->y); // Check for any adjacent clear pieces
			board[ComboPieces[i]->x][ComboPieces[i]->y]=NULL;
			delete ComboPieces[i];
		}
	}
	for (int y=0;y < 12;++y)
	   for (int x=0;x < 6;++x)
	       if (board[x][y] != NULL)
	            board[x][y]->visited=false;
	            
    ComboPieces.clear();
	return;
}

void Board::ComboCalc()
{
	// Determine what combo of combos does what :)
    if (Gamedata->settings->classic)
        if (Gamedata->network->Server && ComboCount.size() > 0) {
            int offset=0;
            for (int i=0; i < ComboCount.size(); ++i)
                if (ComboCount[i] > 4)
                    offset=offset + (ComboCount[i]-4);
            int num=(ComboCount.size() - 1) * 6 + offset;
            if (num == 0)
                num=1;
            Gamedata->network->SendClears(Pnum,num);
        }
    else {
    }
	// Then remove the record of the length of each 4+ connection of a color
	ComboCount.clear();
	return;
}

void Board::ClearStates()
{
	for (int y=0; y < 12;y++) 
		for (int x=0;x < 6;x++) 
            if (board[x][y] != NULL)
                for (int i=0; i < 4;i++)
                    board[x][y]->state[i]=false;
	return;
}

void Board::ClearPieceCheck(int x,int y)
{
	if (x > 0)
        if (board[x-1][y] != NULL)
            if (board[x-1][y]->color == 0) {
                delete board[x-1][y];
                board[x-1][y]=NULL;
                }
	if (x < 5)
        if (board[x+1][y] != NULL)
            if (board[x+1][y]->color == 0) {
                delete board[x+1][y];
                board[x+1][y]=NULL;
                }
	if (y > 0)
        if (board[x][y-1] != NULL)
            if (board[x][y-1]->color == 0) {
                delete board[x][y-1];
                board[x][y-1]=NULL;
                }
	if (y < 11)
        if (board[x][y+1] != NULL)
            if (board[x][y+1]->color == 0) {
                delete board[x][y+1];
                board[x][y+1]=NULL;
            }
	return;
}

void Board::FillBoardWClears()
{
    for (int y=0;y < 12;++y)
        for (int x=0; x < 6;++x) {
            if (board[x][y] != NULL)
                board[x][y]->color=0;
            else {
                board[x][y]=new Piece(0,x,y);
                board[x][y]->frame=RandNum(0,Gamedata->images->Frames);
            }
        }
    ClearStates();
    return;
}

void Board::ClearBoard()
{
    for (int y=0;y < 12;++y)
        for (int x=0;x < 6;++x)
            if (board[x][y] != NULL) {
                delete board[x][y];
                board[x][y]=NULL;
            }
    return;
}

void Board::MovePiece()
{
	// Logic here.. If the piece is vertical we check the bottom piece to see if it hits a 
	// piece on the board. If it's horizontal we check if either hit and then drop the other
	// one until it hits another piece or the bottom.
	int drop=-1;
	// When drop == -1 we drop both pieces 1 spot. If it's 0 we drop CurPiece[0], If 1
	// CurPiece[1].. When drop == 3 both pieces have been placed and a new piece is ready

	if (CurPiece[0].y == CurPiece[1].y) { // Horizontal!
		// Checking to see if there is anything under CurPiece[0]
		if (board[CurPiece[0].x][CurPiece[0].y+1] != NULL) {
			// If there is then drop=1 so we drop CurPiece[1] until it hits bottom/another piece.
			drop=1;
			board[CurPiece[0].x][CurPiece[0].y] = new Piece(CurPiece[0]); // Add CurPiece[0] to the board
		}
		// Now we check if anything is under Curpiece[1]
		if (board[CurPiece[1].x][CurPiece[1].y+1] != NULL) {
			if (drop ==1) // If we already put CurPiece[0] in place and now we're putting
				drop = 3;	// CurPiece[1] then both are in place so drop = 3.
			else
				drop=0; // Only happens when CurPiece[0] is not in place.
			board[CurPiece[1].x][CurPiece[1].y] = new Piece(CurPiece[1]); // Add CurPiece[1] to the board
		}
	}
	else // We got a Vertical Piece here!
	{	// Check which Piece is on top then check the bottom piece to see if anything
		// is under it. If there is add to board else drop = -1
		if (CurPiece[0].y > CurPiece[1].y) {
			if (board[CurPiece[0].x][CurPiece[0].y+1] != NULL) {
				// Add them both to the board!
				board[CurPiece[0].x][CurPiece[0].y] = new Piece(CurPiece[0]); // Add CurPiece[0] to the board
				board[CurPiece[1].x][CurPiece[1].y] = new Piece(CurPiece[1]); // Add CurPiece[1] to the board
				drop = 3;
			}
		}
		else // We know that CurPiece[1] is under CurPiece[0] so we check if anything is under [1]
			if (board[CurPiece[1].x][CurPiece[1].y+1] != NULL) {
				// Add them both to the board!
				board[CurPiece[0].x][CurPiece[0].y] = new Piece(CurPiece[0]); // Add CurPiece[0] to the board
				board[CurPiece[1].x][CurPiece[1].y] = new Piece(CurPiece[1]); // Add CurPiece[1] to the board
				drop = 3;
			}
	}
	// Now we check the drop and drop the piece if needed
	if (drop == -1) {
		CurPiece[0].y++;
		CurPiece[1].y++;
	}
	if (drop == 0) { // Drop CurPiece[0] until it hits bottom or another piece
		while (board[CurPiece[0].x][CurPiece[0].y+1] == NULL)
			CurPiece[0].y++;
		board[CurPiece[0].x][CurPiece[0].y] = new Piece(CurPiece[0]); // Add CurPiece[0] to the board
		drop = 3;
	}
	if (drop == 1) { // Drop CurPiece[1] until it hits bottom or another piece
		while (board[CurPiece[1].x][CurPiece[1].y+1] == NULL)
			CurPiece[1].y++;

		board[CurPiece[1].x][CurPiece[1].y] = new Piece(CurPiece[1]); // Add CurPiece[1] to the board
		drop = 3;
	}
	if (drop == 3) {
        BoardCheck();
		if (Gamedata->network->Client || Gamedata->network->Server)
            Gamedata->network->SendPiece(&CurPiece[0],&CurPiece[1],Gamedata->Pnum);
		NextPiece();
	}
	return;
}

void Board::NextPiece()
{
	CurPiece[0]=NxtPiece[0];
	CurPiece[1]=NxtPiece[1];
	NxtPiece[0].color=RandNum(1,6);
	NxtPiece[1].color=RandNum(1,6);
	NxtPiece[0].x=3;
	NxtPiece[0].y=0;
	NxtPiece[1].x=3;
	NxtPiece[1].y=1;
	
	// Randomize the frames
	CurPiece[0].frame=RandNum(0,Gamedata->images->Frames);
	CurPiece[1].frame=RandNum(0,Gamedata->images->Frames);
	NxtPiece[0].frame=RandNum(0,Gamedata->images->Frames);
	NxtPiece[1].frame=RandNum(0,Gamedata->images->Frames);
    return;
}

int RandNum(int start, int finish)
{
	int num;
	num=rand() % finish;
	if (num <= start)
		num=num+start;
	return num;
}

void Board::DropPieces(bool check)
{
// Logic for this loop is.. Start at the bottom and drop each peice as far down as possible
	for (int y=10;y > -1;y--)
		for (int x=0;x < 6;x++) {
			int Y=y;
			while (board[x][Y] != NULL && board[x][Y+1] == NULL) {
                // If FloatTime is active don't drop any pieces except clears
                if (FloatTime.Active() && board[x][Y]->color != 0)
                    break;	
                board[x][Y+1]=board[x][Y]; // Move piece down
                board[x][Y+1]->y++;	// Set piece's y-cord +1
                board[x][Y]=NULL; // Set old spot to NULL 
                Y++;
			}
		}
	if (check)	
        BoardCheck();
	return;
}

void Board::RotateRight()
{
	bool moved=false;
	// If rotating Right against the right wall, the rotation takes place but the bottom
	// piece is pushed one spot to the left (If there is no piece in that spot)
	if (CurPiece[0].x ==5) {
		if (CurPiece[0].x == CurPiece[1].x && CurPiece[0].y == CurPiece[1].y-1) {
			if (board[CurPiece[1].x-1][CurPiece[1].y] == NULL) {
				CurPiece[0].y++;
				CurPiece[1].x--;
				moved=true;
			}
		}
	}
	// Rotate Top to Right
	if (!moved && CurPiece[0].x <5) {
		if (CurPiece[0].y < CurPiece[1].y) {
			if (board[CurPiece[0].x+1][CurPiece[0].y+1] == NULL) {
				CurPiece[0].y=CurPiece[1].y;
				CurPiece[0].x=CurPiece[1].x+1;
				moved=true;
			}
			else 
			if (board[CurPiece[0].x+1][CurPiece[0].y] != NULL) {
				if (CurPiece[0].x>0)
					if (board[CurPiece[1].x-1][CurPiece[1].y] == NULL) {
						CurPiece[0].y++;
						CurPiece[1].x--;
						moved=true;
					}
			}
		}
	}
	// Rotate Right to Bottom
	if (!moved)
		if  (CurPiece[0].x == CurPiece[1].x+1 && CurPiece[0].y == CurPiece[1].y) 
			if (board[CurPiece[0].x-1][CurPiece[0].y+1] == NULL) {
				CurPiece[0].x--;
				CurPiece[0].y++;
				moved=true;		
	}
	// Rotate Bottom to Left
	if (!moved && CurPiece[0].x > 0)
		if (CurPiece[0].y > CurPiece[1].y)
			if (board[CurPiece[0].x-1][CurPiece[0].y-1] == NULL) {
			CurPiece[0].y=CurPiece[1].y;
			CurPiece[0].x=CurPiece[1].x-1;
			moved=true;
	}
	// Rotate Left to Top
	if (!moved)
		if  (CurPiece[0].x == CurPiece[1].x-1 && CurPiece[0].y == CurPiece[1].y) 
			if (board[CurPiece[0].x+1][CurPiece[0].y-1] == NULL) {
			CurPiece[0].x++;
			CurPiece[0].y--;
			moved=true;		
	}
	return;
}

void Board::RotateLeft()
{
	bool moved=false;
	// If rotating left against the right wall, the rotation takes place but the bottom
	// piece is pushed one spot to the left (If there is no piece in that spot)
	if (CurPiece[0].x == 0) {
		if (CurPiece[0].x == CurPiece[1].x && CurPiece[0].y == CurPiece[1].y-1) {
			if (board[CurPiece[1].x+1][CurPiece[1].y] == NULL) {
				CurPiece[0].y++;
				CurPiece[1].x++;
				moved=true;
			}
		}
	}
	// Rotate Top to Left
	if (!moved && CurPiece[0].x > 0) {
		if (CurPiece[0].y < CurPiece[1].y) { 
			if (board[CurPiece[0].x-1][CurPiece[0].y+1] == NULL) {
				CurPiece[0].y=CurPiece[1].y;
				CurPiece[0].x--;
				moved=true;
			}
			else 
			if (board[CurPiece[0].x-1][CurPiece[0].y] != NULL) {
				if (CurPiece[0].x <5) {
					if (board[CurPiece[1].x+1][CurPiece[1].y] == NULL) {
						CurPiece[0].y++;
						CurPiece[1].x++;
						moved=true;
					}
				}
			}
		
		}
	}
	// Rotate Left to bottom
	if (!moved)
		if  (CurPiece[0].x == CurPiece[1].x-1 && CurPiece[0].y == CurPiece[1].y)
			if (board[CurPiece[0].x+1][CurPiece[0].y+1] == NULL) {
				CurPiece[0].x=CurPiece[1].x;
				CurPiece[0].y++;
				moved=true;		
	}
	// Rotate Bottom to Right
	if (!moved && CurPiece[0].x < 5)
		if (CurPiece[0].y > CurPiece[1].y) 
			if (board[CurPiece[0].x+1][CurPiece[0].y-1]== NULL) {
				CurPiece[0].y=CurPiece[1].y;
				CurPiece[0].x++;
				moved=true;
	}
	// Rotate Right to Top
	if (!moved)
		if  (CurPiece[0].x == CurPiece[1].x+1 && CurPiece[0].y == CurPiece[1].y) 
			if (board[CurPiece[0].x-1][CurPiece[0].y-1] == NULL) {
				CurPiece[0].x=CurPiece[1].x;
				CurPiece[0].y--;
				moved=true;		
	}
	return;
}

void Board::MoveRight()
{
	// Horizontal Check
	if ((CurPiece[0].x < 5 || CurPiece[1].x < 5) && CurPiece[0].y != CurPiece[1].y) {
		if (board[CurPiece[0].x+1][CurPiece[0].y] == NULL && board[CurPiece[1].x+1][CurPiece[1].y] == NULL) {
			CurPiece[0].x++;
			CurPiece[1].x++;
		}
	}
	// Verticle Check
	if ((CurPiece[0].x < 5 && CurPiece[1].x < 5) && CurPiece[0].y == CurPiece[1].y) {
		if (board[CurPiece[0].x+1][CurPiece[0].y] == NULL && board[CurPiece[1].x+1][CurPiece[1].y] == NULL) {
			CurPiece[0].x++;
			CurPiece[1].x++;
		}
	}
	return;
}

void Board::MoveLeft()
{
	// Horizontal Check
	if ((CurPiece[0].x > 0 || CurPiece[1].x > 0) && CurPiece[0].y != CurPiece[1].y) {
		if (board[CurPiece[0].x-1][CurPiece[0].y] == NULL && board[CurPiece[1].x-1][CurPiece[1].y]== NULL) {
			CurPiece[0].x--;
			CurPiece[1].x--;
		}
	}
	// Verticle Check
	if ((CurPiece[0].x > 0 && CurPiece[1].x > 0) && CurPiece[0].y == CurPiece[1].y) {
		if (board[CurPiece[0].x-1][CurPiece[0].y] == NULL && board[CurPiece[1].x-1][CurPiece[1].y] == NULL) {
			CurPiece[0].x--;
			CurPiece[1].x--;
		}
	}
	return;
}


// *****************************************************************************************
// Specials related functions
// *****************************************************************************************
void Board::ClearSpecials()
{
    for (int i=0; i < 10;++i)
        specials[i]=0;
}

void Board::UseSpecial(int special,int to, int from,int randnum)
{
   if (special == 0)
        return;
        
    switch (special) {
        case 1:
            Gamedata->boards[to]->DropClears(1,randnum);
            break;
        case 2:
            Gamedata->boards[to]->ClearLine();
            break;
        case 3:
            Gamedata->boards[to]->DropClears(3,randnum);
            break;
        case 4:
            Gamedata->boards[to]->NukeBoard();
            break;
        case 5:
            Gamedata->boards[to]->DropClears(6,randnum);
            break;
        case 6:
            Gamedata->boards[to]->SwitchBoards(from);
            break;
        case 7:
            Gamedata->boards[to]->FloatingPieces();
            break;
    }
    // Okay, we used a special so let's remove it. We do this by shifting all specials
    // over by one and then assigning the 10th special to 0 (or NONE)
    if (Gamedata->Pnum == from) {
        for (int i=0; i < 9;++i) 
            Gamedata->boards[from]->specials[i]=specials[i+1];
        Gamedata->boards[from]->specials[9]=0;
    }
    return;
}

void Board::AddSpecial(int special)
{
    for (int i=9; i > 0;--i)
        specials[i]=specials[i-1];
        
    specials[0]=special;
    return;
}

void Board::DropClears(int num2drop, int randnum)
{
    Gamedata->UpdateNicks=true;
	Num2Drop=Num2Drop+num2drop;
	static int Randnum;
        if (randnum != 0)
            Randnum=randnum;
            
	if (ClearDropRate.Active() == false) {
		ClearDropRate.Activate();
		return;
	}
	if (!ClearDropRate.Ready())
		return;
	
	int i=0; // Our friendly iterator.
	int fullrows=int(Num2Drop/6); // Number of vertile rows of 6 to drop
	int leftovers=Num2Drop % 6; // Number of Clears on the top row.
	
	// We're checking to make sure the current board can support the number of clear blocks coming
	// If it's the amount of freespace is <= the number of clears then simply fill the board and end it
	int freespaces=0;
	for (int y=0;y<12;++y)
		for (int x=0;x < 6;++x)
			if (board[x][y] == NULL)
				freespaces++;
	if (freespaces <= Num2Drop) {
		FillBoardWClears();
		Num2Drop=0;
		ClearDropRate.Deactivate();
		return;
	}
	
	// Drop the full rows
	for (int i=0; i < fullrows;++i) {
		for (int x=0; x < 6;++x) {
			Piece temp(0,x,0);
			board[x][0]=new Piece(temp);
		}
		DropPieces(false);
	}
	// Now drop the remaining clears
	for (int i=0; i < leftovers;++i) {
		Piece temp(0,Randnum,0);
		board[Randnum][0]=new Piece(temp);
		Randnum++;
		if (Randnum > 5)
			Randnum=0;
	}
	DropPieces(false);
	Num2Drop=0;
	ClearDropRate.Deactivate();
	return;
}

void Board::ClearLine()
{
    // Erase the bottom line of pieces
    for (int i=0; i < 6;++i)
        if (board[i][11] != NULL) {
            delete board[i][11];
            board[i][11]=NULL;
    }
    DropPieces();
    return;
}

void Board::NukeBoard()
{
    for (int x=0; x < 6;++x)
        for (int y=0; y < 12;++y)
            if (board[x][y] != NULL) {
                delete board[x][y];
                board[x][y] = NULL;
            }
    return;
}

void Board::SwitchBoards(int from)
{
    for (int x=0; x < 6;++x)
        for (int y=0; y < 12;++y) {
            Piece* temp=board[x][y];
            board[x][y]=Gamedata->boards[from]->board[x][y];
            Gamedata->boards[from]->board[x][y]=temp;
        }
    return;
}

void Board::FloatingPieces()
{
    // If FloatTime isn't active well activate it.
    if (!FloatTime.Active())
        FloatTime.Activate();
    else { // If it is active we add the remaining time to the floattime 
        FloatTime.ChangeDelay(FloatTime.TimeLeft() + Gamedata->settings->floattime);
        FloatTime.Deactivate();
        FloatTime.Activate();
    }    
}
