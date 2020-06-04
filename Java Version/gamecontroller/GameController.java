package com.bestgroup.downpour.gamecontroller;


import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEventNoReply;
import com.bestgroup.downpour.gamelogic.Board;

import java.util.HashMap;
import java.util.Iterator;
import java.util.concurrent.LinkedBlockingQueue;

import org.eclipse.swt.SWT;

public class GameController extends Thread {
	private LinkedBlockingQueue<GameEvent> inputQueue;
	private LinkedBlockingQueue<GameEvent> outputQueue;

	private HashMap<Long, Player> gamePlayers = new HashMap<Long, Player>();
	
	GameState gameState = null;
	Boolean keepListening=true;
    
	public GameController(LinkedBlockingQueue<GameEvent> in, LinkedBlockingQueue<GameEvent> out) {
		inputQueue = in;
		outputQueue = out;
		
		gameState = new GameNotPlaying();
	}
	
	public void shutdown() {
		keepListening = true;
	}
	
	public void run() {
		GameEvent incomingEvent = null, outgoingEvent;
		
		while (keepListening) {
			try {
				incomingEvent = inputQueue.take();
				
				if (this instanceof GameControllerClient)
					System.out.print("GameControllerClient got: ");
				else
					System.out.print("GameControllerServer got: ");
				System.out.println(incomingEvent.toString());
				
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			/*
			 * And outgoingEvent is created in response to all events and is pushed onto the outgoing queue 
			 * For example if a client sends a Piece the server processes the piece on the board
			 * 	and then sends the newly processed board to all clients
			 * 
			 * If an event doesn't need to respond a dummy event of type GameEventNoReply is used instead. These
			 * events are not pushed onto the outgoing queue
			 */
			outgoingEvent = incomingEvent.process(this);
/*			
			if (!(outgoingEvent instanceof GameEventNoReply))
				if (this instanceof GameControllerClient)
					System.out.print("GameControllerClient sending: ");
				else
					System.out.print("GameControllerServer sending: ");
*/						
			if (!(outgoingEvent instanceof GameEventNoReply))
				try {
					outputQueue.put(outgoingEvent);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					super.interrupt();
				}
			
		}
	}
	
	public void push(GameEvent e) {
		try {
			inputQueue.put(e);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
	
	public void pushOut(GameEvent e) {
		try {
			outputQueue.put(e);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
	
	/*
	 * Non blocking Pop. If no events exist it will pop a NO_RESPONSE GameEvent
	 * TODO: Maybe also create a blocking version?
	 */
	public GameEvent pop() {
		GameEvent event = outputQueue.poll();
		
		/* If there is no event in the Queue we return a NO_RESPONSE GameEvent
		 * 
		 */
		if (event == null)
			event = new GameEvent((long) 0);
		
		return event;
	}
	
	public void clearAllBoard() {
		for(Iterator it = gamePlayers.keySet().iterator(); it.hasNext();) {
		    Object key = it.next();
		    Player p = gamePlayers.get(key);
		    p.setBoard(Board.createEmptyBoard());
		}
	}
	
	public void fillAllBoard() {
		for(Iterator it = gamePlayers.keySet().iterator(); it.hasNext();) {
		    Object key = it.next();
		    Player p = gamePlayers.get(key);
		    p.setBoard(Board.createFullBoard());
		}		
	}
	
	public GameState getGameState() {
		return gameState;
	}
	
	// TODO: Maybe return Boolean?
	public void setState(GameState newState) {
		gameState = newState;
	}
	
	public void addPlayer(Long newUserID) {
		Long userID = new Long(newUserID);
		
		if (gamePlayers.containsKey(userID)) {
			// TODO: Throw exception because player already exists
		} 
		else {
			Player p = new Player(userID);
			gamePlayers.put(userID, p);
		}
			
	}
	
	public void removePlayer(long userID) {	
		if (!gamePlayers.containsKey(userID)) {
			// TODO: Throw Exception becuase player DOESN'T exist
		}
		else {
			gamePlayers.remove(userID);
		}
	}
	
	public HashMap<Long, Player> getPlayers() {
		return gamePlayers;
	}
	
	public Player getPlayer(long userID) {
		return gamePlayers.get(userID);
	}
	
	public String getName(long userID) {
		// TODO: Throw exceptions...
		return gamePlayers.get(userID).getName();
	}
	
	public Board getBoard(long userID) {
		Player p = gamePlayers.get(userID);
		
		if (p == null)
			return null;
		else
			return p.getBoard();
	}
	
	public void setBoard(long userID, Board newBoard) {
		Player p = getPlayer(userID);
		
		if (p != null) {
			p.setBoard(newBoard);
		}
	}
	
	public void setName(long userID, String newName) {
		gamePlayers.get(userID).setName(newName);

		System.out.println(" GC " + gamePlayers.toString() + " GC");
		
		for(Iterator it = getPlayers().keySet().iterator(); it.hasNext();) {
		    Object key = it.next();
		    Player p = getPlayers().get(key);
		}			
		System.out.println();
		
	}
}
