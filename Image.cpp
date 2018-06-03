#include "Image.h"
#include "GameData.h"

Images::Images(GameData* GD) 
{
	Gamedata = GD;
    LargePs=NULL; SmallPs=NULL; SingleP=NULL; SSingleP=NULL; LargeCs=NULL; SmallCs=NULL;
    char *filename=new char[11];
    char *sfilename=new char[12];
    char *specname=new char[13];
    sprintf(filename,"Pieces.bmp%c",'\0');
    sprintf(sfilename,"SPieces.bmp%c",'\0');
    sprintf(specname,"Specials.bmp%c",'\0');
    
    // Loads Pieces.bmp from the game dir or if a Theme is set it loads it from 
    // the specified Theme folder
    if (Gamedata->settings->ThemePath != NULL) {
        delete [] filename; delete [] sfilename; delete [] specname;
        filename=new char[strlen(Gamedata->settings->ThemePath)+11];
        sfilename=new char[strlen(Gamedata->settings->ThemePath)+12];
        specname=new char[strlen(Gamedata->settings->ThemePath)+13];
        if (Gamedata->settings->ThemePath[strlen(Gamedata->settings->ThemePath)-1] != '\\' 
            && Gamedata->settings->ThemePath[strlen(Gamedata->settings->ThemePath)-1] != '/') {
            sprintf(filename,"%s\\Pieces.bmp%c",Gamedata->settings->ThemePath,'\0');
            sprintf(sfilename,"%s\\SPieces.bmp%c",Gamedata->settings->ThemePath,'\0');
            sprintf(specname,"%s\\Specials.bmp%c",Gamedata->settings->ThemePath,'\0');
        }
        else {
            sprintf(filename,"%sPieces.bmp%c",Gamedata->settings->ThemePath,'\0');
            sprintf(sfilename,"%sSPieces.bmp%c",Gamedata->settings->ThemePath,'\0');
            sprintf(specname,"%sSpecials.bmp%c",Gamedata->settings->ThemePath,'\0');
        }
    }
	SDL_Surface *piece=SDL_LoadBMP(filename);
	SDL_Surface *spiece=SDL_LoadBMP(sfilename);
	SDL_Surface *specials=SDL_LoadBMP(specname);
    delete [] filename; delete [] sfilename; delete [] specname;
    if (piece == NULL) {
        Error("Can't load Pieces.bmp!");
        exit(0);
    }
    if (specials == NULL) {
        Error("Can't load Specials.bmp!");
        exit(0);
    }
	if (piece->format->BitsPerPixel != 8) {
	   Error("Invalid Pieces.bmp file");
	   exit(0);
    }
    // Create the Large and Small Pieces
    CreateLargePs(piece);
    CreateSmallPs(spiece);
    
    // Create the Large and Small Clear Piecese
    CreateLargeCs();
    CreateSmallCs();
    
    // Setup the Specials
    CreateSpecials(specials);
    
	if (spiece != NULL)
        SDL_FreeSurface(spiece);
	SDL_FreeSurface(piece);
	SDL_FreeSurface(specials);
	return;
}

Images::~Images()
{
    if (LargePs != NULL)
        SDL_FreeSurface(LargePs);
    if (SmallPs != NULL)
        SDL_FreeSurface(SmallPs);
    if (SingleP != NULL)
        SDL_FreeSurface(SingleP);
    if (SSingleP != NULL)
        SDL_FreeSurface(SSingleP);
    if (LargeCs != NULL)
        SDL_FreeSurface(LargeCs);
    if (SmallCs != NULL)
        SDL_FreeSurface(SmallCs);
    return;
}

void Images::CreateLargePs(SDL_Surface *piece)
{
	int psize=piece->w/4;  // The size of the piece
	Frames=piece->h / (psize*4);   // The number of frames
	
	// This is the adjusted size of the piece based on the users resolution
	int newpsize=int(((double(Gamedata->settings->X) / 640) * 30)+.5);
	
	// Create a LargePs, SmallPs, LCear & SClear surfaces of the screens format
	
	// Adjust the piece to fit the size based on resolution
	LargePs=ResizeImageToFit(piece,4*newpsize,4*newpsize*Frames); 
    // Create a single Large piece surface for usage when rendering color
	SingleP=SDL_CreateRGBSurface(SDL_SWSURFACE, newpsize, newpsize, 8, LargePs->format->Rmask, LargePs->format->Gmask, LargePs->format->Bmask, LargePs->format->Amask);
    SDL_SetPalette(SingleP, SDL_LOGPAL, LargePs->format->palette->colors, 0, 256);
    return;
}

void Images::CreateSmallPs(SDL_Surface *spiece)
{
    int psize;
    int newpsize=SingleP->w/2;
    if (spiece != NULL) {
        psize=spiece->w /4;
        SFrames=spiece->h / (psize * 4);
    }
    else {
        psize=LargePs->w /4;
        SFrames=LargePs->h /(psize * 4);
    }
    // If SPieces.bmp doesn't exist create one from Pieces.bmp
    if (spiece == NULL) {
        SmallPs=ResizeImageToFit(LargePs, LargePs->w/2, LargePs->h/2);
        SDL_SetPalette(SmallPs,SDL_LOGPAL,LargePs->format->palette->colors,0,256);
        }
    else {
        SmallPs=ResizeImageToFit(spiece,SingleP->w*2, SingleP->h*2*SFrames);
        SDL_SetPalette(SmallPs,SDL_LOGPAL,spiece->format->palette->colors,0,256);
    }
    // Create a single Small piece surface for usage when rending color
    SSingleP=SDL_CreateRGBSurface(SDL_SWSURFACE, newpsize, newpsize, 8, SmallPs->format->Rmask, SmallPs->format->Gmask, SmallPs->format->Bmask, SmallPs->format->Amask);  
    SDL_SetPalette(SSingleP, SDL_LOGPAL, SmallPs->format->palette->colors, 0, 256);
    return;
}

void Images::CreateLargeCs()
{
    SDL_Rect src,dest;
    // First create a surface 
    LargeCs=SDL_CreateRGBSurface(SDL_SWSURFACE,SingleP->w,SingleP->h *  Frames, 8, LargePs->format->Rmask, LargePs->format->Gmask, LargePs->format->Bmask, LargePs->format->Amask);
    SDL_SetPalette(LargeCs, SDL_LOGPAL, LargePs->format->palette->colors, 0, 256);
    // Now blit the frames into place
    dest.w=src.w=SingleP->w; dest.h=src.h=SingleP->h;
    dest.x=src.x=0; dest.y=src.y=0;
    for (int i=0; i < Frames;++i) {
        SDL_BlitSurface(LargePs,&src,LargeCs,&dest);
        src.y=src.y+SingleP->h*4;
        dest.y=dest.y+SingleP->h;
    }
    // Now update the pixel information   
    SDL_Color* cols=LargeCs->format->palette->colors;
    for (int x=0; x < LargeCs->w;++x)
        for (int y=0; y < LargeCs->h;++y)
            for (int i=128; i < 255;++i)
                if (getpixel(LargeCs,x,y) == SDL_MapRGB(LargeCs->format, cols[i].r,cols[i].g,cols[i].b))
                    putpixel(LargeCs,x,y,SDL_MapRGB(LargeCs->format,cols[127].r,cols[127].g,cols[127].b));

    return;
}

void Images::CreateSmallCs()
{
    SDL_Rect src,dest;
    // First create a surface 
    SmallCs=SDL_CreateRGBSurface(SDL_SWSURFACE,SSingleP->w,SingleP->h *  SFrames, 8, SmallPs->format->Rmask, SmallPs->format->Gmask, SmallPs->format->Bmask, SmallPs->format->Amask);
    SDL_SetPalette(SmallCs, SDL_LOGPAL, SmallPs->format->palette->colors, 0, 256);
    // Now blit the frames into place
    dest.w=src.w=SSingleP->w; dest.h=src.h=SSingleP->h;
    dest.x=src.x=0; dest.y=src.y=0;
    for (int i=0; i < SFrames;++i) {
        SDL_BlitSurface(SmallPs,&src,SmallCs,&dest);
        src.y=src.y+SSingleP->h*4;
        dest.y=dest.y+SSingleP->h;
    }
    // Now update the pixel information  
    SDL_Color* cols=SmallCs->format->palette->colors;
    for (int x=0; x < SmallCs->w;++x)
        for (int y=0; y < SmallCs->h;++y)
            for (int i=128; i < 255;++i)
                if (getpixel(SmallCs,x,y) == SDL_MapRGB(SmallCs->format, cols[i].r,cols[i].g,cols[i].b))
                    putpixel(SmallCs,x,y,SDL_MapRGB(SmallCs->format,cols[127].r,cols[127].g,cols[127].b));

    return;
}

void Images::CreateSpecials(SDL_Surface *specials)
{
    int x=int(double(Gamedata->settings->X)/640 * 16 * 7);
    int y=int(double(Gamedata->settings->Y)/480 * 16);
    Specials=ResizeImageToFit(specials,x,y);
    SDL_SetPalette(Specials,SDL_LOGPAL,specials->format->palette->colors,0,256);
    SDL_Color *trans=Specials->format->palette->colors; // Set the last color to the color key
    SDL_SetColorKey(Specials, SDL_SRCCOLORKEY, SDL_MapRGB(Specials->format,trans[255].r,trans[255].g,trans[255].b));
    return;
}
