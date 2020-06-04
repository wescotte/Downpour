package com.bestgroup.downpour.gamecontroller.gameevents;


/**
 * Dummy GameEvent used because every time an event is processed via process()
 * it should return a new GameEvent as a response. However for events that don't require
 * a response this event is created instead and is never pushed onto the outgoing queue
 * @author Eric
 *
 */
public class GameEventNoReply extends GameEvent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 5343952887042419855L;

	public GameEventNoReply(Long uID) {
		super(uID);
	}
}
