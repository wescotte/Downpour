package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;

public class UserIDRequest extends GameEvent {
	    
	/**
	 * 
	 */
	private static final long serialVersionUID = 4374078285441333553L;

	public UserIDRequest(Long uID) {
		super(uID);
	}
	
	/**
	 * Create Player resources or a new player connected to the server
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		if (gameController instanceof GameControllerClient) {
			event = new UserIDRequest(userID);
		}
		else {
		}

		return event;
	}	
}
