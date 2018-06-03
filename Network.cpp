#include "Network.h"
#include "GameData.h"

Network::Network(GameData *GD)
{
    Gamedata=GD;
    Client=false;
    Server=false;
    
    if(SDLNet_Init()==-1) {
        Error("Problem initializing SDL_Net");
        exit(0);
    }

    SockSet=SDLNet_AllocSocketSet(6);
    for (int i=0;i < 6;++i)
        tcpsock[i]=NULL;
    
    // The delay between checking for network packets
    PacketRate.ChangeDelay(50);    
    return;
}

Network::~Network()
{
    if (Client)
        ClientDiscon();
    if (Server)
        ServerDiscon();
        
    SDLNet_Quit();
    return;
}

void Network::HostGame()
{
	if (Server==true) // If already hosting disconnect.
        ServerDiscon();
	if (Client==true) // If connected elsewhere disconnect.
        ClientDiscon();

	Server=true;
	PacketRate.Activate(); // Allows for PackServ() to be called in TimedEvents() function
	Gamedata->Pnum=0; // Sets the server to use board 0. 

	SetNick(Gamedata->settings->MyNick,Gamedata->Pnum); // Stores your current Nick
	if(SDLNet_ResolveHost(&ip,NULL,Gamedata->settings->port)==-1) {
	   Error("Problem resolving localhost!");
	   return;
    }

	tcpsock[0]=SDLNet_TCP_Open(&ip);

	if(tcpsock[0] == NULL) {
	   Error("Unable to open port for listening");
	   return;
    }
	
	SDLNet_TCP_AddSocket(SockSet,tcpsock[0]);
	SDL_Delay(350);    // If you don't add this the status window gets tossed into the background
	Status("Now hosting a DownPour server");
	// Load the highscores
	Gamedata->scores->LoadScores();
	return;
}

void Network::ClientConnect(char* hostname)
{
	if (Client==true) // If connected elsewhere disconnect.
        ClientDiscon();
	if (Server==true) // If already hosting disconnect.
        ServerDiscon();
    
    char* host=hostname; // So we can modify the hostname of a port is specified. 
    int port=9999;  // Default port
    bool colon=false;
    // If we find a colon then a port is specified
    for (int i=0; i < strlen(hostname);i++)
        if (hostname[i]==':') {
            colon=true; 
            port=i;
    }
    // Parse the hostname and specified port
    if (colon) {
        host=new char[port+1];
        strncat(host,hostname,port);
        char* temp=&host[port+1];
        port=atoi(temp);
    }
    int error=SDLNet_ResolveHost(&ip,host,port);
    
    // If a port was specififed we have some cleanup to do
    if (colon)
        delete [] host;
    
    if (error == -1) {
        Error("Unable to connect to server"); 
		return;
	}
	tcpsock[0]=SDLNet_TCP_Open(&ip);
	if (!tcpsock[0]) {
        Error("Problem connecting to server");
		return;
    }
	SDLNet_TCP_AddSocket(SockSet,tcpsock[0]);
	Client=true;
    PacketRate.Activate();
    
	// On connect Get the Player Number you are from the server.
	SDLNet_TCP_Recv(tcpsock[0],&Gamedata->Pnum,4);
	// Also set your nick (NoName by default) and send it to the server.
	SetNick(Gamedata->settings->MyNick,Gamedata->Pnum);
    SendNick(0,Gamedata->Pnum);
    SendStatus(PJOIN,Gamedata->Pnum);
    SDL_Delay(350);
    Status("Connection established");
    return;
}

void Network::ServerDiscon()
{
	Server = false; // Don't need to listen for packets if not
	// hosting a game.
	for (int i=0;i < 6;++i) {
		SDLNet_TCP_DelSocket(SockSet,tcpsock[i]);  // Done so that
		// no more network traffic is detected on a closed socket.
		SDLNet_TCP_Close(tcpsock[i]); // Was connected.. Disconect and prepare
												// to connect elsewhere.
		tcpsock[i]=NULL;
	}
	PacketRate.Deactivate();   // We no longer have to look for packets
	return;
}

void Network::ClientDiscon()
{
    SendStatus(PLEAVE,Gamedata->Pnum);
	Client = false; // Don't need to listen for packets if not
	// connected to a server. 
	SDLNet_TCP_DelSocket(SockSet,tcpsock[0]);  // Done so that
	// no more network traffic is detected on a closed socket.
	SDLNet_TCP_Close(tcpsock[0]); // Was connected.. Disconect and prepare
												// to connect elsewhere.
	tcpsock[0]=NULL;
	PacketRate.Deactivate();
	return;
}

void Network::PackServ()
{
    // Might want to include a quick connect/disconnect but send a message to the client 
    // letting them know the server is full
	int i=0,result,len;
	Uint32 Pack;
	char* txt;
	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;
    
    // Checking for any packet activity
	if ((SDLNet_CheckSockets(SockSet, 0)) != 0) {
		if(SDLNet_SocketReady(tcpsock[0])) {  // We got a new client so find a spot to put them
			for (i=1;i < 6;i++) {
				if (tcpsock[i] == NULL) {
					tcpsock[i]=SDLNet_TCP_Accept(tcpsock[0]);
					SDLNet_TCP_AddSocket(SockSet,tcpsock[i]);
					// On connect it sends the socket number to the client. (also the player number)
					SDLNet_TCP_Send(tcpsock[i],&i,4); 
					// Then Send all current nicks
					break;
				}
			}
		}
		for (i=1; i < 6;i++) {
			if (tcpsock[i] != NULL && SDLNet_SocketReady(tcpsock[i])) {
				result=SDLNet_TCP_Recv(tcpsock[i], &Pack, 4);

				if (result != 4) {	// Problem close the damn socket!
					SDLNet_TCP_DelSocket(SockSet,tcpsock[i]); // Done so
					// constant network traffic isn't detected.
                    
					SDLNet_TCP_Close(tcpsock[i]);
					tcpsock[i]=NULL;
					// Set so that it doesn't attempt to recive anymore info
					// from the now closed socket.
					Gamedata->boards[i]->ClearBoard();
					Gamedata->boards[i]->ClearSpecials();
					// Clear the board and Specials when somebody leaves
                    SendStatus(PLEAVE,i);
					return;					 
				}
				DecodePacket(Pack,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
				switch (arg1) {
					case SENDBOARD:
						GetBoard(arg2);
						break;
					case SENDPIECE:
						GetPiece(arg2,arg3,arg4,arg5,arg6,arg7,arg8);
						break;
					case SENDSPECIAL:
						GetSpecial(arg2,arg3,arg4,arg5);
						break;
					case SENDNICK:
                        len=CreatePacket(arg3,arg4,arg5,arg6,arg7,arg8,0,0);
						GetNick(arg2,len);
						SendNickToAll(arg2);
						break;
					case GAMESTATUS:
						GetStatus(arg2,arg3);
						break;
					case CHATMSG:
                        len=CreatePacket(arg4,arg5,arg6,arg7,arg8,0,0,0);
				        txt=new char[len];
				        SDLNet_TCP_Recv(Gamedata->network->tcpsock[i],txt,len);
				        SendText(txt,arg2,i);
				        GetText(txt,arg2,arg3);
						break;
                    case SENDHIGHS:
                        SendHighScores(arg2);
                        break;
				}
			}
		}
	}
}

void Network::PackClient()
{
	int result=0,len;
	Uint32 Pack;
	char* txt;
	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;

	if ((SDLNet_CheckSockets(SockSet, 0)) != 0) {
		result=SDLNet_TCP_Recv(tcpsock[0], &Pack, 4);

		if (result != 4) {	// Problem close the damn socket!
			SDLNet_TCP_DelSocket(SockSet,tcpsock[0]);  // Done so that
			// no more network traffic is detected on a closed socket.
			SDLNet_TCP_Close(tcpsock[0]);

			Client=false;	// Set so you can reconnect to another.
			Error("Connection to the server has been lost!");
			return;	// Disconnected so no need to attempt any more with this particular packet.
		}
		DecodePacket(Pack,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
		switch (arg1) {
			case SENDBOARD:
				GetBoard(arg2);
				break;
			case SENDPIECE:
				GetPiece(arg2,arg3,arg4,arg5,arg6,arg7,arg8);
				break;
			case SENDSPECIAL:
				GetSpecial(arg2,arg3,arg4,arg5);
				break;
			case SENDNICK:
                len=CreatePacket(arg3,arg4,arg5,arg6,arg7,arg8,0,0);
				GetNick(arg2,len);
				break;
			case GAMESTATUS:
				GetStatus(arg2,arg3);
				break;
			case CHATMSG:
                len=CreatePacket(arg4,arg5,arg6,arg7,arg8,0,0,0);
				txt=new char[len];
				SDLNet_TCP_Recv(Gamedata->network->tcpsock[0],txt,len);
				GetText(txt,arg2,arg3);
				break;
            case SENDCLEARS:
                len=CreatePacket(arg4,arg5,0,0,0,0,0,0);
                GetClears(arg2,len,arg3);
                break;
            case SENDHIGHS:
                GetHighScores();
                break;
		}
	}
	return;
}

void Network::GetStatus(int status, int pnum)
{
	if (Server) {
		switch (status) {
            case PJOIN:
                SendAllNicks(pnum);
                SendAllBoards(pnum);
            break;
			case OUTOFGAME:
                numplaying--;
                outorder[numplaying]=pnum;
                Gamedata->boards[pnum]->IsPlaying=false;
                if (numplaying == 1) {
                    numplaying--;
                    for (int j=0; j < 6;++j)
                        if (Gamedata->boards[j]->IsPlaying)
                            outorder[numplaying]=j;
                    SendStatus(STOPGAME,0);
                }
                Gamedata->boards[pnum]->FillBoardWClears();
			break;
			case PLEAVE:
            // Close connection to leaving player
                SDLNet_TCP_DelSocket(SockSet,tcpsock[pnum]); 
                SDLNet_TCP_Close(tcpsock[pnum]);
				tcpsock[pnum]=NULL;	
                // Clear the board and Specials when somebody leaves
                Gamedata->boards[pnum]->ClearBoard();
                Gamedata->boards[pnum]->ClearSpecials();
			break;
		}
		SendStatus(status,pnum);
	}
	if (Client) {
		switch (status) {
			case STARTGAME: {
				Gamedata->GameOn=true;
				for (int i=0;i < 6;i++) {
					Gamedata->boards[i]->ClearBoard();
					Gamedata->boards[i]->ClearSpecials();
					Gamedata->boards[i]->Num2Drop=0;
					if (Gamedata->settings->Nicks[i] != NULL)
                        Gamedata->boards[i]->IsPlaying=true;
                    else
                        Gamedata->boards[i]->IsPlaying=false;
                }
				Gamedata->boards[Gamedata->Pnum]->NextPiece();
				Gamedata->settings->DropRate.Activate();
				Gamedata->settings->IncreaseDropRate.Activate();
				break;
			}
			case STOPGAME:
				Gamedata->GameOn=false;
				Gamedata->settings->DropRate.Deactivate();
				Gamedata->settings->IncreaseDropRate.Deactivate();
			break;
			case OUTOFGAME:
				if (pnum != Gamedata->Pnum) 
				    Gamedata->boards[pnum]->FillBoardWClears();
                Gamedata->boards[pnum]->IsPlaying=false;
                break;
			case INPROGRESS:
                break;
			case PLEAVE:
				SetNick(NULL,pnum);
                // Clear the board and Specials when somebody leaves
                Gamedata->boards[pnum]->ClearBoard();
                Gamedata->boards[pnum]->ClearSpecials();
                Gamedata->boards[pnum]->IsPlaying=false;
                break;
            case PJOIN:
                break;
		}
	}
}

void Network::SendStatus(int status,int pnum)
{
	Uint32 Pack;
    Pack=CreatePacket(GAMESTATUS,status,pnum,0,0,0,0,0);
	if (Server) {
		if (status == STARTGAME) {
            numplaying=1; // 1 because the server is playing
            Gamedata->boards[Gamedata->Pnum]->NextPiece();
			Gamedata->GameOn=true;
            Gamedata->settings->DropRate.Activate();
			Gamedata->settings->IncreaseDropRate.Activate();
			for (int i=0;i < 6;i++) {
				Gamedata->boards[i]->ClearBoard();
				Gamedata->boards[i]->ClearSpecials();
				Gamedata->boards[i]->Num2Drop=0;
				if (tcpsock[i] != NULL && i != 0) //  tcpsock[0] is always not null cuz it's listening sock
				    numplaying++;
                outorder[i]=-1;
                if (Gamedata->settings->Nicks[i] != NULL)
                    Gamedata->boards[i]->IsPlaying=true;
                else
                    Gamedata->boards[i]->IsPlaying=false;
            }
		}
		if (status == STOPGAME) {
			Gamedata->GameOn=false;
            Gamedata->settings->DropRate.Deactivate();
            Gamedata->settings->IncreaseDropRate.Deactivate();
            AwardPoints();    
		}
		if (status == PLEAVE) 
			SetNick(NULL,pnum);
			
		if (status == OUTOFGAME) {
			if (pnum == Gamedata->Pnum) {
                    Gamedata->boards[pnum]->FillBoardWClears();
					Gamedata->GameOn=false;		
                    Gamedata->settings->DropRate.Deactivate();
                    Gamedata->settings->IncreaseDropRate.Deactivate();
                    Gamedata->boards[pnum]->IsPlaying=false;
                    numplaying--;
                    outorder[numplaying]=pnum;
                        if (numplaying == 1) {
                            numplaying--;
                            for (int j=0; j < 6;++j)
                                if (Gamedata->boards[j]->IsPlaying) 
                                    outorder[numplaying]=j;
                            SendStatus(STOPGAME,0);
                    }
            }
		}
        if (status == PJOIN) {  
        }
        
		for (int i=1;i < 6;i++) 
			if (tcpsock[i] != NULL) 
				SDLNet_TCP_Send(tcpsock[i],&Pack,4);
	}

	if (Client) {
		Pack=CreatePacket(GAMESTATUS,status,pnum,0,0,0,0,0);
		SDLNet_TCP_Send(tcpsock[0],&Pack,4);
	}
	return;
}

void Network::SetNick(char* nick,int num)
{
    if (nick == NULL) {
        delete [] Gamedata->settings->Nicks[num];
        Gamedata->settings->Nicks[num]=NULL;
        Gamedata->UpdateNicks=true;
        return;
    }
    int len=strlen(nick)+1;
    if (Gamedata->settings->Nicks[num] != NULL) 
        delete [] Gamedata->settings->Nicks[num];    
    Gamedata->settings->Nicks[num]=new char[len];
    strcpy(Gamedata->settings->Nicks[num],nick);
    
    Gamedata->UpdateNicks=true;
    return;
}

void Network::SendNick(int to,int from)
{
	if (Gamedata->settings->Nicks[from] == NULL)
		return;
		
	int len=strlen(Gamedata->settings->Nicks[from])+1;
    int arg3,arg4,arg5,arg6,arg7,arg8,unused;
    DecodePacket(len,arg3,arg4,arg5,arg6,arg7,arg8,unused,unused);
	int temp=CreatePacket(arg4,arg5,arg6,arg7,arg8,0,0,0);
    // Store the length of the nick across 6 x 4bit parts.. Abit overkill for a nicksize but
    // it's all good.
	Uint32 Pack;
	if (Server) {
        if (tcpsock[to] != NULL) {
            Pack=CreatePacket(SENDNICK,from,arg3,arg4,arg5,arg6,arg7,arg8);
            SDLNet_TCP_Send(tcpsock[to],&Pack,4);
            SDLNet_TCP_Send(tcpsock[to],Gamedata->settings->Nicks[from],len);			
        }
	}
	if (Client) {
		Pack=CreatePacket(SENDNICK,from,arg3,arg4,arg5,arg6,arg7,arg8);
		SDLNet_TCP_Send(tcpsock[0],&Pack,4);
		SDLNet_TCP_Send(tcpsock[0],Gamedata->settings->Nicks[from],len);
	}
    return;
}

void Network::SendAllNicks(int to)
{
	for (int i=0; i < 6;i++) 
		SendNick(to,i);
    return;
}

void Network::SendNickToAll(int from)
{
	if (Gamedata->settings->Nicks[from] == NULL)
		return;

	int len=strlen(Gamedata->settings->Nicks[from])+1;
    int arg3,arg4,arg5,arg6,arg7,arg8,unused;
    DecodePacket(len,arg3,arg4,arg5,arg6,arg7,arg8,unused,unused);
    // Store the length of the nick across 6 x 4bit parts.. Abit overkill for a nicksize but
    // it's all good.
	Uint32 Pack;
	if (Server) {
        for (int i=0; i < 6;++i) {
            if (tcpsock[i] != NULL && i != from) {
                Pack=CreatePacket(SENDNICK,from,arg3,arg4,arg5,arg6,arg7,arg8);
                SDLNet_TCP_Send(tcpsock[i],&Pack,4);
                SDLNet_TCP_Send(tcpsock[i],Gamedata->settings->Nicks[from],len);
            }			
        }
	}
	if (Client) {
		Pack=CreatePacket(SENDNICK,from,arg3,arg4,arg5,arg6,arg7,arg8);
		SDLNet_TCP_Send(tcpsock[0],&Pack,4);
		SDLNet_TCP_Send(tcpsock[0],Gamedata->settings->Nicks[from],len);
	}
	return;
}

void Network::GetNick(int from, int len)
{
	char *nick=new char[len];

	if (Server) 
		SDLNet_TCP_Recv(tcpsock[from],nick,len);
    
	if (Client) {
		SDLNet_TCP_Recv(tcpsock[0],nick,len);
	}	

    SetNick(nick,from);
	delete [] nick;
	return;
}

void Network::SendBoard(int to, int from)
{
	// Sends a packet with a SENDBOARD message and arg1 containing the board number being sent. 
	// The client then listens for NEWPIECE messages until a SENDBOARD packet is sent again.
	Uint32 Pack;
	Pack=CreatePacket(SENDBOARD, from,0,0,0,0,0,0);
	SDLNet_TCP_Send(tcpsock[to],&Pack,4);

	for (int x=0;x < 6;x++) {
		for (int y=0;y < 12;y++) {
			if (Gamedata->boards[from]->board[x][y] != NULL) {
				Pack=CreatePacket(SENDPIECE, Gamedata->boards[from]->board[x][y]->color, 
                    Gamedata->boards[from]->board[x][y]->x, 
					Gamedata->boards[from]->board[x][y]->x, 0,0,0,0);
				SDLNet_TCP_Send(tcpsock[to],&Pack,4);
			}
		}
	}
	// Sending SENDBOARD packet again to let them know that's the end of pieces.
	Pack=CreatePacket(SENDBOARD, 0,0,0,0,0,0,0);
	SDLNet_TCP_Send(tcpsock[to],&Pack,4);
	return;
}

void Network::SendAllBoards(int to)
{
	for (int i=0;i < 6;i++) 
		if (i != to) 
			SendBoard(to,i);
	return;
}
    
void Network::GetBoard(int from)
{
	// TO DO: Get bnum passed from the main RecClient/RecServer section!!!
	int x=0,y=0,color=0;
	int unused,header;
	bool fin=false;
	// Server sends SENDBOARD when the board is completed
	Uint32 Pack;
	int temp=0;
	while (!fin) {
		// Getting each piece until the header is SENDBOARD which signals all pieces sent for that board.
		SDLNet_TCP_Recv(tcpsock[0],&Pack,4);
		DecodePacket(Pack, header,color,x,y,unused,unused,unused,unused);
		if (header != SENDBOARD) { 
			Gamedata->boards[from]->board[x][y]=new Piece(color,x,y);
			temp++;
		}
		else
			fin=true;
	}
	// Run a board check to set the correct connections
	// Shouldn't have to check if no pieces were added find out why it crashes when you do!
	if (temp > 0)
	   Gamedata->boards[from]->BoardCheck();
    return;
}
    
void Network::SendPiece(Piece* one, Piece* two, int from)
{
	Uint32 Pack;

	if (Server) 
		// Send to everyone else except the client who sent it originally!
		for (int i=1;i < 6;i++) 
			if (tcpsock[i] != NULL && i != from) {
				Pack=CreatePacket(SENDPIECE,from,one->color,one->x,one->y,two->color,two->x,two->y);
				SDLNet_TCP_Send(tcpsock[i],&Pack,4);
				// Player to update your screen for
			}	
	if (Client) {
		Pack=CreatePacket(SENDPIECE,from,one->color,one->x,one->y,two->color,two->x,two->y);
		SDLNet_TCP_Send(tcpsock[0],&Pack,4);
	}
	return;
}

void Network::GetPiece(int pnum, int c1, int x1, int y1, int c2, int x2, int y2)
{
	if (Server) {
		Gamedata->boards[pnum]->board[x1][y1] = new Piece(c1,x1,y1);
		Gamedata->boards[pnum]->board[x2][y2] = new Piece(c2,x2,y2);

		// Send Piece to all other Clients
		SendPiece(Gamedata->boards[pnum]->board[x1][y1],Gamedata->boards[pnum]->board[x2][y2],pnum);

		Gamedata->boards[pnum]->BoardCheck();
	}
	
	if (Client) {
		Gamedata->boards[pnum]->board[x1][y1] = new Piece(c1,x1,y1);
		Gamedata->boards[pnum]->board[x2][y2] = new Piece(c2,x2,y2);
		Gamedata->boards[pnum]->BoardCheck();
	}
}

void Network::SendSpecial(int to,int from)
{
	Uint32 Pack;
	int randnum=RandNum(0,5);
	int special=Gamedata->boards[from]->specials[0];
	if (Server) {
		for (int i=1;i < 6;i++) 
			if (tcpsock[i] != NULL && i != from) {
				Pack=CreatePacket(SENDSPECIAL, special,from,to,randnum,0,0,0);
				SDLNet_TCP_Send(tcpsock[i],&Pack,4);
			}
		// Might want to verify the user HAS the special in the first place to use.
		// but add at a later date. Since being the server it's assumed you're not cheating in
		// some way!
        Gamedata->boards[from]->UseSpecial(special,to,from,randnum);
	}
	if (Client) {
		Pack=CreatePacket(SENDSPECIAL, special,from,to,randnum,0,0,0);
		SDLNet_TCP_Send(tcpsock[0],&Pack,4);
		Gamedata->boards[to]->UseSpecial(special,to,from,randnum);
	}
}

void Network::GetSpecial(int special, int from, int to, int randnum)
{
// Might want to verify the user HAS the special in the first place to use.
// but add at a later date.
	if (Server) 
		SendSpecial(to,from);
		 
	if (Client)
		Gamedata->boards[to]->UseSpecial(special,to,from,randnum);
    return;
}

void Network::SendClears(int from,int num)
{       
    Uint32 Pack;
    int arg4,arg5,unused;
    int arg3=RandNum(0,6);
    DecodePacket(num,arg4,arg5,unused,unused,unused,unused,unused,unused);
    Pack=CreatePacket(SENDCLEARS,from,arg3,arg4,arg5,0,0,0);
    for (int i=1; i < 6;++i)
        if (tcpsock[i] != NULL) 
            SDLNet_TCP_Send(tcpsock[i],&Pack,4);    
          
    // If the server didn't do the combo then drop the clears on himself
//    if (Gamedata->network->Server)
        GetClears(from,num,arg3);   
    return;
}

void Network::GetClears(int from,int num,int randnum)
{
    for (int i=0; i < 6;++i)
        if (from != i && Gamedata->boards[i]->IsPlaying)
            Gamedata->boards[i]->DropClears(num,randnum);
    return;
}

void Network::SendText(char* msg, int loc,int from)
{
    Uint32 Pack;
    int arg3,arg4,arg5,arg6,arg7,arg8,unused;
    int len=strlen(msg)+1;
    DecodePacket(len,arg4,arg5,arg6,arg7,arg8,unused,unused,unused);
    Pack=CreatePacket(CHATMSG,loc,from,arg4,arg5,arg6,arg7,arg8);
    if (Server) {     
        for (int i=1; i < 6;++i)
            if (tcpsock[i] != NULL) {
                SDLNet_TCP_Send(tcpsock[i],&Pack,4);
				SDLNet_TCP_Send(tcpsock[i],msg,len);
            }
    }
    if (Client) {
        SDLNet_TCP_Send(tcpsock[0],&Pack,4);
        SDLNet_TCP_Send(tcpsock[0],msg,len);
    }
    return;
}

void Network::GetText(char *msg,int loc,int from)
{
    char *temp=new char[strlen(msg) + strlen(Gamedata->settings->Nicks[from]) + 3];
    sprintf(temp,"%s: %s%c",Gamedata->settings->Nicks[from],msg,'\0');
    delete msg;
    msg=temp;
    
    if (loc == MSGINFIELDS) 
        Gamedata->StatusMSGS.insert(Gamedata->StatusMSGS.end(),msg);
    
    if (loc == MSGINCHATRM) 
        Gamedata->ChatMSGS.insert(Gamedata->ChatMSGS.end(),msg);    
    return;
}


void Network::SendHighScores(int to)
{
    Uint32 Pack;
        
    Pack=CreatePacket(SENDHIGHS,0,0,0,0,0,0,0);
    SDLNet_TCP_Send(tcpsock[to],&Pack,4);
    
    int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,unused;
    int len,score; int size=Gamedata->scores->List.size()-1;;
    char *name;
    for (int i=0; i < 10;++i) {
        if (i > size) {
            name=new char[7];
            len=7;
            sprintf(name,"Nobody%c",'\0');
            score=0;
        }
        else {
            len=strlen(Gamedata->scores->List[i]->name)+1;
            name=Gamedata->scores->List[i]->name;
            score=Gamedata->scores->List[i]->score;
        }
        DecodePacket(len,arg1,arg2,arg3,arg4,unused,unused,unused,unused);
        DecodePacket(score,arg5,arg6,arg7,arg8,unused,unused,unused,unused);
        Pack=CreatePacket(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
        SDLNet_TCP_Send(tcpsock[to],&Pack,4);
        SDLNet_TCP_Send(tcpsock[to],name,len);  
        if (i > size) 
            delete [] name;
    }
    return;
}

void Network::GetHighScores()
{
    Uint32 Pack;
    int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;
    int score, len;
    char name[1024];
    Gamedata->scores->Clear();
    for (int i=0; i < 10;++i) {
        SDLNet_TCP_Recv(tcpsock[0],&Pack,4);
        DecodePacket(Pack,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
        len=CreatePacket(arg1,arg2,arg3,arg4,0,0,0,0);
        score=CreatePacket(arg5,arg6,arg7,arg8,0,0,0,0);
        SDLNet_TCP_Recv(tcpsock[0],&name,len);
        Gamedata->scores->AddEntry(name,score);
    }
    Gamedata->UpdateScores=true;
    return;
}

void Network::RequestHighScores()
{
    Uint32 Pack=CreatePacket(SENDHIGHS,Gamedata->Pnum,0,0,0,0,0,0);
    SDLNet_TCP_Send(tcpsock[0],&Pack,4);
    return;
}

void Network::AwardPoints()
{
    if (outorder[0] != -1)
        Gamedata->scores->AddPoints(Gamedata->settings->Nicks[outorder[0]],3);
    if (outorder[1] != -1)
        Gamedata->scores->AddPoints(Gamedata->settings->Nicks[outorder[1]],2);
    if (outorder[2] != -1)
        Gamedata->scores->AddPoints(Gamedata->settings->Nicks[outorder[2]],1);
    
    Gamedata->scores->Sort();
    for (int i=1;i < 6;++i)
        if (tcpsock[i] != NULL)
            SendHighScores(i);
    return;
}

Uint32 CreatePacket(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8)
{
	// NEED TO ADD: Correction for big and little edian!!!

	// Simply combines 8 ints into one unsigned 32bit int.
	// Each argmuent can contain 4bits (0-15)
	Uint32 Pack;
	Pack = arg1 << 0 | arg2 << 4 | arg3 << 8 | arg4 << 12 | arg5 << 16 | arg6 << 20 | arg7 << 24 | arg8 << 28;
	return Pack;
}

void DecodePacket(Uint32 Packet,int &arg1,int &arg2,int &arg3,int &arg4,int &arg5,int &arg6,int &arg7,int &arg8)
{
	// NEED TO ADD: Correction for big and little edian!!!

	// Takes a unsigned 32bit int and splits it into 8 parts
	arg1 = (Packet & 0x0f) >> 0;
	arg2 = (Packet & 0xf0) >> 4;
	arg3 = (Packet & 0xf00) >> 8;
	arg4 = (Packet & 0xf000) >> 12;
	arg5 = (Packet & 0xf0000) >> 16;
	arg6 = (Packet & 0xf00000) >> 20;
	arg7 = (Packet & 0xf000000) >> 24;
	arg8 = (Packet & 0xf0000000) >> 28;
}
