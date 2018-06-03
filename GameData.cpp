#include "GameData.h"

GameData::GameData()
{
    Pnum=0;
    GameOn=false;
    UpdateNicks=false;
    UpdateScores=false;
    // Load Settings
    settings=new Settings(this);
    // Load Images
    images=new Images(this);
    // Setup Network
    network=new Network(this);
    
    // Setup the scores
    scores=new Scores(this);
    
    // Setup new empty boards
    for (int i=0;i < 6;++i) {
        boards[i]=new Board(this,i);
    }
    return;
}

GameData::~GameData()
{
    // Cleanup all the boards
    for (int i=0;i < 6;++i) {
        delete boards[i];
    } 
    // Clear settings
    delete settings;
    // Cleanup Images
    delete images;
    // Cleanup network
    delete network;
    return;
}

