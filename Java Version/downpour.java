package com.bestgroup.downpour;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.Iterator;
import java.util.concurrent.LinkedBlockingQueue;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.*;

import com.bestgroup.downpour.gamecontroller.GameControllerClient;
import com.bestgroup.downpour.gamecontroller.GamePlaying;
import com.bestgroup.downpour.gamecontroller.Player;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEvent;
import com.bestgroup.downpour.gamecontroller.gameevents.GameEventNoReply;
import com.bestgroup.downpour.gamecontroller.gameevents.GameStart;
import com.bestgroup.downpour.gamecontroller.gameevents.GameStop;
import com.bestgroup.downpour.gamecontroller.gameevents.MoveDown;
import com.bestgroup.downpour.gamecontroller.gameevents.MoveLeft;
import com.bestgroup.downpour.gamecontroller.gameevents.MoveRight;
import com.bestgroup.downpour.gamecontroller.gameevents.PlayerAdd;
import com.bestgroup.downpour.gamecontroller.gameevents.RotateLeft;
import com.bestgroup.downpour.gamecontroller.gameevents.RotateRight;
import com.bestgroup.downpour.gamecontroller.gameevents.SendName;
import com.bestgroup.downpour.gamecontroller.gameevents.UserIDRequest;
import com.bestgroup.downpour.gamelogic.Board;
import com.bestgroup.downpour.gamelogic.Piece;
import com.bestgroup.downpour.gamelogic.Piece.pieceType;
import com.bestgroup.downpour.networking.Client;

public class downpour {
	// SWT GUI Widgets
	private static Display myDisplay;
	private static Shell mainShell;
	private static Text chatWindow;
	private static Text chatInput;
	
	private static Shell connectionDialog;
	private static Shell nameDialog;
	
	private static Canvas mainBoard;
	private static Canvas otherBoards[] = new Canvas[6];
	private static Long playerNumbers[] = new Long[6];
	
	private static Canvas nextPiece;
	private static Image bg_image;
	
	private static Button startButton;
	private static Button connectButton;
	private static Button nameButton;
	
	private static Boolean advanceFrame = false;
	
	private static Client connection;
	/** 
	 * The actual image of all the game pieces and all it's animated frames
	 * 
	 * The format is as follows
	 * 4x4 grid for each frame with the following connections 
	 * N=None R=Right L=Left U=Up D=Down
	 * 
	 * N 	R	D	RD
	 * L	RL	DL	RDL
	 * U	UR	UD	URD
	 * UL	URL	UDL	URDL
	*/
	private static ImageData piecesImage;
	
	/*
	 * TODO: Change type form ServerBoard to ClientBoard
	 * 
	 * Temporarly using ServerBoard class to demo all game logic is functioning
	 */
//	private static BoardLogic playerBoard;
//	static Long myUserID = (long) 0;
	
	// Number of milliseconds to wait to draw another frame
	/* TODO: Investigate drawing the frames to an offscreen buffer during the waiting time rather than doing it all at once */
	private final static int FPS=120;
	
	/* Number of milliseconds to wait before dropping a piece down one spot
	 * TODO: This should be set by the server and immutable by the player
	 * TODO: Add "levels" so the pieces drop faster over time
	 */
	private static int DROP_RATE=2000;
	
	/* TODO: Detect pieceSize from image file
	 * Pieces are Sqaure. So pieceSize = image width/6
	 * maxFames = image height / (pieceSize * 6) 
	*/
	private final static int pieceSize=30;
	private static int maxFrames=10;

	// Can probably remove these from the Downpour class because you never access then directly
	// use .push .pop
	private static LinkedBlockingQueue<GameEvent> msgsFromServer;
	private static LinkedBlockingQueue<GameEvent> msgsToServer;

	static GameControllerClient gameController;

	/**
	 * Client game loop.
	 * 
	 * While the mainShell GUI Widget exists read events, process them and sleep between
	 * 
	 * @param args Command line arguments. For now everything is ignored. 
	 */
	public static void main (String [] args) {

		setup();
		
		mainShell.open();
				
		while (!mainShell.isDisposed ()) {
			if (!myDisplay.readAndDispatch ()) 
				myDisplay.sleep ();
		}
		
		shutdown();
	}
	
	/**
	 * Initialize all SWT GUI Elements
	 *
	 * Initialize primary display and create a shell widget. Then populate
	 * it with 
	 * Canvas: Draw all game boards/animations on
	 * Text: For user text input and another dedicated to chat history
	 * 
	 *  TODO: Finalize widget layout and add event handlers
	 */
	private static void setup() {
		myDisplay = new Display ();
		mainShell = new Shell(myDisplay);
		mainShell.setSize(640,480);

		setupPiecesImage();
		setupBackgroundImage();
		setupCanvas();
		setupNames();
		
//		setupChatWidgets();
		setupKeyboardInput();
		
		setupButtons();
		
		setupGameController();	
		setupAnimation();		
	}
	
	private static void showConnectionDialog() {
		connectionDialog = new Shell(mainShell, SWT.APPLICATION_MODAL | SWT.DIALOG_TRIM);
		connectionDialog.setText("Connect To");
		connectionDialog.setSize(300, 200);
	
		final Text address = new Text(connectionDialog, SWT.BORDER);
		address.setSize(200,20);
		address.setLocation(0,100);		
		address.setText("127.0.0.1");
		
		final Text port = new Text(connectionDialog, SWT.BORDER);
		port.setSize(60,20);
		port.setLocation(225,100);		
		port.setText("5152");
		
		Button ok = new Button(connectionDialog, SWT.PUSH);
		ok.setText("OK");
		ok.setBounds(0,0,50,50);
	    ok.addSelectionListener(new SelectionListener() {    	
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				if (connection.isConnected())
					connection.disconnect();
				
				try {
					connection.connect(address.getText(), Integer.parseInt(port.getText()) );
				} catch (NumberFormatException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (UnknownHostException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
								
				if (connection.isConnected()) {
					GameEvent gameEvent = new UserIDRequest(gameController.getMyUserID());
					gameController.push(gameEvent);
				}
				
				connectionDialog.close();
				mainShell.forceFocus();
			}
		});

	
		Button cancel = new Button(connectionDialog, SWT.PUSH);
		cancel.setText("Cancel");
		cancel.setBounds(65, 0, 50,50);
		
	    cancel.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				connectionDialog.close();
				mainShell.forceFocus();

			}
		});
				
		connectionDialog.open();
	}
	
	private static void showNameDialog() {
		nameDialog = new Shell(mainShell, SWT.APPLICATION_MODAL | SWT.DIALOG_TRIM);
		nameDialog.setText("Connect To");
		nameDialog.setSize(300, 200);
	
		final Text name = new Text(nameDialog, SWT.BORDER);
		name.setSize(200,20);
		name.setLocation(0,100);		
		name.setText(gameController.myUserName);
				
		Button ok = new Button(nameDialog, SWT.PUSH);
		ok.setText("OK");
		ok.setBounds(0,0,50,50);
	    ok.addSelectionListener(new SelectionListener() {    	
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				gameController.myUserName = name.getText();
				
				GameEvent gameEvent = new SendName(gameController.getMyUserID(), name.getText());
				gameController.pushOut(gameEvent);
				
				mainShell.redraw();
				
				nameDialog.close();
				mainShell.forceFocus();
			}
		});

	
		Button cancel = new Button(nameDialog, SWT.PUSH);
		cancel.setText("Cancel");
		cancel.setBounds(65, 0, 50,50);
		
	    cancel.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				nameDialog.close();
				mainShell.forceFocus();
			}
		});
				
		nameDialog.open();
	}

	private static void setupButtons() {
		startButton = new Button(mainShell, SWT.PUSH);
	    startButton.setBounds(225, 250, 65, 25);
	    startButton.setText("Start");
	    
	    startButton.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				
				if (connection.isConnected()) {
					GameEvent gameEvent;
					if (gameController.getGameState() instanceof GamePlaying) {
						gameEvent=new GameStop(gameController.getMyUserID()); 
						startButton.setText("Start");
						mainShell.forceFocus();
					}
					else {
						gameEvent=new GameStart(gameController.getMyUserID());
						startButton.setText("Stop");
						mainShell.forceFocus();
					}
					gameController.pushOut(gameEvent);
				}
			}
		});
	    
		connectButton = new Button(mainShell, SWT.PUSH);
	    connectButton.setBounds(225, 285, 65, 25);
	    connectButton.setText("Connect");

	    connectButton.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				showConnectionDialog();		
			}
		});
	    
	    nameButton = new Button(mainShell, SWT.PUSH);
	    nameButton.setBounds(225, 320, 65, 25);
	    nameButton.setText("Set Name");

	    nameButton.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent event) {
				// TODO Auto-generated method stub		
			}
			public void widgetSelected(SelectionEvent event) {
				// TODO Auto-generated method stub
				showNameDialog();		
			}
		});

	}
	
	private static void setupChatWidgets() {
		chatWindow = new Text(mainShell, SWT.MULTI | SWT.BORDER | SWT.WRAP | SWT.V_SCROLL | SWT.READ_ONLY);
		chatWindow.setSize(400,60);
		chatWindow.setLocation(0,360);
		
		chatInput = new Text(mainShell, SWT.BORDER);
		chatInput.setSize(400,20);
		chatInput.setLocation(0,420);		
		
//		chatWindow.insert("Chat between clients\nRead ONLY"); 
//		chatInput.insert("Type here");		    
		
	}
	
	private static void setupPiecesImage() {
		piecesImage = new ImageData("images/Pieces.bmp");
		piecesImage.transparentPixel = 127;		
	}
	
	private static void setupBackgroundImage() {
		bg_image = new Image(myDisplay, "images/background.bmp");
		mainShell.setBackgroundImage(bg_image);		
	}
	
	private static void setupNames() {		
		mainShell.addListener(SWT.Paint, new Listener() {
			public void handleEvent (Event event) {
				Font font1 = new Font(myDisplay, "Tahoma", 12, SWT.BOLD);
				Font font2 = new Font(myDisplay, "Tahoma", 8, SWT.BOLD);
				int x=220, y=7;
				int spot=0;
				
				for(Iterator it = gameController.getPlayers().keySet().iterator(); it.hasNext();) {
				    Object key = it.next();
				    Player p = gameController.getPlayers().get(key);
				   
				    if (!p.getUserID().equals(gameController.getMyUserID() ) ) {
				    	event.gc.setFont(font2);
				    	event.gc.drawText(p.getName(), x, y, SWT.DRAW_TRANSPARENT);
				    	if (spot == 2) {
				    		x=215; y=y+202;
				    	}
				    	x=x+150;
				    	spot++;	    	
				    }
				    else {
				    	event.gc.setFont(font1);
				    	event.gc.drawText(p.getName(), 7, 4, SWT.DRAW_TRANSPARENT);
				    }
				}			
				for (int i=1; i < 6; i++) {
					if (i == 2) {
						x=215; y=y+202;
					}
					x=x+150;
				}

			}
		});

	}
	
	private static void setupCanvas() {
		mainBoard=new Canvas(mainShell, SWT.NO_BACKGROUND);
		mainBoard.setLocation(5, 26);
		mainBoard.setSize(pieceSize * 6, pieceSize * 12);		
		
		nextPiece=new Canvas(mainShell, SWT.NO_BACKGROUND);
		nextPiece.setLocation(185, 26);
		nextPiece.setSize(pieceSize, pieceSize * 2);		
		
		otherBoards[0]=mainBoard;
		int x=215, y=26;

		for (int i=1; i < 6; i++) {
			otherBoards[i] = new Canvas(mainShell, SWT.NO_BACKGROUND);
			otherBoards[i].setLocation(x,y);
			// These board are half the size of the mainBoard so 3/6 instead of 6/12
			otherBoards[i].setSize(pieceSize * 3 , pieceSize * 6);
			if (i == 3) {
				x=215; y=y+202;
			}
			x=x+150;
			
			final int j=i;
		    otherBoards[i].addPaintListener(new PaintListener() {

		    	public void paintControl(PaintEvent e) {
		    		Image t=drawBoard(j);
		    		if (t != null) {
		    			e.gc.drawImage(t, 0, 0, pieceSize * 6, pieceSize * 12, 0, 0, pieceSize * 3,  pieceSize * 6);		    		
		    			t.dispose();
		    		}
		        }  
		    });
		
		}
		
		nextPiece.addPaintListener(new PaintListener() {
	    	public void paintControl(PaintEvent e) {
	    		Image t=drawNextPiece();
	    		// TODO: Figure out why transparency isn't working right.. The background is redrawn here..
	    		e.gc.drawImage(bg_image, 185, 25, pieceSize, pieceSize * 2, 0, 0, pieceSize, pieceSize * 2);
	    		if (t != null) {
	    			e.gc.drawImage(t, pieceSize * 3, 0, pieceSize, pieceSize * 2, 0, 0, pieceSize, pieceSize * 2);
	    			t.dispose();
	    		}
	        }  
	    });
			
	    mainBoard.addPaintListener(new PaintListener() {
	    	public void paintControl(PaintEvent e) {
	    		Image t=drawBoard(0);
	    		if (t != null) {
	    			e.gc.drawImage(t,0,0);
	    			t.dispose();
	    		}
	        }  
	    });

	}
	
private static void setupKeyboardInput() {
	mainShell.addKeyListener(new KeyListener() {
		@Override
		public void keyPressed(org.eclipse.swt.events.KeyEvent e) {
			// TODO Auto-generated method stub
			GameEvent event = new GameEventNoReply(gameController.getMyUserID());
			mainBoard.redraw();
			switch (e.keyCode) {
				case SWT.ARROW_UP:
					event=new RotateLeft(gameController.getMyUserID());	break;
				case SWT.ARROW_DOWN:
					event=new RotateRight(gameController.getMyUserID()); break;
				case SWT.ARROW_LEFT:
					event=new MoveLeft(gameController.getMyUserID()); break;
				case SWT.ARROW_RIGHT:
					event=new MoveRight(gameController.getMyUserID()); break;
				case SWT.SPACE:
					event=new MoveDown(gameController.getMyUserID()); break;
					
				case SWT.TAB: // TODO: Remove this as it's only for testing
					if (gameController.getGameState() instanceof GamePlaying)
						event=new GameStop(gameController.getMyUserID());
					else
						event=new GameStart(gameController.getMyUserID());
					break;
				case SWT.F1:
					for(Iterator it = gameController.getPlayers().keySet().iterator(); it.hasNext();) {
					    Object key = it.next();
					    Player p = gameController.getPlayers().get(key);
					    System.out.printf("Player: %s -- %d \n",p.getName(), key);
					}	
					
					break;
					
				default:
					event = new GameEvent(gameController.getMyUserID()); break;
			}
			gameController.push(event);				
		}

		@Override
		public void keyReleased(org.eclipse.swt.events.KeyEvent e) {
			// TODO Auto-generated method stub				
		}
	   });
	
	}
	
	private static void setupGameController() {
	    msgsFromServer = new LinkedBlockingQueue<GameEvent>();
	    msgsToServer = new LinkedBlockingQueue<GameEvent>();
	    
		gameController = new GameControllerClient(msgsFromServer, msgsToServer, playerNumbers);
		gameController.start();
		
		connection = new Client(msgsFromServer, msgsToServer);

		for (int i=0; i < 6;i++) {
			playerNumbers[i] = new Long(0);
		}
		/*
		 * TODO: This code below only exists to test locally. Once server exists this will be removed
		 */	
		
		// REMOVE UNECESSARY STUFF IN KEYBOARD HANDLER TOO!!!
		// REMOVE UNECESSARY STUFF IN KEYBOARD HANDLER TOO!!!
		// REMOVE UNECESSARY STUFF IN KEYBOARD HANDLER TOO!!!
		// REMOVE UNECESSARY STUFF IN KEYBOARD HANDLER TOO!!!
		// REMOVE UNECESSARY STUFF IN KEYBOARD HANDLER TOO!!!
	}
	

	/**
	 * Clean up SWT Wigdets
	 * 
	 * TODO: Find out if this is necessary/good practice or if it's better to let the garbage collector do this work
	 */
	private static void shutdown() {
//		chatWindow.dispose();
//		chatInput.dispose();
		mainBoard.dispose();
		mainShell.dispose();
//		myDisplay.dispose();
		
		connection.disconnect();
		gameController.shutdown();
	}

	
	private static Image drawNextPiece() {
		// TODO: mainBoard.getBouds() produces a larger image than we need
		Image bufferImage = new Image(myDisplay, mainBoard.getBounds());
		GC gcBuffer = new GC(bufferImage);
		
		Board playerBoard = gameController.getBoard(gameController.getMyUserID());
		
		if (playerBoard != null)
			if (gameController.getGameState() instanceof GamePlaying) {
				drawPiece(playerBoard.nextPiece[0], gcBuffer);
				drawPiece(playerBoard.nextPiece[1], gcBuffer);
			}
		
		return bufferImage;		
	}
	/** 
	 * Draw each game board to each a Canvas Widget
	 * 
	 * Loop all game boards and draw them to offscreen buffers and then when complete draw the buffers to Canvas widgets
	 * to be displayed on screen. 
	 */
	private static Image drawBoard(int b) {		
		Board playerBoard = gameController.getBoard(playerNumbers[b]);
		if (playerBoard == null)
				return null;
		
		// TODO: mainBoard.getBouds() produces a larger image than we need
		Image bufferImage = new Image(myDisplay, mainBoard.getBounds());
		GC gcBuffer = new GC(bufferImage);
		
		for (int x=0; x < 6; x++)
			for (int y=0; y < 12; y++)
				drawPiece(playerBoard.getPiece(x,y), gcBuffer);
				//gcBuffer.drawImage(drawPiece(playerBoard.getPiece(x,y)), x*pieceSize, y*pieceSize);
				
		if (b == 0 && gameController.getGameState() instanceof GamePlaying) {
			drawPiece(playerBoard.curPiece[0], gcBuffer);
			drawPiece(playerBoard.curPiece[1], gcBuffer);
		}
		
		return bufferImage;
	}

	
	/**
	 * Draw a single piece to an offscreen buffer
	 * 
	 * 1: determine what type of piece to draw. ie what connections it has.
	 * 2: determine what color the piece is and swap the palette accordingly.
	 * 3: determine what out what frame to draw
	 * 4: Render image to offscreen buffer
	 *  
	 * @param p Piece object to be rendered
	 * @param gcBuffer Offscreen buffer to draw to
	 */
	private static void drawPiece(Piece p, GC gcBuffer) {
	    // Ensure we don't bother attempting to draw EMPTY spots on the board
	    if ( p.getColor() == pieceType.EMPTY)
	    	return;
	    
		int s=0;
		int x,y;
		
		boolean states[]=p.getState();

		/** Convert four booleans to a single integer to calculate the row/column of the piece to draw */
		if (states[0]) // RIGHT
			s += 0x1;	
		if (states[1]) // DOWN
			s += 0x1 << 1;
		if (states[2]) // LEFT
			s += 0x1 << 2;
		if (states[3]) // UP
			s += 0x1 << 3;
		
		// Use s integer to calculate the row and column to use
		x=((s % 4) * pieceSize);
		y=((int)(s / 4) * pieceSize); 
	    
		// Find the frame to use
		y=y + (p.getFrame() * (pieceSize * 4));
	    
	    RGB newColor;
	    switch (p.getColor()) {
	    	case RED:
	    		newColor= new RGB(255,0,0);
	    		break;
	    	case GREEN:
	    		newColor= new RGB(0,255,0);
	    		break;
	    	case BLUE:
	    		newColor= new RGB(0,0,255);
	    		break;
	    	case YELLOW:
	    		newColor= new RGB(255,255,0);
	    		break;
	    	case PURPLE:
	    		newColor= new RGB(255,0,255);
	    		break;
	    	case LT_BLUE:
	    		newColor= new RGB(0,255,255);
	    		break;
	    	case CLEAR:
	    		// TODO: Set transparency again..
	    		newColor= new RGB(128,128,128);
	    		break;
	    	default:
	    		// TODO: Throw an exception because this should never actually happen
	    		newColor=new RGB(0,0,0);
	    		break;
	    }

	    // Swap the Palette with the new one and use it to create an Image we can draw from
	    piecesImage.palette.getRGBs()[128] = newColor;	
	    Image pImage=new Image(myDisplay.getCurrent(), piecesImage);	
	    
	    // Actually copy pixel data from our original Pieces image with an updated palette to the offscreen buffer
	    gcBuffer.drawImage(pImage, x, y, pieceSize, pieceSize, p.getX()*pieceSize, p.getY()*pieceSize, pieceSize, pieceSize);
	    
	    pImage.dispose();
	    
	    /* TODO: Figure out how to disable advancing frames...
	     * Framerate set to like 90ms per frame.. But when you move a piece you want to
	     * redraw the board instantly instead of waiting like 90ms for the timer....
	     * So might need a global flag that is disabled after a timer stops but enabled when
	     * the timer starts..
	     */
	    p.advanceFrame(maxFrames);	    
	}
	
	/**
	 * Setup the timer to draw each frame
	 * 
	 * This function calls mainBoard.redraw after "FPS" milliseconds and then adds another timer event of the same
	 * duration so it never quits drawing frames until the program ends.
	 * 
	 * TODO: Figure out if I have to disable this timer when the window is hidden, not active, or even if nobody is playing the game
	 * 	However, if the all the boards are empty there should be minimal render time as doesn't do anything too computationally intense
	 * 	for empty boards
	 */
	private static void setupAnimation() {
		
		final Runnable animationTimer = new Runnable() {
			public void run() {
				/*
				 * If the gameController tells us we just redraw EVERYTHING. Otherwise just draw the minimun required.
				 */
				if (gameController.forceRedraw) {
					mainShell.redraw();
					gameController.forceRedraw = false;
				}
				else {	
					nextPiece.redraw();
				
					for (int i=0; i < 6; i++) {
						otherBoards[i].redraw();
					}
				
				}
				myDisplay.timerExec(FPS, this);
		      }
		};
		
		final Runnable dropRateTimer = new Runnable() {
			public void run() {
				GameEvent event = new MoveDown(gameController.getMyUserID());
				gameController.push(event);	
				
				myDisplay.timerExec(DROP_RATE, this);
			}
		};
		
		myDisplay.timerExec(FPS, animationTimer);
		myDisplay.timerExec(FPS, dropRateTimer);
	}
			

}
