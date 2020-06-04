package com.bestgroup.downpour.gamecontroller.gameevents;


import com.bestgroup.downpour.gamelogic.BoardLogic;
import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GamePlaying;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;

public class MoveRight extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = -8672686115120157823L;

	public MoveRight(Long uID) {
		super(uID);
	}

	/**
	 * Moves curPiece[] right with a side effect to curPiece[] has new x coordinates
	 * @return Empty GameEvent with type NO_RESPONSE
	 */	
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		if (gameController instanceof GameControllerClient) {
			GameControllerClient gc = (GameControllerClient) gameController;
			
			if (gc.getGameState() instanceof GamePlaying) {
				BoardLogic bl=new BoardLogic(gameController.getPlayer(userID), gameController);
				bl.moveRight();
			}
		}
		
		return event;
	}
}
