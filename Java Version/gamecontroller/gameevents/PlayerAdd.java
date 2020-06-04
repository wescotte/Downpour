package com.bestgroup.downpour.gamecontroller.gameevents;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;

public class PlayerAdd extends GameEvent { 
	/**
	 * 
	 */
	private static final long serialVersionUID = -7174152304567979576L;
	private ArrayList<Long> allUserIDs;

	
	public PlayerAdd(Long uID, ArrayList<Long> newAllUserIDs) {
		super(uID);
		
		allUserIDs = newAllUserIDs;
	}
	
	/**
	 * Create Player resources or a new player connected to the server
	 */
	public GameEvent process(GameController gameController) {
		GameEvent event = new GameEventNoReply(userID);
				
		if (gameController instanceof GameControllerClient) {
			GameControllerClient gc = (GameControllerClient) gameController; 

			for (Long newUserID : allUserIDs) {
				gameController.addPlayer(newUserID);
			}
			
			if (gc.getMyUserID().equals(userID)) {
				event = new SendName(userID, gc.myUserName);
			}
			
			gc.forceRedraw = true;
		}
		
		else if (gameController instanceof GameControllerServer) {			
			gameController.addPlayer(userID);		
			System.out.printf("Sever: Adding play %d\n", userID);

			ArrayList<Long> allUserIDs = new ArrayList<Long>();
			ArrayList<SendName> allNames = new ArrayList<SendName>();
			
			GameControllerServer gc = (GameControllerServer) gameController;
			HashMap<Long, Player> gamePlayers = gc.getPlayers();

			for(Iterator<Long> it = gamePlayers.keySet().iterator(); it.hasNext();) {
			    Object key = it.next();
			    Player p = gameController.getPlayers().get(key);
				System.out.printf("Sever: REALLY Adding player %d\n", key);
			    
			    allUserIDs.add((Long) key);
			    SendName n = new SendName((Long) key, p.getName());
			    
			    allNames.add(n);
			}	
			
			GameEvent e = new PlayerAdd(userID, allUserIDs);
			gameController.pushOut(e);
			
			for (SendName newUserName : allNames) {
				gameController.pushOut(newUserName);
			}
			
		}
		
		return event;
	}	
}
