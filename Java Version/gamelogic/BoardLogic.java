package com.bestgroup.downpour.gamelogic;

import java.util.Random;
import java.util.Vector;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;
import com.bestgroup.downpour.gamecontroller.gameevents.SendBoard;
import com.bestgroup.downpour.gamecontroller.gameevents.SendClears;
import com.bestgroup.downpour.gamecontroller.gameevents.SendPiece;
import com.bestgroup.downpour.gamelogic.Piece.pieceType;

/**
 * 
 * @author Eric
 *	ServerBoard contains all the game logic the server will need to execute when a player informs them where they position a piece
 *
 */
public class BoardLogic {

	public Piece board[][];
    public Piece curPiece[];
    public Piece nextPiece[];
    
    Board otherBoard;
    GameController gameController;
    Player player;
    
    /** 
     * Hack to allow BoardLogic to manipulate Board without extending...
     * This is because of typecasting and serialization problems
     * TODO: Find a better way.. Maybe rewrite entire thing to just manipulate a Board when 
     * board[][] curPiece[] and nextPiece[] are not public..
     * @param b
     */
    public BoardLogic(Player p, GameController gc) {
    	player = p;
    	
    	otherBoard=player.getBoard();
    	board=otherBoard.board;
    	
    	curPiece=otherBoard.curPiece;
    	nextPiece=otherBoard.nextPiece;
    	gameController=gc;
    }
                           
	/** A list of all Pieces in a current combination being calculated */
	Vector<Piece> comboPieces = new Vector<Piece>(); 
	/** 
	 * Each element in this list is the integer value specifying the number of Pieces connected to each other. 
	 * We use this list to calculate how many Clears to send to other players
	 */
    Vector<Integer> comboCount = new Vector<Integer>(); 

    
    /**
     * Pick a players Current and Next Piece elements
     */
    public void nextPiece(){
		//TODO: Make nextPice actually random
		curPiece[0]=nextPiece[0];
		curPiece[1]=nextPiece[1];
		nextPiece[0] = new Piece(otherBoard.randomPiece(),3,0);
		nextPiece[1] = new Piece(otherBoard.randomPiece(),3,1); 
	}
		
    /**
     * Establish what Pieces are connected and update their states accordingly.
     * 
     * This function traverses a Piece board[][] and finds the connected pieces, calculates combinations and destroys any
     * combinations > 4 connections long and finally drops any pieces into new positions.
     */
	public void boardCheck() {
	    boolean drop=false;
	    clearStates();	// Reset all states during a check
	    
	    /* Traverse the game board and check every piece for Combinations */
		for (int y=0; y < 12;y++) 
			for (int x=0;x < 6;x++) {
	            if (board[x][y].color != pieceType.EMPTY) {
	              comboCheck(x,y); 
	              if (comboPieces.size() > 3) {
	                drop=true;
	                }
	              removeCombo();
				}
			}
		// If there is nothing to drop don't because after the drop it has to search for combos
		//     again via boardCheck().. An infinite loop occurs if it's always called
		if (drop)  
	        dropPieces(true);  
		comboCalc();
	}
	
	/**
	 * Check for combinations
	 * 
	 * Recursively check Piece starting at x,y to see if it has any adjacent pieces of the same color
	 * @param x X-coordinate of Pieces to check
	 * @param y y-coordinate of Piece to check
	 */
	private void comboCheck(int x, int y) {
		board[x][y].visited=true;
		
		if (board[x][y].color == pieceType.CLEAR)	// Don't check CLEAR pieces
			return;

		if (x < 5) {	// Don't check further RIGHT if we are on the RIGHT edge of the board
				if (board[x][y].color == board[x+1][y].color) {
					board[x][y].state[0] = true;	
					if (board[x+1][y].visited == false)
						comboCheck(x+1,y);
				}
		}
		
		if (x > 0) {	// Don't check further LEFT if we are at the LEFT edge of the board
				if (board[x][y].color == board[x-1][y].color) {
					board[x][y].state[2] = true;
					if (board[x-1][y].visited == false)
						comboCheck(x-1,y);
				}
		}
		if (y < 11) {	// Don't check further DOWN if we are at the BOTTOM edge of the board
				if (board[x][y].color == board[x][y+1].color) {
					board[x][y].state[1] = true;
					if (board[x][y+1].visited == false)
						comboCheck(x,y+1);		
				}
		}
		if (y > 0) {	// Don't check further UP if we are at the TOP edge of the board
				if (board[x][y].color == board[x][y-1].color) {
					board[x][y].state[3] = true;
					if (board[x][y-1].visited == false)
						comboCheck(x,y-1);
				}	
		}
		
		// If any of the states are true add it into the combo list for possible combos
		boolean addp=false;
		for (int i=0;i <4;i++)
			if (board[x][y].state[i]==true) 
				addp=true;

		// If the piece is in the check list already don't add it again!
		for (int i=0; i < comboPieces.size();++i)
			if (comboPieces.elementAt(i).isEqual(board[x][y])) 
				addp=false;
		
	    if (addp) 
	        comboPieces.add(board[x][y]);     
	}
	
	/**
	 * Reset the connection states for ALL Pieces on the game board
	 */
	private void clearStates() {
		for (int y=0; y < 12;y++) 
			for (int x=0;x < 6;x++) 
	                for (int i=0; i < 4;i++)
	                    board[x][y].state[i]=false;
	}
	
	/**
	 *	Check to see if any Pieces are floating in the air. If so drop them down until they rest on another Piece 
	 * @param check
		TODO: Make this private again once the server is actually setup and functioning
	*/
	void dropPieces(boolean check) {
		// Logic for this loop is.. Start at the bottom and drop each peice as far down as possible
		for (int y=10;y > -1;y--)
			for (int x=0;x < 6;x++) {
				int Y=y;
				while (board[x][Y].color != pieceType.EMPTY && board[x][Y+1].color == pieceType.EMPTY) {
					/* If/when we implement specials like antigravity status this is a check
					 * that when a player has that status don't drop pieces 
					// If FloatTime is active don't drop any pieces except clears
	                if (AntiGravity.Active() && board[x][Y]->color != pieceType.CLEAR)
	                    break;	
	                */
	                board[x][Y+1].color=board[x][Y].color; // Move piece down
	                board[x][Y].color=pieceType.EMPTY; // Set old spot to EMPTY 
	                Y++;
				}
			}
		if (check)	
	        boardCheck();		
	}
 
	/**
	 * Drop clear blocks on a board
	 * @param num Number of Clear blocks to drop on a board
	 */
	public void dropClears(int num) {
		if (num == 0)
			return;
	}
	
	/**
	 * Based on the number of 4+ Pieces connections in succession during a single Piece drop we calculate 
	 * how many CLEAR pieces to give to all other players
	 */
	private void comboCalc() {
		// Determine what combo of combos does what :)    
		if (comboCount.size() > 0) {
			int offset=0;
			for (int i=0; i < comboCount.size(); ++i)
				if (comboCount.elementAt(i) > 4)
					offset=offset + (comboCount.elementAt(i) - 4);
			int num=(comboCount.size() - 1) * 6 + offset;
			if (num == 0)
				num=1;

	            // TODO: Inform the clients of their attack
				GameEvent event = new SendClears(player.getUserID(), num);
				gameController.push(event);
		}
	    else {
	    }
		// Then remove the record of the length of each 4+ connection of a color
		comboCount.clear();
	}
	
	/**
	 * Remove all 4+ Piece connections from the game board
	 */
	private void removeCombo() {
		if (comboPieces.size() > 3) {
			comboCount.add(comboPieces.size());
			for (int i=0;i < comboPieces.size();++i) {
				clearPieceCheck(comboPieces.elementAt(i).x,comboPieces.elementAt(i).y); // Check for any adjacent clear pieces
				board[comboPieces.elementAt(i).x][comboPieces.elementAt(i).y].color=pieceType.EMPTY;
			}
		}
		for (int y=0;y < 12;++y)
		   for (int x=0;x < 6;++x)
		       if (board[x][y].color != pieceType.EMPTY)
		            board[x][y].visited=false;
		            
	    comboPieces.clear();
		return;		
	}
	
	/**
	 * Remove a CLEAR Piece if it's adjacent to a 4+ Piece connection that just exploded
	 * @param x	X-Cord to check for adjacent CLEAR pieces
	 * @param y Y-Cord to check for adjacent CLEAR pieces
	 */
	private void clearPieceCheck(int x, int y) {
		if (x > 0)
			if (board[x-1][y].color == pieceType.CLEAR) {    
				board[x-1][y].color=pieceType.EMPTY;
			}
		if (x < 5)
			if (board[x+1][y].color == pieceType.CLEAR) {
				board[x+1][y].color=pieceType.EMPTY;
			}
		if (y > 0)
			if (board[x][y-1].color == pieceType.CLEAR) {    
				board[x][y-1].color=pieceType.EMPTY;
			}
		if (y < 11)
			if (board[x][y+1].color == pieceType.CLEAR) {    
				board[x][y+1].color=pieceType.EMPTY;
			}
	}

	public void dropClears() {
		// We're checking to make sure the current board can support the number of clear blocks coming
		// If it's the amount of freespace is <= the number of clears then simply fill the board and end it
		int freespaces=0;
		for (int y=0;y<12;++y)
			for (int x=0;x < 6;++x)
				if (board[x][y].getColor() == pieceType.EMPTY)
						freespaces++;
		
		if (freespaces <= otherBoard.numberOfIncomingClears) {
			otherBoard = Board.createFullBoard();
			return;
		}	
			
		int i=0; // Our friendly iterator.
		int fullrows=(int) (otherBoard.numberOfIncomingClears/6); // Number of vertical rows of 6 to drop
		int leftovers=otherBoard.numberOfIncomingClears % 6; // Number of Clears on the top row.

		// Drop the full rows
		for (i=0; i < fullrows;++i) {
			for (int x=0; x < 6;++x) {
				board[x][0].color = pieceType.CLEAR;
			}
			dropPieces(false);
		}
		
		Random randNum = new Random();
		int r = randNum.nextInt(5);
		
		// Now drop the remaining clears
		for (i=0; i < leftovers;++i) {
			board[r][0].color = pieceType.CLEAR;
			r++;
			if (r > 5)
				r=0;
		}
		
		dropPieces(false);		
		otherBoard.numberOfIncomingClears = 0;	
	}

	
	
	
	
	
	
	public Boolean movePiece() {
		// Logic here.. If the piece is vertical we check the bottom piece to see if it hits a 
		// piece on the board. If it's horizontal we check if either hit and then drop the other
		// one until it hits another piece or the bottom.
		int drop=-1;
		// When drop == -1 we drop both pieces 1 spot. If it's 0 we drop curPiece[0], If 1
		// curPiece[1].. When drop == 3 both pieces have been placed and a new piece is ready
		Boolean sendPieceToServer = false;
		
		if (curPiece[0].y == curPiece[1].y) { // Horizontal!
			// Checking to see if there is anything under curPiece[0]
			if (board[curPiece[0].x][curPiece[0].y+1].color != pieceType.EMPTY) {
				// If there is then drop=1 so we drop curPiece[1] until it hits bottom/another piece.
				drop=1;
				board[curPiece[0].x][curPiece[0].y].color = curPiece[0].color; // Add curPiece[0] to the board
			}
			// Now we check if anything is under Curpiece[1]
			if (this.board[curPiece[1].x][curPiece[1].y+1].color != pieceType.EMPTY) {
				if (drop ==1) // If we already put curPiece[0] in place and now we're putting
					drop = 3;	// curPiece[1] then both are in place so drop = 3.
				else
					drop=0; // Only happens when curPiece[0] is not in place.
				board[curPiece[1].x][curPiece[1].y].color = curPiece[1].color; // Add curPiece[1] to the board
			}
		}
		else // We got a Vertical Piece here!
		{	// Check which Piece is on top then check the bottom piece to see if anything
			// is under it. If there is add to board else drop = -1
			if (curPiece[0].y > curPiece[1].y) {
				if (board[curPiece[0].x][curPiece[0].y+1].color != pieceType.EMPTY) {
					// Add them both to the board!
					board[curPiece[0].x][curPiece[0].y].color = curPiece[0].color; // Add curPiece[0] to the board
					board[curPiece[1].x][curPiece[1].y].color = curPiece[1].color; // Add curPiece[1] to the board
					drop = 3;
				}
			}
			else // We know that curPiece[1] is under curPiece[0] so we check if anything is under [1]
				if (board[curPiece[1].x][curPiece[1].y+1].color != pieceType.EMPTY) {
					// Add them both to the board!
					board[curPiece[0].x][curPiece[0].y].color = curPiece[0].color; // Add curPiece[0] to the board
					board[curPiece[1].x][curPiece[1].y].color = curPiece[1].color; // Add curPiece[1] to the board
					drop = 3;
				}
		}
		// Now we check the drop and drop the piece if needed
		if (drop == -1) {
			curPiece[0].y++;
			curPiece[1].y++;
		}
		if (drop == 0) { // Drop curPiece[0] until it hits bottom or another piece
			while (board[curPiece[0].x][curPiece[0].y+1].color == pieceType.EMPTY)
				curPiece[0].y++;
			board[curPiece[0].x][curPiece[0].y].color = curPiece[0].color; // Add curPiece[0] to the board
			drop = 3;
		}
		if (drop == 1) { // Drop curPiece[1] until it hits bottom or another piece
			while (board[curPiece[1].x][curPiece[1].y+1].color == pieceType.EMPTY)
				curPiece[1].y++;

			board[curPiece[1].x][curPiece[1].y].color = curPiece[1].color; // Add curPiece[1] to the board
			drop = 3;
		}
		if (drop == 3) {
			/* TODO: Disable boardCheck on client side.. MAYBE... Doesn't really matter 
			 * but when future animations are put into place it will most likely need to be disabled
			 * OR split BoardLogic into Client/Server classes and overload for animations..?
			*/
			if (gameController instanceof GameControllerClient) {
				sendPieceToServer = true;
				
				/* TODO: Figure out a way to advance the piece w/o picking a new random one. So when the server
				 * sends the new board back it doesn't switch again and look strange to the user... 
				*/
//				NextPiece();
			}
		}
		return sendPieceToServer;
	}

	public void rotateRight() {
		Boolean moved=false;
		// If rotating Right against the right wall, the rotation takes place but the bottom
		// piece is pushed one spot to the left (If there is no piece in that spot)
		if (curPiece[0].x == 5) {
			if (curPiece[0].x == curPiece[1].x && curPiece[0].y == curPiece[1].y-1) {
				if (board[curPiece[1].x-1][curPiece[1].y].color == pieceType.EMPTY) {
					curPiece[0].y++;
					curPiece[1].x--;
					moved=true;
				}
			}
		}
		// Rotate Top to Right
		if (!moved && curPiece[0].x < 5) {
			if (curPiece[0].y < curPiece[1].y) {
				if (board[curPiece[0].x+1][curPiece[0].y+1].color == pieceType.EMPTY) {
					curPiece[0].y=curPiece[1].y;
					curPiece[0].x=curPiece[1].x+1;
					moved=true;
				}
				else 
				if (board[curPiece[0].x+1][curPiece[0].y].color != pieceType.EMPTY) {
					if (curPiece[0].x > 0)
						if (board[curPiece[1].x-1][curPiece[1].y].color == pieceType.EMPTY) {
							curPiece[0].y++;
							curPiece[1].x--;
							moved=true;
						}
				}
			}
		}
		// Rotate Right to Bottom
		if (!moved)
			if  (curPiece[0].x == curPiece[1].x+1 && curPiece[0].y == curPiece[1].y) 
				if (board[curPiece[0].x-1][curPiece[0].y+1].color == pieceType.EMPTY) {
					curPiece[0].x--;
					curPiece[0].y++;
					moved=true;		
		}
		// Rotate Bottom to Left
		if (!moved && curPiece[0].x > 0)
			if (curPiece[0].y > curPiece[1].y)
				if (board[curPiece[0].x-1][curPiece[0].y-1].color == pieceType.EMPTY) {
				curPiece[0].y=curPiece[1].y;
				curPiece[0].x=curPiece[1].x-1;
				moved=true;
		}
		// Rotate Left to Top
		if (!moved)
			if  (curPiece[0].x == curPiece[1].x-1 && curPiece[0].y == curPiece[1].y) 
				if (board[curPiece[0].x+1][curPiece[0].y-1].color == pieceType.EMPTY) {
				curPiece[0].x++;
				curPiece[0].y--;
				moved=true;		
		}
		return;
	}

	public void rotateLeft() {
		Boolean moved=false;
		// If rotating left against the right wall, the rotation takes place but the bottom
		// piece is pushed one spot to the left (If there is no piece in that spot)
		if (curPiece[0].x == 0) {
			if (curPiece[0].x == curPiece[1].x && curPiece[0].y == curPiece[1].y-1) {
				if (board[curPiece[1].x+1][curPiece[1].y].color == pieceType.EMPTY) {
					curPiece[0].y++;
					curPiece[1].x++;
					moved=true;
				}
			}
		}
		// Rotate Top to Left
		if (!moved && curPiece[0].x > 0) {
			if (curPiece[0].y < curPiece[1].y) { 
				if (board[curPiece[0].x-1][curPiece[0].y+1].color == pieceType.EMPTY) {
					curPiece[0].y=curPiece[1].y;
					curPiece[0].x--;
					moved=true;
				}
				else 
				if (board[curPiece[0].x-1][curPiece[0].y].color != pieceType.EMPTY) {
					if (curPiece[0].x <5) {
						if (board[curPiece[1].x+1][curPiece[1].y].color == pieceType.EMPTY) {
							curPiece[0].y++;
							curPiece[1].x++;
							moved=true;
						}
					}
				}
			
			}
		}
		// Rotate Left to bottom
		if (!moved)
			if  (curPiece[0].x == curPiece[1].x-1 && curPiece[0].y == curPiece[1].y)
				if (board[curPiece[0].x+1][curPiece[0].y+1].color == pieceType.EMPTY) {
					curPiece[0].x=curPiece[1].x;
					curPiece[0].y++;
					moved=true;		
		}
		// Rotate Bottom to Right
		if (!moved && curPiece[0].x < 5)
			if (curPiece[0].y > curPiece[1].y) 
				if (board[curPiece[0].x+1][curPiece[0].y-1].color == pieceType.EMPTY) {
					curPiece[0].y=curPiece[1].y;
					curPiece[0].x++;
					moved=true;
		}
		// Rotate Right to Top
		if (!moved)
			if  (curPiece[0].x == curPiece[1].x+1 && curPiece[0].y == curPiece[1].y) 
				if (board[curPiece[0].x-1][curPiece[0].y-1].color == pieceType.EMPTY) {
					curPiece[0].x=curPiece[1].x;
					curPiece[0].y--;
					moved=true;		
		}
		return;
	}

	public void moveRight() {
		// Horizontal Check
		if ((curPiece[0].x < 5 || curPiece[1].x < 5) && curPiece[0].y != curPiece[1].y) {
			if (board[curPiece[0].x+1][curPiece[0].y].color == pieceType.EMPTY && board[curPiece[1].x+1][curPiece[1].y].color == pieceType.EMPTY) {
				curPiece[0].x++;
				curPiece[1].x++;
			}
		}
		// Verticle Check
		if ((curPiece[0].x < 5 && curPiece[1].x < 5) && curPiece[0].y == curPiece[1].y) {
			if (board[curPiece[0].x+1][curPiece[0].y].color == pieceType.EMPTY && board[curPiece[1].x+1][curPiece[1].y].color == pieceType.EMPTY) {
				curPiece[0].x++;
				curPiece[1].x++;
			}
		}
		return;
	}

	public void moveLeft() {
		// Horizontal Check
		if ((curPiece[0].x > 0 || curPiece[1].x > 0) && curPiece[0].y != curPiece[1].y) {
			if (board[curPiece[0].x-1][curPiece[0].y].color == pieceType.EMPTY && board[curPiece[1].x-1][curPiece[1].y].color == pieceType.EMPTY) {
				curPiece[0].x--;
				curPiece[1].x--;
			}
		}
		// Verticle Check
		if ((curPiece[0].x > 0 && curPiece[1].x > 0) && curPiece[0].y == curPiece[1].y) {
			if (board[curPiece[0].x-1][curPiece[0].y].color == pieceType.EMPTY && board[curPiece[1].x-1][curPiece[1].y].color == pieceType.EMPTY) {
				curPiece[0].x--;
				curPiece[1].x--;
			}
		}
		return;
	}
	

	// TODO: This function will be removed once server code is in place. This info will be obtained from the server
/*	
	private void NextPiece() {
		curPiece[0].color=nextPiece[0].color;
		curPiece[1].color=nextPiece[1].color;
		
		curPiece[0].x=3;	curPiece[0].y=0; 
		curPiece[1].x=3;	curPiece[1].y=1; 

		nextPiece[0].color=otherBoard.randomPiece();
		nextPiece[1].color=otherBoard.randomPiece();
		nextPiece[0].x=3;
		nextPiece[0].y=0;
		nextPiece[1].x=3;
		nextPiece[1].y=1;
		
		// Randomize the frames
		curPiece[0].randomizeFrame();
		curPiece[1].randomizeFrame();
		nextPiece[0].randomizeFrame();
		nextPiece[1].randomizeFrame();
	    return;
	}
*/	
}
