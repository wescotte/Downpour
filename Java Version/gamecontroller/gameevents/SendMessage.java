package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;

public class SendMessage extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 4714338446018280396L;
	String message;
	
	SendMessage(Long uID, String newMessage) {
		super(uID);
		
		message=newMessage;
	}

	public GameEvent Process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);

		if (gameController instanceof GameControllerClient) {		
			// TODO: Add message to TextWindow	
			System.out.println(message);
		}
		else if (gameController instanceof GameControllerServer) {
			String newMessage = gameController.getName(userID) + ": " + message;		
			event = new SendMessage(userID, newMessage);
		}

		return event;
	}
		
}
