package com.bestgroup.downpour.gamecontroller.gameevents;

import java.util.Iterator;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.GameNotPlaying;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamelogic.Board;

public class GameStop extends GameEvent {

	/**
	 * 
	 */
	private static final long serialVersionUID = -6532864501565555769L;

	public GameStop(Long uID) {
		super(uID);
	}
	
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);

		gameController.setState(new GameNotPlaying());
		
		if (gameController instanceof GameControllerClient) {
		}
		else if (gameController instanceof GameControllerServer) {
			
			for(Iterator<Long> it = gameController.getPlayers().keySet().iterator(); it.hasNext();) {
			    Object key = it.next();
			    Player p = gameController.getPlayers().get(key);
			    
			    p.setBoard(Board.createFullBoard());

			    GameEvent e;			    
				e = new SendBoard((Long) key, p.getBoard());
				gameController.pushOut(e);
			}	
			event = new GameStop(userID);	
		}
		return event;
	}		
}
