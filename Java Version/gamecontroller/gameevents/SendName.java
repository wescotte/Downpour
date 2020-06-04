package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.Player;

public class SendName extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = -5711337151336272245L;
	
	String name=null;
	
	public SendName(Long uID, String newName) {
		super(uID);
		
		name=new String(newName);		
	}
	
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);

		Player p = gameController.getPlayer(userID);
		
		if (p != null) 
			p.setName(name);

		if (gameController instanceof GameControllerClient) {
			((GameControllerClient) gameController).forceRedraw = true;
		}
		else if (gameController instanceof GameControllerServer){
			event = new SendName(userID, name);
		}

		return event;
	}
}
