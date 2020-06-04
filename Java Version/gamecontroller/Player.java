package com.bestgroup.downpour.gamecontroller;

import com.bestgroup.downpour.gamelogic.*;

public class Player {
	private Long userID;
	private String name;
	
	private Board board;
		
	public Player(Long uID) {
		userID = uID;
		name = "Nobody";
		
		board = Board.createFullBoard();
	}
	
	public Long getUserID() {
		return userID;
	}
	
	public String getName() {
		return name;
	}

	public Board getBoard() {
		return board;
	}
	
	public void setName(String newName) {
		name=newName;
	}
	
	public void setBoard(Board newBoard) {
		board=newBoard;
	}
	
	public void addClears(int num) {
		board.addClears(num);
	}
}
