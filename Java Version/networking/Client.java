package com.bestgroup.downpour.networking;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import com.bestgroup.downpour.gamecontroller.gameevents.*;



public final class Client extends Thread {
	// private LinkedBlockingQueue<GameEvent> msgsFromServer = new LinkedBlockingQueue<GameEvent>();
	private BlockingQueue<GameEvent> msgsToDownpour;
	private BlockingQueue<GameEvent> msgsFromDownpour;
	private ObjectInputStream ois;
	private ObjectOutputStream oos;
	private Socket socket;

	
	private static Logger logger = Logger.getLogger(Client.class.getName());
	

	/**
	 * Networking Client to connect to main DPServer hosting the game
	 * @param msgsToDownpour non-blocking queue passed for storing incoming events
	 * @param msgsFromDownpour	non-blocking queue reference of queue that holds events 
	 * 							to be sent
	 */
	public Client(BlockingQueue<GameEvent> msgsToDownpour,
			BlockingQueue<GameEvent> msgsFromDownpour) {
		super();
		this.msgsToDownpour = msgsToDownpour;
		this.msgsFromDownpour = msgsFromDownpour;
	}

	/**
	 * Opens a new socket connection and starts a new thread to automatically read new game events
	 * 
	 * 
	 * @throws IOException
	 * @throws UnknownHostException
	 */
	public void connect(String serverAdd, int serverPort) throws IOException,
			UnknownHostException {
		this.socket = new Socket(serverAdd, serverPort);
		ois = new ObjectInputStream(socket.getInputStream());
		oos = new ObjectOutputStream(socket.getOutputStream());
		
		this.start();
	}

	/**
	 * Implicitly disconnects the socket 
	 */
	public void disconnect() {
		try {
			socket.shutdownInput();
			socket.shutdownOutput();
			socket.close();
			this.interrupt();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	/**
	 * 
	 * @return	true if currently connected to server
	 */
	public boolean isConnected() {
		if (socket == null)
			return false;
		
		return socket.isConnected();
	}
	
	/**
	 * Send a GameEvent to the server
	 * 
	 * this is only left for convenience, instead outgoing events should be placed in the
	 * blocking-queue that was passed in this clients constructor
	 * 
	 * must be already connected to server.
	 * @param ge GameEvent to send
	 * @throws IOException
	 */
	public synchronized void sendEvent(GameEvent ge) throws IOException {
			oos.writeObject(ge);
			oos.flush();
			
			logger.info("Sent Event " + ge.toString());
	}
	
	/**
	 * Non-blocking check for new events, returns immediately
	 * @return	GameEvent if no new events returns null
	 */
	public GameEvent getEvent(){
		return msgsToDownpour.poll();
	}
	
	/**
	 * blocking check for server events, will wait until new event is available
	 * @return	GameEvent 
	 * @throws InterruptedException
	 */
	public GameEvent waitEvent() throws InterruptedException{
		return msgsToDownpour.take();
	}
	
	public void run() {
		try {
			Object ev = null;
			GameEvent ge = null;
			logger.info("Connection to server Thread Started");
			while (socket.isConnected()) {
				/**
				 * read from object stream put in ToDownpour queue if one exists
				 */
				
//				if (ois.available() >= 1) {
				if (socket.getInputStream().available() >1) {
					
					ev = ois.readObject();
					if (ev != null) {
						logger.info("Got an event from the server: "
								+ ev.toString());
						msgsToDownpour.offer((GameEvent) ev, 200,
								TimeUnit.MILLISECONDS);
					}
					else {
						logger.info("Got a NULL event from the server");
					}
				}
				/**
				 * check incoming queue and send event if there is one
				 */
				ge = msgsFromDownpour.poll();
				if (ge != null){
					oos.writeObject(ge);
					oos.flush();
					logger.info("Sent Event " + ge.toString());
				}
			}
		} catch (IOException e) {

		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// Thread got interrupted so close the socket
			disconnect();
			e.printStackTrace();
		}

	}

	
}

