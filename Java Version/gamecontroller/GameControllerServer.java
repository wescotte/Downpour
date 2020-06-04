package com.bestgroup.downpour.gamecontroller;

import java.util.concurrent.LinkedBlockingQueue;

import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;

public class GameControllerServer extends GameController {
 
    public GameControllerServer(LinkedBlockingQueue<GameEvent> in,
			LinkedBlockingQueue<GameEvent> out) {
		super(in, out);
	}

}
