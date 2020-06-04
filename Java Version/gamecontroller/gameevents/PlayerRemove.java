package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;

public class PlayerRemove extends GameEvent {
	    
	/**
	 * 
	 */
	private static final long serialVersionUID = 4399784417909970907L;

	public PlayerRemove(Long uID) {
		super(uID);
	}
	
	/**
	 * Create Player resources or a new player connected to the server
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		gameController.removePlayer(userID);
		
		return event;
	}	
}
