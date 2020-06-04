package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamelogic.BoardLogic;
import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GamePlaying;

public class RotateLeft extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 8383481507222162478L;

	public RotateLeft(Long uID) {
		super(uID);
	}
	
	/**
	 * Rotates curPiece[] counter clockwise with a side effect to curPiece[] has new x/y coordinates
	 * @return Empty GameEvent with type NO_RESPONSE
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		if (gameController instanceof GameControllerClient) {
			GameControllerClient gc = (GameControllerClient) gameController;
			
			if (gc.getGameState() instanceof GamePlaying) {
				BoardLogic bl=new BoardLogic(gameController.getPlayer(userID), gameController);
				bl.rotateLeft();
			}
		}
		
		return event;
	}
}
