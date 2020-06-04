package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;

public class UserIDReply extends GameEvent {
	    
	/**
	 * 
	 */
	private static final long serialVersionUID = 7569122202309239185L;

	public UserIDReply(Long uid) {
		super(uid);
	}
	
	/**
	 * Create Player resources or a new player connected to the server
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		if (gameController instanceof GameControllerClient) {
			GameControllerClient gc = (GameControllerClient) gameController;
			gc.setMyUserID(userID);
			
			event = new PlayerAdd(userID, null);
		}
		else {
			/* TODO: Check all cases of GameEvents?
			 * Throw an exception? These events should never be received by the server...
			 * Do this for all GameEvents?
			 */
		}
				
		return event;
	}	
}
