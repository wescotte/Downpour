package com.bestgroup.downpour.gamelogic;

import java.io.Serializable;
import java.util.Random;


/**
 *  @author Eric Wescott
 *	Piece class
 *	
 *	Each gameboard is made up of 2D array of these elements
 */
public class Piece  implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = -4696594812321380728L;

	/*
	 * Valid Piece colors
	RED 	made from RED=X, GREEN=0, BLUE=0
	GREEN 	made from RED=0, GREEN=X, BLUE=0
	BLUE 	made from RED=0, GREEN=0, BLUE=X
	YELLOW 	made from RED=X, GREEN=X, BLUE=0
	PURPLE 	made from RED=X, GREEN=0, BLUE=X
	LT BLUE made from RED=0, GREEN=X, BLUE=X
	 */
	public enum pieceType {
		EMPTY, CLEAR, RED, GREEN, BLUE, YELLOW, PURPLE, LT_BLUE;
	}
	
	/*
    public enum pieceType implements Serializable {
    	EMPTY("EMPTY"), CLEAR("CLEAR"), 
    	RED("RED"), GREEN("GREEN"), BLUE("BLUE"), 
    	YELLOW("YELLOW"), PURPLE("PURPLE"), LT_BLUE("LT_BLUE");
    	    	  
    	pieceType(final String newType)  {   
    	}  
    }
    */
    /**
     * Determiens how the piece is connected to others. This is really only used to display the correct Piece image
     * [0] = LEFT
     * [1] = DOWN
     * [2] = RIGHT
     * [3] = UP
     */
	public boolean state[] = new boolean[4];
	
	/**
	 * Current frame to display for this Peice 
	 */
	public int frame;
	
	/**
	 * Has this Piece been visited during a boardCheck?
	 * 
	 * This is used calculate combos in a recursive manor. Once a Piece is visited we want to ensure we don't check it again
	 * or else we will get stuck in an infinite loop.
	 */
    public boolean visited;
    
    /** 
     * Actual color of the Piece
     * Also be an EMPTY space or a CLEAR piece
     */
	public pieceType color; 
	
	/** 
	 * X,Y Coordinates of the Piece.
	 * Sort of reduntant because it's stored in an 2D array but having this data in the Piece object
	 * makes things easier because we can just pass a reference to a Piece and get everything we need
	 */
	public int x, y;
    
	/**
	 * Create a new Piece object
	 * @param Color Color of Piece (also EMPTY and CLEAR are valid)
	 * @param X X-Coordinate
	 * @param Y Y-Coordinate
	 */
	public Piece(pieceType Color, int X, int Y)
	{
	    color=Color;
	    x=X;
	    y=Y;
	    
	    // Set initial states to unconnected values
	    for (int i=0; i < 4;++i)
	        state[i]=false;
	    
        visited = false; // We have not visited this Piece to check for connections yet
       
        // Set a random frame value so we don't have all the pieces doing the exact same animation at the same time
        randomizeFrame();
	}
	
	/** Copy constructor 
	 * 
	 * @param p
	 */
	public Piece(Piece p) {
		color=p.color;
		x=p.x;
		y=p.y;
		
	    for (int i=0; i < 4;++i)
	        state[i]=false;
	    
        visited = false; // We have not visited this Piece to check for connections yet
       
        // Set a random frame value so we don't have all the pieces doing the exact same animation at the same time
        randomizeFrame();		
	}
	
	public pieceType getColor() {
		return color;
	}
	
	public boolean[] getState() {
		return state;
	}
	
	public boolean getState(int i) {
		return state[i];
	}
	
	public int getX() {
		return x;
	}
	
	public int getY() {
		return y;
	}
	
	public boolean isVisited() {
		return visited;
	}
	
	public int getFrame() {
		return frame;
	}
	
	/**
	 * Advance Piece to the next animated frame to display
	 * @param maxFrame The total number of frames in the animation
	 */
	public void advanceFrame(int maxFrame) {
		frame++;
		if (frame >= maxFrame)
			frame=0;
	}
	
	/**
	 * Compare two pieces to see if they are the same
	 * 
	 * We check to see the X, Y and Color values are the same. If so then they are in fact the same Piece
	 * @param p Piece to compare with
	 * @return
	 */
	public boolean isEqual(Piece p) {
		if (this.color != p.color)
			return false;
		
		if (this.x != p.x)
			return false;
		
		if (this.y != p.y)
			return false;
		
		return true;
	}
	
	/**
	 * Set a random frame
	 * 
	 * This is done to ensure the Pieces are not all displaying the exact same animation frame at the same time
	 * Otherwise it would look very borning.
	 */
	public void randomizeFrame() {
		// Todo.. Connect this to the actual max frames
		Random randomGenerator = new Random();
		this.frame = randomGenerator.nextInt(10);
	}
}


