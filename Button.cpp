#include "Button.h"

// *****************************************************************************
// BUTTON FUNCTIONS
// *****************************************************************************

Button::Button(int id, int type, int x, int y,int w, int h) {
    ID=id;
    TYPE=type;
    X=x; Y=y;
    IMAGE=NULL;
    W=w;
    H=h;
    DRAW=true;
    ACTIVE=false;
    DRAWCUR=false;
    TEXT=NULL;
    FONT=NULL;
    if (TYPE == Button::textinput) {
    	CURSOR.ChangeDelay(500);
    	TEXT = new Text("\0",ID,X, Y);	
        SetCursor();
    }
    return;
}

Button::Button(int id, int type, int x, int y, SDL_Surface* image,int w, int h) {
    W=0;H=0;
    ID=id;
    TYPE=type;
    X=x; Y=y;
    IMAGE=image;
    if (w != -1)
        W=w;
    else
        if (IMAGE != NULL) 
            W=IMAGE->w;
    if (h != -1)
        H=h;
    else 
        if (IMAGE != NULL)
            H=IMAGE->h;
    TEXT=NULL;
    FONT=NULL;
    DRAW=true;
    ACTIVE=false;
    DRAWCUR=false;
    return;
}

Button::Button(int id, int type, int x, int y, char* filename,int w, int h) {
    ID=id;
    TYPE=type;
    X=x; Y=y;
    TEXT=NULL;
    FONT=NULL;
    IMAGE=NULL;
    switch (TYPE) {
        case textinput: {
            CURSOR.ChangeDelay(500);
            TEXT = new Text(filename,ID,X, Y);
            if (w != -1)
                W=w;
            if (h != -1)
                H=h;
            SetCursor();
            break;
        }
        case text: {
            TEXT=new Text(filename,id,X,Y);
            break;
        }
        default: {
            IMAGE=SDL_LoadBMP(filename);
            if (IMAGE != NULL) {
                if (w != -1)
                    W=w;
                else 
                    W=IMAGE->w;
                if (h != -1)
                    H=h;
                else 
                    H=IMAGE->h;
            }
            else {
                W=0; H=0;
            }
        	break;
    	}
	}
    DRAW=true;
    ACTIVE=false;
    DRAWCUR=false;
    return;
}

Button::~Button() {
    if (IMAGE != NULL)
        SDL_FreeSurface(IMAGE);
	if (TEXT != NULL)
		delete TEXT;
    return;
}

SDL_Rect* Button::Display(SDL_Surface* dest)
{
    SDL_Rect* Rect;
    if (CURSOR.Active()) {
        if (CURSOR.Ready()) {
            FlipCursor();
            if (!DRAW)
                ShowCursor();
        }
    }
    if (!DRAW) // No need to draw it!
        return NULL;

    switch (TYPE) {
        case pushbutton:
            Rect=DisplayPushButton(dest);
        break;
        case text:
            Rect=DisplayText(dest);
        break;
        case scrollbar:
            Rect=DisplayScrollBar(dest);
        break;
        case textinput:
        	Rect=DisplayTextInput(dest);
        break;
    }
    return Rect;
}

SDL_Rect* Button::DisplayPushButton(SDL_Surface* dest) {
    if (IMAGE == NULL)
        return NULL;

    SDL_Rect* Rect = new SDL_Rect;
    Rect->x=X; Rect->y=Y; Rect->w=W; Rect->h=H;
    SDL_BlitSurface(IMAGE,NULL,dest,Rect);
    DRAW=false;
    return Rect;
}

SDL_Rect* Button::DisplayText(SDL_Surface* dest) {
    SDL_Rect* Rect = new SDL_Rect;
    Rect->x=X; Rect->y=Y; Rect->w=TEXT->GetWidth(); Rect->h=TEXT->GetHeight();
    if (TEXT->GetText()[0]='\0') 
        SDL_BlitSurface(BG,Rect,dest,Rect);
    else
    TEXT->Display(dest);
    DRAW=false;
    return Rect;
}

SDL_Rect* Button::DisplayScrollBar(SDL_Surface* dest) {
    SDL_Rect* Rect = new SDL_Rect;
    Rect->x=X; Rect->y=Y; Rect->w=W; Rect->h=H;
    SDL_BlitSurface(IMAGE,NULL,dest,Rect);
    DRAW=false;
    return Rect;
}

SDL_Rect* Button::DisplayTextInput(SDL_Surface* dest) {
    SDL_Rect* Rect = new SDL_Rect;
    Rect->x=X; Rect->y=Y; Rect->w=W; Rect->h=H;
    if (BG != NULL)
        SDL_BlitSurface(BG,Rect,dest,Rect);

    if (TEXT != NULL)
    	TEXT->Display(dest);

    if (CURSOR.Active())
        ShowCursor();

    DRAW=false;
    return Rect;
}

void Button::ChangeSurface(SDL_Surface* image) {
	if (IMAGE != NULL)
    	SDL_FreeSurface(IMAGE);

    if (image != NULL) {
        IMAGE=image;
        W=IMAGE->w;
        H=IMAGE->h;
        DRAW=true;
    }
    else {
        IMAGE=NULL;
        W=0;
        H=0;
        DRAW=false;
    }
    return;
}

void Button::ChangeSurface(char* filename) {
	if (IMAGE != NULL)
    	SDL_FreeSurface(IMAGE);

    IMAGE=SDL_LoadBMP(filename);

    if (IMAGE != NULL) {
        W=IMAGE->w;
        H=IMAGE->h;
        DRAW=true;
    }
    else {
        W=0;
        H=0;
        DRAW=false;
    }
    return;
}

void Button::ChangeCords(int x, int y,int w, int h) {
	X=x; Y=y;
	if (w != -1)
        W=w;
    if (h != -1)
        H=h;
	DRAW=true;
	if (TYPE == textinput) {
	   TEXT->ChangeCords(X,Y);
	}
	return;
}

void Button::ChangeText(char* text) {
	TEXT->ChangeText(text);
	if (FONT != NULL)
	   TEXT->RenderText(FONT->GetFont());
    DRAW=true;
}

void Button::Activate() {
    if (TYPE == textinput)
        CURSOR.Activate();
	ACTIVE=true;
	return;
}

void Button::Deactivate() {
	ACTIVE=false;
	if (TYPE == textinput) {
	   CURSOR.Deactivate();
	   DRAWCUR=false;
	}
	DRAW=true;
	if (TEXT != NULL)
	   TEXT->Draw(true);
	return;
}

bool Button::Active() {
	return ACTIVE;
}

int Button::GetX() {
	return X;
}

int Button::GetY() {
	return Y;
}

int Button::GetHeight() {
	if (IMAGE != NULL)
		return IMAGE->h;
	else 
		return 0;
}

int Button::GetWidth() {
	if (IMAGE != NULL)
		return IMAGE->w;
	else
		return 0;
}

int Button::GetID() {
	return ID;
}


SDL_Surface* Button::GetSurface()
{
    return IMAGE;
}

char* Button::GetText() {
	return TEXT->GetText();
}

void Button::Draw(bool draw) {
	DRAW=draw;
	return;
}

bool Button::ProcessEvent(SDL_Event* event) {
    bool valid=false;  
    if (event->button.type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) 
    	if (event->button.x >= X && event->button.x <= X+W)
    		if (event->button.y >= Y && event->button.y <= Y+H) 
    			valid=true;
    			
    if (!valid && event->button.type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
        Deactivate();
  			
	if (valid && TYPE == textinput) 
		Activate();
	
	if (CURSOR.Active()) 
		if (event->type == SDL_KEYDOWN) 
			EnterText(event);
		
    return valid;
}

void Button::RenderText(Font* font) {
	if (font != NULL) {
		FONT=font;
		TEXT->RenderText(font->GetFont());
	}
	return;
}

void Button::SetCursor() {
    if (IMAGE != NULL)
        SDL_FreeSurface(IMAGE);
    SDL_Surface* temp=SDL_GetVideoSurface();
    int width=2;
    IMAGE=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, width, H, 32, temp->format->Rmask,  temp->format->Gmask,  temp->format->Bmask,  temp->format->Amask); 
    SDL_FillRect(IMAGE, NULL, SDL_MapRGB(IMAGE->format, 255,0,0)); 
    return;
}

void Button::FlipCursor() {
    static bool flip=false;
    if (flip) {
        flip=false;
        SDL_SetAlpha(IMAGE,SDL_SRCALPHA , 0);
    }
    else {
        flip=true;
        SDL_SetAlpha(IMAGE,SDL_SRCALPHA , 255);
    }
    return;
}

void Button::ShowCursor() {
    SDL_Surface* dest=SDL_GetVideoSurface();
    if (DRAWCUR) {
        SDL_SetAlpha(IMAGE,SDL_SRCALPHA , 255);
        DRAWCUR=false;
    }

    SDL_Rect temp;
    temp.x=TEXT->GetWidth()+X; temp.y=Y; temp.w=IMAGE->w; temp.h=IMAGE->h;
    if (BG != NULL)
    	SDL_BlitSurface(BG,&temp,dest,&temp);
    SDL_BlitSurface(IMAGE,NULL,dest,&temp);
    SDL_UpdateRect(dest,temp.x,temp.y,temp.w,temp.h);
    return;
}

void Button::SetBG(SDL_Surface* bg) {
	BG=bg;
	return;
}

void Button::EnterText(SDL_Event* event) {
	if (CURSOR.Active() == false)
		return;

    int len=strlen(TEXT->GetText());
    char *temp=TEXT->GetText();
    if (isgraph(event->key.keysym.unicode & 0x007F)) {
    	if (TEXT->GetWidth() >= W) 
    		return; // Slightly buggy might want to do a more detailed check
        char *newtext=new char[len+2];
        for (int i=0;i < len;i++)
            newtext[i]=temp[i];
        newtext[len]=char(event->key.keysym.unicode);
        newtext[len+1]='\0';
        TEXT->ChangeText(newtext);
        TEXT->RenderText(FONT->GetFont());
        CURSOR.Deactivate();
        CURSOR.Activate();
        DRAWCUR=true;
        DRAW=true;
        delete [] newtext;
    }
    if (event->type==SDL_KEYDOWN && event->key.keysym.sym == SDLK_SPACE) {
  	if (TEXT->GetWidth() >= W) 
  		return;	// Slightly buggy might want to do a more detailed check
        char *newtext=new char[len+2];
        for (int i=0;i < len;i++)
            newtext[i]=temp[i];
        newtext[len]=' ';
        newtext[len+1]='\0';
        TEXT->ChangeText(newtext);
        TEXT->RenderText(FONT->GetFont());
        CURSOR.Deactivate();
        CURSOR.Activate();
        DRAWCUR=true;
        DRAW=true;
        delete [] newtext;
    }
    if (event->type==SDL_KEYDOWN && event->key.keysym.sym == SDLK_BACKSPACE) {
        if (len != 0) { 
            char *newtext=new char[len];
            for (int i=0;i < len-1;i++)
                newtext[i]=temp[i];
            newtext[len-1]='\0';
            TEXT->ChangeText(newtext);
            TEXT->RenderText(FONT->GetFont());
        CURSOR.Deactivate();
        CURSOR.Activate();
        DRAWCUR=true;
        DRAW=true;
        delete [] newtext;
        }
    }       
    return;
}

void Button::SetFont(Font* font) {
    FONT=font;
    return;
}
