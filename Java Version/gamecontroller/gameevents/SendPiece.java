package com.bestgroup.downpour.gamecontroller.gameevents;

import com.bestgroup.downpour.gamelogic.Board;
import com.bestgroup.downpour.gamelogic.BoardLogic;
import com.bestgroup.downpour.gamelogic.Piece;
import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.GamePlaying;

public class SendPiece extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 3354909207467729002L;
	Piece curPiece[];
	/*
	 * Duplicates a Piece[] and sends it to the server to perform game logic on
	 * This will always be a copy of Board::curPiece[]
	 * The reason it's duplicated is because after a MovePiece event is processed
	 *	curPiece[] is overwritten by nextPiece[] and is lost
	 */
	public SendPiece(Long uID, Piece curP[]) {
		super(uID);
		curPiece = curP;
	}
		
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
		
		/** Technically this should never happen when gameState != PLAYER 
		 * TODO: Throw an exception perhaps?
		 */
		if (gameController.getGameState() instanceof GamePlaying) {
			Board b=gameController.getBoard(userID);
			
			if (gameController instanceof GameControllerClient) {
				event = new SendPiece(userID, b.curPiece);
			}
			else if (gameController instanceof GameControllerServer) {
				// TODO: Check to make sure the incoming Piece is what we're expecting.. If not.. CHEATER!!!
				b=gameController.getBoard(userID);
				int x1=curPiece[0].x; int x2=curPiece[1].x;
				int y1=curPiece[0].y; int y2=curPiece[1].y;

				b.board[x1][y1]= new Piece(curPiece[0]);
				b.board[x2][y2]= new Piece(curPiece[1]);
				
				displayBoard(b);

				BoardLogic bl=new BoardLogic(gameController.getPlayer(userID), gameController);
				
				bl.boardCheck();
				bl.nextPiece();
				/*
				 * Only send clear blocks to a player AFTER they position their current piece.
				 * TODO: Figure out a way to avoid sending it if a comobo is still in progress? Or just send it anyway? 
				 */
				bl.dropClears();
				event = new SendBoard(userID, gameController.getPlayer(userID).getBoard());
			}
		}
		
		return event;
	}
	public void displayBoard(Board b) {
		for (int y=0; y < 12; y++) {
			System.out.printf("[Row %2d] --- ", y+1);
			for (int x=0; x < 6; x++) {
				System.out.printf("[%6.6s]", b.board[x][y].getColor());
			}
			System.out.printf("\n");
		}
		System.out.printf("[Current Piece]\t***\t[%5.5s]\t[%5.5s]\n", b.curPiece[0].getColor(), b.curPiece[1].getColor());
		System.out.printf("[Next    Piece]\t***\t[%5.5s]\t[%5.5s]\n", b.nextPiece[0].getColor(), b.nextPiece[1].getColor());
		System.out.printf("Number of incoming CLEARS: %d\n", b.numberOfIncomingClears);
	}

}
