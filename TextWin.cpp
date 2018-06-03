#include "TextWin.h"

// *****************************************************************************
// TEXT WINDOW FUNCTIONS
// *****************************************************************************
TxtWin::TxtWin(int xpos, int ypos, int width, int height,int buffersize,int id,Font* font,SDL_Surface* background,bool textinput) {
    XPOS=xpos;
    YPOS=ypos;
    WIDTH=width;
    HEIGHT=height;
    BSIZE=buffersize;
    ID=id;
    FONT=font;
    BACKGROUND=background;
    FHEIGHT=TTF_FontHeight(FONT->GetFont());
    DRAW=true;
    DRAWCUR=false;
    ACTIVE=false;
    DRAG=false;
    POSITION=0;
    MAXPOS=0;
    TEXTINPUT=textinput;

    SetButtons();
 	SetScrollBar();
    return;
/*
Might want to clean this up so that if you want text input included with the window then
YPOS=YPOS-FHEIGHT <-- so you have space for 1 more line of text where the input would be..
Also when scrollbars come into place might wanna adjust the width
*/

}

TxtWin::~TxtWin() {
    for (int i=0;i < TEXT.size();i++)
        delete TEXT[i];
    TEXT.clear();

    delete UPARROW;
    delete DOWNARROW;
    delete TEXTINPUTB;

    return;
}

int TxtWin::GetID() {
    return ID;
}

void TxtWin::AddText(char* text) {
// Also might want to include filters for the text for style/color changes
    if (TEXT.size() >= BSIZE) {
    	MAXPOS=MAXPOS-(TEXT[0]->GetHeight() / FHEIGHT);
    	delete TEXT[0];
        TEXT.erase(TEXT.begin());
    }
    Text* temp=new Text(text,-1,0,0);
    TEXT.insert(TEXT.end(), temp);   
    temp->RenderTextWrap(FONT->GetFont(),WIDTH);
 	if (POSITION == MAXPOS)
 		POSITION=POSITION+(temp->GetHeight() / FHEIGHT);
 	MAXPOS=MAXPOS+(temp->GetHeight() / FHEIGHT);
 	SetScrollBar();
 	DRAW=true;
    return;
}

void TxtWin::AddText(Text* text) {
    if (TEXT.size() >= BSIZE) {
    	MAXPOS=MAXPOS-(TEXT[0]->GetHeight() / FHEIGHT);
    	delete TEXT[0];
        TEXT.erase(TEXT.begin());
    }
    TEXT.insert(TEXT.end(),text);
    text->RenderTextWrap(FONT->GetFont(),WIDTH);
 	if (POSITION == MAXPOS)
 		POSITION=POSITION+(text->GetHeight() / FHEIGHT);
 	MAXPOS=MAXPOS+(text->GetHeight() / FHEIGHT);
 	SetScrollBar();
 	DRAW=true;
}

SDL_Rect* TxtWin::Display(SDL_Surface* dest) {
    SDL_Rect* Rect=new SDL_Rect;
	if (DRAW) {
		Rect->x=XPOS;Rect->y=YPOS;Rect->w=WIDTH;Rect->h=HEIGHT;
		SDL_BlitSurface(BACKGROUND,Rect,dest,Rect);
		Rect->w=Rect->w+16;
		Rect->h=Rect->h+16;
		DisplayText(dest);	
		
 		SDL_Rect temp;
   		temp.x=XPOS+WIDTH; temp.y=YPOS+16; temp.w=16; temp.h=HEIGHT-32;
		SDL_BlitSurface(BACKGROUND,&temp,dest,&temp);

   		SCROLLBAR->Display(dest);
		UPARROW->Display(dest);
		DOWNARROW->Display(dest);	
		DRAW=false;
	}
    if (TEXTINPUT) {
        SDL_Rect* temp=TEXTINPUTB->Display(dest);
        if (temp != NULL)
            SDL_UpdateRect(dest,temp->x,temp->y,temp->w,temp->h);      
    }
    return Rect;
}

void TxtWin::DisplayText(SDL_Surface* dsurface) {
	if (MAXPOS == 0)
		return;
		
	int space=HEIGHT/FHEIGHT;
	int pos=0;
	int startp=0;
	SDL_Rect src,dest;
	dest.x=XPOS; dest.y=YPOS+HEIGHT;
	// Find where to start displaying from..
	for (int i=0;i < TEXT.size();i++) {
		pos=pos+ (TEXT[i]->GetHeight() / FHEIGHT);
		if (pos >= POSITION) {
			startp=i;
			break;
		}
	}
	// Display the first line
	src.x=0; src.y=0; src.w=TEXT[startp]->GetWidth(); src.h=(pos - POSITION) * FHEIGHT; 
	dest.w=src.w; dest.h=src.h;
	SDL_BlitSurface(TEXT[startp]->RTEXT,&src,dsurface,&dest);
	space=space-(src.h/FHEIGHT);

	while (space > 0) {
		if (startp < 0)	// Check to make sure there is even text to display
			break;
		// Checks if there is space to display the entire line of text
		if (space - (TEXT[startp]->GetHeight() / FHEIGHT) < 0)
			break;
		dest.y=dest.y-TEXT[startp]->GetHeight(); dest.h=TEXT[startp]->GetHeight();
		dest.w=TEXT[startp]->GetWidth();
		src.x=0; src.y=0; src.h=dest.h; src.w=dest.w;
		SDL_BlitSurface(TEXT[startp]->RTEXT,NULL,dsurface,&dest);
		startp--;
		space=space-( src.h/FHEIGHT);
	}
	
	// If we get here and space still exists well display what we can!
	if (space > 0 && startp >= 0) {
		src.y=TEXT[startp]->GetHeight() - (space * FHEIGHT);
  		src.h=TEXT[startp]->GetHeight() - src.y;
 		src.x=0; src.w=TEXT[startp]->GetWidth();
   		dest.y=dest.y-src.h; dest.w=src.w; dest.h=src.h;
     	SDL_BlitSurface(TEXT[startp]->RTEXT,&src,dsurface,&dest); 
	}
}

bool TxtWin::ProcessEvent(SDL_Event* event) {
    bool valid=false;
	int Page = HEIGHT / FHEIGHT;
	if (event->type == SDL_KEYDOWN && ACTIVE) {
		switch (event->key.keysym.sym) {
			case SDLK_UP:
				POSITION--;
				if (POSITION < Page)
					POSITION++;
				SetScrollBar();
				DRAW=true;
				break;
			case SDLK_DOWN:
				POSITION++;
				if (POSITION > MAXPOS)
					POSITION = MAXPOS;
				SetScrollBar();
				DRAW=true;
				break;
			case SDLK_PAGEUP:
				POSITION=POSITION - Page;
				if (POSITION < Page)
						if (MAXPOS < Page)
							POSITION = MAXPOS;
						else
							POSITION = Page;
				SetScrollBar();
				DRAW=true;
				break;
    		case SDLK_PAGEDOWN:	
				POSITION=POSITION + Page;
				if (POSITION > MAXPOS)
					POSITION = MAXPOS;
				SetScrollBar();
				DRAW=true;
				break;		
			default:
				if (TEXTINPUT && TEXTINPUTB != NULL && ACTIVE) {
					TEXTINPUTB->ProcessEvent(event);
                    SetScrollBar();	
				}
				break;
		}
	}
	if (event->motion.type == SDL_MOUSEMOTION && DRAG) {
		int y;
		SDL_GetRelativeMouseState(NULL, &y);
		if (y != 0) {
			int maxy=YPOS+HEIGHT-16-SCROLLBAR->GetHeight();
			int miny=YPOS + 16;
			y=y+SCROLLBAR->GetY();
			if (y > maxy)
				y = maxy;
			if (y < miny)
				y = miny;
			double NumPages = double(MAXPOS) / Page;
			int barsize = int((HEIGHT -32) / NumPages);
			SCROLLBAR->ChangeCords(SCROLLBAR->GetX(), y);
			POSITION = ((SCROLLBAR->GetY() - barsize + 16)* MAXPOS) / (HEIGHT - 32);
			if (y == maxy)
				POSITION = MAXPOS;
			if (y == miny)
				POSITION = Page;
			if (POSITION < Page)
				POSITION = Page;
			if (POSITION > MAXPOS)
				POSITION = MAXPOS;
			DRAW=true;
		}
	}
	if (DRAG && event->button.type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
		DRAG = false;
		SetScrollBar();
		DRAW=true;
		SDL_GetRelativeMouseState(NULL, NULL);
	}

    if (UPARROW->ProcessEvent(event)) {
        POSITION--;
        if (POSITION < Page)
            POSITION++;
        SetScrollBar();
		DRAW=true;
    }
    if (DOWNARROW->ProcessEvent(event)) {
        POSITION++;
        if (POSITION > MAXPOS)
            POSITION = MAXPOS;
        SetScrollBar();
        DRAW=true;
    }
	if (SCROLLBAR->ProcessEvent(event)) {
        DRAG=true;
		SDL_GetRelativeMouseState(NULL, NULL);
	}	
    if (event->button.type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (event->button.x >= TBX && event->button.x <= TBX+TBW)
            if (event->button.y >= TBY && event->button.y <= TBY+TBH) {
                TEXTINPUTB->Activate();
                valid=true;
            }
            else
                TEXTINPUTB->Deactivate();
    }  
	if (event->button.type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        if (event->button.x >= XPOS && event->button.x <= XPOS+WIDTH)
            if (event->button.y >= YPOS && event->button.y <= YPOS+HEIGHT) 
                valid=true;
    }
    return valid;
}

void TxtWin::SetScrollBar() {
	int barsize;
	int height=HEIGHT-32; // make space for the up/down arrow button
	int Page = HEIGHT / FHEIGHT;
	double NumPages = double(MAXPOS) / Page;

	if (NumPages > 1.0)
		barsize = int(height / NumPages);
	else 
		barsize = height;

	double Y;
	if (MAXPOS != 0)
		 Y = (double(height) / MAXPOS) * double(POSITION);
	else Y=barsize;
	
	SCROLLBAR->ChangeCords(SCROLLBAR->GetX(), YPOS+int(Y)-barsize+16);
	SDL_Surface* screen=SDL_GetVideoSurface();

	SDL_Surface *button=SDL_CreateRGBSurface(SDL_SWSURFACE, 16, barsize, 32,screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
 	SDL_FillRect(button, NULL, SDL_MapRGB(screen->format, 128, 128, 128));
   	SCROLLBAR->ChangeSurface(button);
	
    return;
}

void TxtWin::SetButtons() {
// Gonna need this changed in the future
	SCROLLBAR=new Button(0,Button::scrollbar,XPOS+WIDTH,YPOS+16,0,0);
    UPARROW=new Button(0,Button::pushbutton,XPOS+WIDTH,YPOS,"uparrow.bmp");
	DOWNARROW=new Button(1,Button::pushbutton,XPOS+WIDTH,YPOS+HEIGHT-16,"dwarrow.bmp");
    TEXTINPUTB=NULL;
    
    if (TEXTINPUT) {
        TBX=XPOS; TBY=YPOS+HEIGHT; TBW=WIDTH; TBH=FONT->GetHeight();
        TEXTINPUTB=new Button(ID,Button::textinput,XPOS,(YPOS+HEIGHT),"",TBX,TBH);     
        TEXTINPUTB->RenderText(FONT);
        TEXTINPUTB->SetBG(BACKGROUND);
        TEXTINPUTB->SetFont(FONT);
    }
    return;
}

void TxtWin::Activate() {
	ACTIVE=true;
	return;
}

void TxtWin::Deactivate() {
	ACTIVE=false;
	return;
}

bool TxtWin::Active() {
    return ACTIVE;
}
void TxtWin::Draw(bool draw) {
	DRAW=draw;
    SCROLLBAR->Draw(true);
    UPARROW->Draw(true);
	DOWNARROW->Draw(true);
    if (TEXTINPUT)
        TEXTINPUTB->Draw(true);
	return;
}

int TxtWin::GetSize() {
    return TEXT.size();
}

char* TxtWin::GetText(int pos) {
    return TEXT[pos]->GetText();
}

char* TxtWin::GetTextInput() {
    return TEXTINPUTB->GetText();
}

void TxtWin::ChangeTextBText(char* newtext) {
    TEXTINPUTB->ChangeText(newtext);
    TEXTINPUTB->RenderText(FONT);
    DRAW=true;
    return;
}

void TxtWin::ChangeTextInputCoords(int x,int y,int w,int h=-1) {
    TBX=x; TBY=y; TBW=w;
    if (h != -1)
        TBH=h;
    if (TEXTINPUTB != NULL)
        TEXTINPUTB->ChangeCords(TBX,TBY,TBW,TBH);
    return;
}
