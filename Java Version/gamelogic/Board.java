package com.bestgroup.downpour.gamelogic;

import java.io.Serializable;
import java.util.Random;
import com.bestgroup.downpour.gamelogic.Piece.pieceType;


/**
 * 
 * @author Eric
 *	The Board object is the basic Gameboard for every player. it consists of a 2D array of Pieces along with their
 *	current and next pieces obtained from the server at random.
 */
public class Board  implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 3385803137741292259L;

	static Random rand = new Random();
	
	public Piece board[][] = new Piece[6][13];
    public Piece curPiece[] = new Piece[2];
    public Piece nextPiece[] = new Piece[2];
 
    public int numberOfIncomingClears=3;
    
    protected Board() {
    	//Initialize board pieces to empty space
		for(int y = 0; y < 13; y++)
			for(int x = 0; x < 6; x++)
				board[x][y] = new Piece(pieceType.EMPTY, x, y);  
		
		//Set bottom boundary so pieces don't fall off the Board
		for(int i = 0; i <6; i++)
			board[i][12] = new Piece(pieceType.CLEAR, i, 12);		
		
		curPiece[0]=new Piece(randomPiece(), 3, 0);
		curPiece[1]=new Piece(randomPiece(), 3, 1);
		nextPiece[0]=new Piece(randomPiece(), 3, 0);
		nextPiece[1]=new Piece(randomPiece(), 3, 1);
		
		numberOfIncomingClears = 0;
    }
    
    public Piece getPiece(int x, int y) {
    	return board[x][y];
    }
    
    public void resetBoard() {
    	for (int x=0; x < 6;x++)
    		for (int y=0; y < 12; y++)
    			board[x][y].color=pieceType.EMPTY;
    	
    	numberOfIncomingClears = 0;
    }   	    

    public void FillBoardWithCLears() {
    	for (int x=0; x < 6;x++)
    		for (int y=0; y < 12; y++)
    			board[x][y].color=pieceType.CLEAR;    	
    }   	    

    /** 
     * Board Factory.. Create a new board filled with pieceType.EMPTY
     * @return
     */
    public static Board createEmptyBoard() {
    	Board b = new Board();
    	b.resetBoard();
    	
    	return b;
    }
    
    /**
     * Board Factor.. Create a new board filled with pieceType.CLEAR
     * @return
     */
    public static Board createFullBoard() {
    	Board b = new Board();
    	b.FillBoardWithCLears();
    	
    	return b;
    }
    
    public void randomBoard() {
		for (int x=0; x < 6; x++)
			for (int y=0; y < 12; y++) {
				board[x][y] = new Piece(randomPiece(), x, y);
				board[x][y].randomizeFrame();
			}
    }
    
    // TODO: This could be done better..
    // TODO: Why protected??
    public pieceType randomPiece() {
    		pieceType[] p = pieceType.values();
    		pieceType t = p[rand.nextInt(p.length)];
   		
    		while (t == pieceType.EMPTY || t == pieceType.CLEAR) 
    			t =  p[rand.nextInt(p.length)];
    		
            return t;
    }
    	
    public void addClears(int num) {
    	numberOfIncomingClears = numberOfIncomingClears + num;
    }
}