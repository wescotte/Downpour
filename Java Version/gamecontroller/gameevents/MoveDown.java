package com.bestgroup.downpour.gamecontroller.gameevents;


import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GamePlaying;
import com.bestgroup.downpour.gamelogic.Board;
import com.bestgroup.downpour.gamelogic.BoardLogic;

public class MoveDown extends GameEvent {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 5600039394368645769L;


	public MoveDown(Long uID) {
		super(uID);
	}
	

	/**
	 * Move curPiece[] down 1 space with 2 possible side effects
	 * 1) curPiece[] has new y coordinates
	 * 2) If curPiece hits the bottom of the board or another game piece
	 * 	curPiece[] = nextPiece[]
	 * @return SendPiece GameEvent 
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		//e = new UserIDRequest(userID);
		
		if (gameController instanceof GameControllerClient) {
			GameControllerClient gc = (GameControllerClient) gameController;
			
			if (gc.getGameState() instanceof  GamePlaying) {
				Board b=gc.getBoard(userID);
				
				/* Move the piece down 1 spot.. If movePiece() returns TRUE that means the piece has
				 * bottomed out and it is now locked in place on the board. So send it to the server
				*/
				BoardLogic bl = new BoardLogic(gameController.getPlayer(userID), gameController);
				
				if ( bl.movePiece() )
					event = new SendPiece(userID, b.curPiece);
			}
		}
		
		return event;
	}
}
