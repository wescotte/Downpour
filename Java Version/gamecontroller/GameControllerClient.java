package com.bestgroup.downpour.gamecontroller;

import java.util.Iterator;
import java.util.concurrent.LinkedBlockingQueue;

import org.eclipse.swt.SWT;

import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;

public class GameControllerClient extends GameController {

	private Long myUserID = new Long(-1);
	public String myUserName = new String("DefaultUser");
	
	private Long playerNumbers[];
	
	/*
	 * When a GameEvent requires the screen to be updated with new information this is set to true
	 */
	public Boolean forceRedraw = false;
	
    public GameControllerClient(LinkedBlockingQueue<GameEvent> in,
			LinkedBlockingQueue<GameEvent> out, Long newPlayerNumbers[]) {
		super(in, out);
		
		playerNumbers=newPlayerNumbers;
	}
    
    public Long getMyUserID() {
    	return myUserID;
    }
    
    public void setMyUserID(Long uid) {
    	myUserID = uid;
    } 
    
	public void addPlayer(Long newUserID) {
		super.addPlayer(newUserID);

		int i=1;
		for(Iterator it = getPlayers().keySet().iterator(); it.hasNext();) {
		    Object key = it.next();
		    Player p = getPlayers().get(key);
		   
		    if (!key.equals(myUserID)) {
		    	playerNumbers[i] = (Long) key;
		    	p.setName(myUserName);
		    	i++;
		    }
		    else
		    	playerNumbers[0] = (Long) key;
		}			
	}

	public void removePlayer(Long userID) {
		super.removePlayer(userID);
		
		for (int i=0; i < 6; i++)
			if (playerNumbers[i].equals(userID))
				playerNumbers[i] = (long) -1;
	}
}
