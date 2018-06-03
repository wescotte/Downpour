#include "Settings.h"


Settings::Settings(GameData *GD)
{
    Gamedata = GD;
    SetDefaults();
    
/*    FILE *input = fopen( "DownPour.ini", "r" );
    if (input == NULL) {
        Error("Can't load DownPour.ini! Using default settings");

    }
    else {
        fscanf(input,"%s %d %d %d %d %d",temp,&X,&Y,&BBP,&FULLSCREEN,&port);

    }
    fclose(input);    
*/
    // Perform several checks to make sure the settings are valid
    //CheckVideo();
    

    return;
}

Settings::~Settings()
{
    // Might want to save settings on exit here
    for (int i=0; i < 6;++i)
        if (Nicks[i] != NULL)
            delete [] Nicks[i];
    
    if (ThemePath != NULL)
        delete [] ThemePath;
    
    if (MyNick != NULL)
        delete [] MyNick;
        
    return;
}

void Settings::SetDefaults()
{
    MyNick=new char[7];
    sprintf(MyNick,"NoName%c",'\0');
    
    for (int i=0; i < 6;++i)
        Nicks[i]=NULL;
	
	ThemePath=NULL;
	X=640; Y=480; BBP=32; FULLSCREEN=0;

    droprate=1500; floattime=7500; cleardroprate=3000;
	increasedroprate=InMinutes(5);
	port=9999;
	classic=true;
    // Setup the timers
    FrameRate.ChangeDelay(30); // Set the framerate to 30MS
    DropRate.ChangeDelay(droprate);
    IncreaseDropRate.ChangeDelay(increasedroprate);
	return;
}

void Settings::CheckVideo()
{    
    double temp=double(X) / Y;
    if (temp != double(4) / 3) {
        Error("Invalid screen resolution! Valid are 640x480 800x600 1024x768... Anything 4:3");
        exit(0);
    }
    return;
}
