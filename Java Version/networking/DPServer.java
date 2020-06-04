/* 	Downpour TCP Server
	Can be started in a thread programatically e.g.:
		DPServer dpserver = new DPServer();
		Thread t = new Thread(dpserver);
		t.start();
	Or it can be run independently from the command-line.
	In either case, it looks for settings in server.properties file
	unless you supply them through the constructor.
	If properties file fails it defaults to port 5152
	and 6 maximum connections
 */
package com.bestgroup.downpour.networking;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import com.bestgroup.downpour.gamecontroller.GameController;
import com.bestgroup.downpour.gamecontroller.GameControllerServer;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;
import com.bestgroup.downpour.gamecontroller.gameevents.UserIDReply;
import com.bestgroup.downpour.gamecontroller.gameevents.UserIDRequest;


public class DPServer extends Thread {
	private static Logger logger = Logger.getLogger(DPServer.class.getName());
	private static int port;
	private static int maxConnections;
	// private static Hashtable<String, ServerPlayer> players = new
	// Hashtable<String, ServerPlayer>();
	protected static Hashtable<Long, ClientHandler> clients = new Hashtable<Long, ClientHandler>();
	
	protected static LinkedBlockingQueue<GameEvent> msgsFromClients = new LinkedBlockingQueue<GameEvent>();
	
	protected static LinkedBlockingQueue<GameEvent> msgsToClients = new LinkedBlockingQueue<GameEvent>();
	
	private static GameControllerServer gcServer;
	
	/**
	 * requires you to pass both for the client side interface and the
	 * drivers 
	 */
	public DPServer() {
		try {
			Properties props = new Properties();
			props.load(this.getClass().getResourceAsStream("server.properties"));
			port = Integer.parseInt(props.getProperty("port"));
			maxConnections = Integer.parseInt(props
					.getProperty("maxConnections"));
		} catch (Exception e) {
			e.printStackTrace();
			port = 5151;
			maxConnections = 10;
		}
		
		
	}
	
	public static void main(String[] args){
		try {
			DPServer server = new DPServer();
			gcServer = new GameControllerServer(msgsFromClients, msgsToClients);

			gcServer.start();
			server.start();
		
			while(true){
				GameEvent ge = msgsToClients.take();
				DPServer.broadcastEvent(ge);
			}
		}
		catch (Exception e){
			// do nothing
		}
	}
	
	public void run() {
		try {
			ServerSocket listener = new ServerSocket(port);
			Socket newClientSocket;
			logger.info("New server listener thread started on port " + port + " with "
					+ maxConnections + " maximum connections.");
			while ((newClientSocket = listener.accept()) != null) { // blocks
																	// until new
																	// connection
				logger.fine("Someone connected to the port...");
				if ((clients.size() < maxConnections) || (maxConnections == 0)) {
					ClientHandler newClientThread = new ClientHandler(
							newClientSocket);
					newClientThread.start();
					logger.fine("new Client thread started and returned from...");
					clients.put(newClientThread.clientID, newClientThread);
					logger.info("New client connected making " + clients.size()
							+ " total concurrent connections...");
				} else {
					logger.info("Too Many Clients Connected. Closing new socket attempt.");
					newClientSocket.close();
				}
				logger.info("Server waiting for connection...");
			}
		} catch (IOException ioe) {
			System.out.println("IOException on socket: " + ioe);
			ioe.printStackTrace();
		} 
	}
	public synchronized static void disconnectClient(Long cid){
		ClientHandler cThread = clients.get(cid);
		if (cThread != null){
			cThread.interrupt();
			logger.info("disconnected and interrupted client thread id: "+cid);
		}
	}
	protected synchronized static void removeClient(Long cid) {
		gcServer.removePlayer(cid);
		
		if (DPServer.clients.remove(cid) != null) {
			logger.info("Client " + cid
					+ " successfully removed from list of attached clients");
		} else {
			logger.info("Tried to remove client that didn't exist");
		}

	}
	/**
	 * Sends broadcast to all connected clients
	 * @param ge
	 * @throws IOException
	 */
	public synchronized static void broadcastEvent(GameEvent ge)
			throws IOException {
		logger.info("Broadcasting event: " + ge.toString());
		
		Enumeration<Long> keys = clients.keys();
		while (keys.hasMoreElements()) {
			Long key = keys.nextElement();
			clients.get(key).sendEvent(ge);
		}
	}
	
	/**
	 * Send a single event to specified ClientID
	 * @param ge Game Event to Send
	 * @param cid client identifier
	 * @throws IOException
	 */
	public synchronized static void sendEvent(GameEvent ge, Long cid) throws IOException {
		clients.get(cid).sendEvent(ge);
	}

}

class ClientHandler extends Thread {
	private Socket socket;
	private static Logger logger = Logger.getLogger(Client.class.getName());
	private ObjectInputStream ois;
	private ObjectOutputStream oos;
	Long clientID;

	ClientHandler(Socket socket) throws IOException {
		this.socket = socket;
		this.oos = new ObjectOutputStream(socket.getOutputStream());
		this.ois = new ObjectInputStream(socket.getInputStream());
		this.clientID = new Long(this.getId());
	}

	synchronized void sendEvent(GameEvent ge) throws IOException {
		oos.writeObject(ge);
		oos.flush();
		oos.reset();
		logger.info("Sent Event2 " + ge.toString());
	}

	/**
	 * 
	 * reads objects sent over socket and adds them to main server in queue
	 * TODO: make this private? should never be called directly.
	 */
	public void run() {
		try {
			while (socket.isConnected()) {
				Object ev = ois.readObject();
				if (ev != null) {
					logger.info("Recieved new game event from: " + clientID
							+ " , with properties: " + ev.toString());
					if (ev instanceof UserIDRequest){
						logger.info("Recognized incoming id request");
						sendEvent(new UserIDReply(clientID));						
					}
					else {
					DPServer.msgsFromClients.offer((GameEvent) ev, 200,
							TimeUnit.MILLISECONDS);
					}
				}
			}
			// Loop exited so now we gotta clean up
			// DPServer.removeClient(clientID);
			throw new InterruptedException();
		} catch (IOException ioe) {
			System.out.println("IOException on socket listen: " + ioe);
			ioe.printStackTrace();
		} catch (ClassCastException e) {
			System.out.println("Non-event object sent");
		} catch (ClassNotFoundException e2) {
			System.out.println("Unresolvable object type sent");
		} catch (InterruptedException e) { // Thread interrupted
			DPServer.removeClient(clientID);
			logger.info("Client connection closed or thread interrupted.");
		}

	}

}
