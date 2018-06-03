#include "Text.h"

// *****************************************************************************
// TEXT FUNCTIONS
// *****************************************************************************
Text::Text(char* text,int id,int x, int y,int r, int g, int b) {
    // Might wanna scan the text for various encoded stuff that suggest style, size etc
    //     examples like %s# for size %b bold etc etc etc.. future use.
    if (text == NULL) {
        TEXT=new char;
        TEXT[0]='\0';
    }
    else {
        int len=strlen(text);
        TEXT=new char[len+1];
        for (int i=0;i<len;i++)
            TEXT[i]=text[i];
        TEXT[len]='\0';
    }
    X=x; Y=y;
    ID=id;
    DRAW=true;
    RTEXT=NULL;
 	COLOR.r=r; COLOR.g=g; COLOR.b=b;
    return;
}

Text::Text(Text* text) {
    this->X=text->X;
    this->Y=text->Y;
    this->ID=text->ID;
    this->DRAW=true;
    this->RTEXT=SDL_ConvertSurface(text->RTEXT,text->RTEXT->format,0);
    this->DRAW=true;
    this->COLOR=text->COLOR;
    return;
}

Text::~Text() {
    if (TEXT != NULL)
        delete [] TEXT;
    if (RTEXT != NULL)
        SDL_FreeSurface(RTEXT);
    return;
}

void Text::ChangeText(char* text) {
    if (TEXT != NULL)
        delete [] TEXT;
    if (text == NULL) {
        TEXT=new char;
        TEXT[0]='\0';
    }
    else {
        int len=strlen(text);
        TEXT=new char[len+1];
        for (int i=0;i < len;i++)
            TEXT[i]=text[i];
        TEXT[len]='\0';
    }
    DRAW=true;
    return;
}

void Text::RenderText(TTF_Font* font) {
	if (RTEXT != NULL)
		SDL_FreeSurface(RTEXT); 
    RTEXT = TTF_RenderText_Blended(font, TEXT, COLOR);
    DRAW=true;
    return;
}

void Text::RenderTextWrap(TTF_Font* font,int width) {
	if (RTEXT != NULL)
		SDL_FreeSurface(RTEXT);
    std::vector <SDL_Surface*> lines;
    int w,h,i;
    TTF_SizeText(font, TEXT, &w, &h);
    // if it fits don't need to wrap
    if (w < width) {
        RenderText(font);
        return;
    }
    int pos=0;
    for (i=0;i < strlen(TEXT)+1;i++) {
        if (FindBreak(TEXT,pos,i,width,font)) {
            lines.insert(lines.end(),RenderText(pos,i-1,font));
            pos=i-1;
        }   
    }
    lines.insert(lines.end(),RenderText(pos,strlen(TEXT),font)); 

    int height=0;
    for (i=0;i < lines.size();i++)
        height=height+lines[i]->h;

    RTEXT=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, width, height, 32, lines[0]->format->Rmask,  lines[0]->format->Gmask,  lines[0]->format->Bmask,  lines[0]->format->Amask);  
    SDL_Rect dest;
    dest.x=0;dest.y=0;dest.w=width;
    for (i=0; i < lines.size();i++) {
        dest.h=lines[i]->h;
			CopySurface(lines[i],NULL,RTEXT,&dest);
        dest.y=dest.y+lines[i]->h; 
        SDL_FreeSurface(lines[i]); 
    }
    lines.clear();
}

bool Text::FindBreak(char* text, int start, int finish, int width,TTF_Font* font)
{
    bool found=false;
    char* temp=new char[finish-start+1];
    for (int i=0;i <finish-start;i++) 
        temp[i]=text[i+start];
        
    temp[finish-start]='\0';
    int w,h;
    TTF_SizeText(font, temp, &w, &h);
    if (w > width)
        found=true;

    return found;
}

SDL_Surface* Text::RenderText(int s, int f, TTF_Font* font) {
    char* temptext=new char[f-s+1];
    for (int i=0;i <=(f-s-1);i++) 
        temptext[i]=TEXT[i+s];

    temptext[f-s]='\0';
//    SDL_Surface* temp=TTF_RenderText_Shaded(font, temptext, forecol,backcol);
    SDL_Surface* temp=TTF_RenderText_Blended(font,temptext,COLOR);
    return temp;
}

SDL_Rect* Text::Display(SDL_Surface* dest) {
    SDL_Rect* Rect=new SDL_Rect;

    if (!DRAW) { // If we don't have to don't draw it!
        delete Rect;
        Rect=NULL;
        return Rect;
    }
    if (RTEXT == NULL) { 
        delete Rect;
        Rect=NULL;
    }
    else {
        Rect->x=X; Rect->y=Y; Rect->w=RTEXT->w; Rect->h=RTEXT->h;
        SDL_BlitSurface(RTEXT,NULL,dest,Rect);
        DRAW=false;
    }
    return Rect;
}

int Text::GetID() {
    return ID;
}

char* Text::GetText() {
	return TEXT;
}

void Text::ChangeCords(int x, int y) {
    X=x; Y=y;
    DRAW=true;
    return;
}

int Text::GetHeight() {
    if (RTEXT != NULL)
        return RTEXT->h;
    else
        return 0;
}

int Text::GetWidth() {
    if (RTEXT != NULL)
        return RTEXT->w;
    else 
        return 0;
}

void Text::SetColor(Uint8 r, Uint8 g, Uint8 b) {
	COLOR.r=r;
	COLOR.g=g;
	COLOR.b=b;
	return;
}

void Text::Draw(bool draw) {
	DRAW=draw;
	return;
}

void Text::CopySurface(SDL_Surface* src,SDL_Rect* Srect,SDL_Surface* dest,SDL_Rect* Drect) {
	int sx, sy, sw, sh;
	int dx, dy, dw, dh;
	if (Srect == NULL) {
		sx=0; sy=0; 
  		sw=src->w; sh=src->h;
	}
	else {
		sx=Srect->x; sy=Srect->y;
  		sw=Srect->w; sh=Srect->h; 
	}
	if (Drect == NULL) {
		dx=0; dy=0; 
  		dw=dest->w; dh=dest->h;
	}
	else {
		dx=Drect->x; dy=Drect->y;
  		dw=Drect->w; dh=Drect->h; 
	}
	SDL_LockSurface(dest);
	for (int x=sx;x < sx+sw;x++) {
		int DY=dy;
		for (int y=sy;y < sy+sh;y++) {
			Uint32 temp=getpixel(src,x,y);
			putpixel(dest,dx,DY,temp);
			DY++;
		}
		dx++;
	}
	SDL_UnlockSurface(dest);
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 Text::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void Text::putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

