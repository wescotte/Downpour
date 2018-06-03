#ifndef Settings_H
#define Settings_H

#include <stdio.h>
#include "System.h"
#include "Timer.h"

class GameData;

class Settings {
public:
    Settings(GameData* GD);
    ~Settings();
    void SetDefaults();
    void CheckVideo();
    
	char *Nicks[6]; char* MyNick;
	char *ThemePath;
	int X,Y,BBP,FULLSCREEN;
	
	int droprate, floattime, cleardroprate;
	Uint32 increasedroprate;
	Timer FrameRate, DropRate, IncreaseDropRate;
	int port;
	bool classic;
	
	GameData* Gamedata;
};

#endif
