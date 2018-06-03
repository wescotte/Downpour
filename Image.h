#ifndef Image_H
#define Image_H

#include "SDL.h"
#include "System.h"
#include "Settings.h"
#include "ImageManip.h"

#include <string>
#include <stdio.h>

class GameData;

class Images {
public:
	Images(GameData *GD);
	~Images();
	void CreateLargePs(SDL_Surface *piece);
	void CreateSmallPs(SDL_Surface *spiece);
	void CreateLargeCs();
	void CreateSmallCs();
	void CreateSpecials(SDL_Surface *specials);
	
	SDL_Surface *LargePs, *SmallPs, *SingleP, *SSingleP;
    SDL_Surface *LargeCs, *SmallCs, *Specials;	
	int Frames;
	int SFrames;
	GameData* Gamedata;
};

#endif
