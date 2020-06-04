package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamelogic.Board;

public class SendBoard extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 2212783132240431891L;
	public Board board = null;
	
	public SendBoard(Long uID, Board b) {
		super(uID);		
		board=b;
	}
		
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);

		if (gameController instanceof GameControllerClient) {
			gameController.setBoard(userID, board);
		}
		else {
		}
		
		return event;
	}	
}
