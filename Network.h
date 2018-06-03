#ifndef Network_H
#define Network_H

#include "SDL_Net.h"
#include "Timer.h"
#include "System.h"
#include "Piece.h"

class GameData;

class Network {
public:
    Network(GameData* GD);
    ~Network();
    void HostGame();
    void ClientConnect(char* hostname);  
     
    void ServerDiscon();
    void ClientDiscon();
    
    void PackServ();
    void PackClient();
    
    void GetStatus(int status, int pnum);
    void SendStatus(int status,int pnum);
    
    void SetNick(char* nick,int num);
    void SendNick(int to,int from);
    void SendAllNicks(int to);
    void SendNickToAll(int from);
    void GetNick(int from, int len);
    
    void SendBoard(int to, int from);
    void SendAllBoards(int to);
    void GetBoard(int from);
    
    void SendPiece(Piece* one, Piece* two, int from=-1);
    void GetPiece(int pnum, int c1, int x1, int y1, int c2, int x2, int y2);
    
    void SendSpecial(int to,int from);
    void GetSpecial(int special, int from, int to, int randnum);  

    void SendClears(int from,int num);
    void GetClears(int from,int num,int randnum);
    
    void SendText(char* msg, int loc,int from);
    void GetText(char *msg,int loc,int from);
    
    void SendHighScores(int to);
    void GetHighScores();
    void RequestHighScores();
    
    void AwardPoints();
    
    bool Server,Client;
    Timer PacketRate;
    
	SDLNet_SocketSet SockSet;
	IPaddress ip;
	int port;
	TCPsocket tcpsock[6];
	int numplaying;
	int outorder[6];
	
	GameData *Gamedata;
};

Uint32 CreatePacket(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8);
void DecodePacket(Uint32 Packet,int &arg1,int &arg2,int &arg3,int &arg4,int &arg5,int &arg6,int &arg7,int &arg8);
// Constants for packet headers to determine what is contained in each packet.

const int CHATMSG = 0;	// Also included is an arg for where the message is sent to.
	const int MSGINFIELDS = 0;
	const int MSGINCHATRM = 1;

const int GAMESTATUS = 1; // Also included is a arg for Starting/Stoping/Lost/Joining game in progress 
	const int STARTGAME = 0;
	const int STOPGAME = 1;
	const int OUTOFGAME = 2;
	const int INPROGRESS = 3;
	const int PLEAVE = 4;
    const int PJOIN = 5;

const int SENDPIECE = 2;
const int SENDSPECIAL = 3;
const int SENDBOARD = 4;
const int SENDNICK = 5;
const int SENDCLEARS = 6;
const int SENDHIGHS = 7;
/*
Packet Specifics
Uint32 Split into 8 parts each 4bits (0-15)

SENDBOARD: Mutiple Packets
	Packet 1: Inform Client a complete game board is being sent to them
		Arg1 - SENDBOARD
		Arg2 - Player Number
	Packet 2-X: Send's all pieces for a specificed board number
		Arg1 - SENDPIECE
		Arg2 - Piece's Color
		Arg3 - Piece's X-cord
		Arg4 - Piece's Y-cord
	Packet 3: Concludes game board info. 
		Arg1 - SENDBOARD
		
SENDPIECE: Can contain a one or two Pieces
	Packet 1: Single Piece case used during a SENDBOARD
		Arg1 - SENDPIECE
		Arg2 - Piece's Color
		Arg2 - Piece's X-cord
		Arg3 - Piece's Y-cord
	Packet 1: Double Piece case used when piece is positioned on game board
		Arg1 - SENDPIECE
		Arg2 - Board Number
		Arg3 - Piece 1's Color
		Arg4 - Piece 1's X-cord
		Arg5 - Piece 1's Y-cord
		Arg6 - Piece 2's Color
		Arg7 - Piece 2's X-cord
		Arg8 - Piece 2's Y-cord

SENDNICK: Send a specificed Player Name multiple packets
	Packet 1: Send player number and length of nick in bytes
		Arg1 - SENDNICK
		Arg2 - Player Number
		Arg3-8 - Nick's Length in bytes
	Packet 2: Actual nick
		Character array of length specified in previous packet


SENDSPECIAL: Send a special to be used
	Packet 1: 
		Arg1 - SENDSPECIAL
		Arg2 - Special
		Arg3 - From which player
		Arg4 - To be used on this player
		Arg5 - Randum Number between 0 and 5

GAMESTATUS: Start/Stop/Go out/In progress info
	Packet 1:
		Arg1 - GAMESTATUS
		Arg2 - STARTGAME/STOPGAME/INPROGRESS/OUTOFGAME    /PLEAVE
		Arg3 -    0     /    0   /     0    /Player Number/Player Number

CHATMSG: Text
	Packet 1:
		Arg1 - CHATMSG
		Arg2 - MSGINFIELDS/MSGINCHATRM
		Arg3 - From Player Number
		Arg4-8 - Size of MSG. Max MSG size just over a million chars :)
		
	Packet 2: Actual Text char array

SENDCLEARS: Used in classic mode. The server determines how many clears to send
                each client based on the combo.
        Arg1 - SENDCLEARS
        Arg2 - From which player
        Arg3 - Random Number between 0 and 5
        Arg4-5 Number of clears to be sent Max being 72 (6*12 or 1 full board)


SENDHIGHS: From client to server
        Arg1: SENDHIGHS

SENDHIGHS: From server to client
        Arg1: SENDHIGHS
        
        followed by 10 packets below        
SENDHIGHS: At the request of a client the server will send the top 10 player names/scores
        Arg1-4 Length of players name
        Arg5-8 Number of Points that player has
        
    Packet 2: Actual Text char array of player's name
*/
#endif
