package com.bestgroup.downpour.gamecontroller.gameevents;

import java.util.Iterator;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamelogic.Board;

public class SendClears extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 3649420670331848468L;
	int numberOfClears = 0;
	
	public SendClears(Long uID, int num) {
		super(uID);		
		numberOfClears = num;
	}
		
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);

		if (gameController instanceof GameControllerClient) {
			//TODO: Display incoming CLEARs and remove System.out...			
			((GameControllerClient) gameController).forceRedraw = true;			
		}
		else if (gameController instanceof GameControllerServer) {
			event = new SendClears(userID, numberOfClears);
			for(Iterator<Long> it = gameController.getPlayers().keySet().iterator(); it.hasNext();) {
			    Object key = it.next();
			    Player p = gameController.getPlayers().get(key);
			    if (p.getUserID() != userID) {
			    	p.addClears(numberOfClears); 
			    }
			}
		}
		return event;
	}

}
