package com.bestgroup.downpour.gamecontroller.gameevents;

import java.util.Iterator;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.GamePlaying;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamelogic.Board;

public class GameStart extends GameEvent{
	/**
	 * 
	 */
	private static final long serialVersionUID = -7848566429247140733L;

	public GameStart(Long uID) {
		super(uID);
	}
	

	public GameEvent process(GameController gameController) {
		GameEvent event =  new GameEventNoReply(userID);

		gameController.setState(new GamePlaying());					

		if (gameController instanceof GameControllerClient) {
			// TODO: You should be able to remove clearAllBoard() when network actually functions
//			gameController.clearAllBoard();	
		}
		else if (gameController instanceof GameControllerServer) {
			/**
			 * We have to send EVERY board to EVERY player...
			 */
			gameController.clearAllBoard();

			for(Iterator<Long> it = gameController.getPlayers().keySet().iterator(); it.hasNext();) {
			    Object key = it.next();
			    Player p = gameController.getPlayers().get(key);
			    
			    p.setBoard(Board.createEmptyBoard());

			    GameEvent e;			    
				e = new SendBoard((Long) key, p.getBoard());
				gameController.pushOut(e);
				
				event = new GameStart(userID);
			}			
		}

		
		return event;
	}	
}
