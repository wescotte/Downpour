#ifndef GameData_H
#define GameData_H

#include "Board.h"
#include "Image.h"
#include "Network.h"
#include "Settings.h"
#include "Piece.h"
#include "Scores.h"

class GameData {
public:
    GameData();
    ~GameData();
    
	Board *boards[6];
	Settings *settings;
	Images *images;
	Network *network;
    Scores *scores;
    
    int Pnum;   // Your Player Number
    bool GameOn;
    bool UpdateNicks; // When this is true somebody changed their nick
    bool UpdateScores; // WHen this is true the high scores have been  updated
    vector <char*> ChatMSGS;
    vector <char*> StatusMSGS;
};

#endif

