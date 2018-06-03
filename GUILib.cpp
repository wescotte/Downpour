#include "GUILib.h"

// *****************************************************************************
// MAIN GUI FUNCTIONS
// *****************************************************************************
GUI::GUI(SDL_Surface* screen) 
{
    SCREEN=screen;
    BACKGROUND=NULL;
    DRAW=true;
    if ( TTF_Init() < 0 ) 
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
    SDL_EnableUNICODE(1);  // Used for keyboard input
	SDL_EnableKeyRepeat(100,100);
    
    // Since a background wasn't specified.. We create one!
    BACKGROUND=SDL_CreateRGBSurface(SDL_HWSURFACE, screen->w, screen->h, screen->format->BitsPerPixel,screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
    SDL_FillRect(BACKGROUND, NULL, SDL_MapRGB(screen->format, 0,0,0)); 
	return;
}

GUI::GUI(SDL_Surface *screen,char *background) 
{
    SCREEN=screen;
    BACKGROUND=SDL_LoadBMP(background);
    if (BACKGROUND == NULL) {
    BACKGROUND=SDL_CreateRGBSurface(SDL_HWSURFACE, screen->w, screen->h, screen->format->BitsPerPixel,screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
    SDL_FillRect(BACKGROUND, NULL, SDL_MapRGB(screen->format, 0,0,0)); 
    }
    DRAW=true;
    if ( TTF_Init() < 0 ) 
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
    SDL_EnableUNICODE(1);  // Used for keyboard input
    SDL_EnableKeyRepeat(100,100);
	return;
}

GUI::GUI(SDL_Surface *screen,SDL_Surface *background) 
{
    SCREEN=screen;
    BACKGROUND=SDL_ConvertSurface(background, screen->format, SDL_SWSURFACE);
    if (background != NULL)
        SDL_FreeSurface(background);
    if (BACKGROUND == NULL) {
    BACKGROUND=SDL_CreateRGBSurface(SDL_HWSURFACE, screen->w, screen->h, screen->format->BitsPerPixel,screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
    SDL_FillRect(BACKGROUND, NULL, SDL_MapRGB(screen->format, 0,0,0)); 
    }
    DRAW=true;
    if ( TTF_Init() < 0 ) 
        fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
    SDL_EnableUNICODE(1);  // Used for keyboard input
    SDL_EnableKeyRepeat(100,100);
    return;
}

GUI::~GUI() 
{
    // Cleanup time boys
    int size,i;
    size=TXTWINS.size();
    for (i=0;i < size;i++)
        delete TXTWINS[i];
    TXTWINS.clear();
    
    size=TEXT.size();
    for (i=0;i < size;i++)
        delete TEXT[i];
    TEXT.clear();
    
    size=BUTTONS.size();
    for (i=0;i < size;i++)
        delete BUTTONS[i];
    BUTTONS.clear();
    
    size=FONTS.size();
    for (i=0;i < size;i++)
        delete FONTS[i];
    FONTS.clear();
    
    size=UPDATEAREA.size();
    for (i=0;i < size;i++)
        delete UPDATEAREA[i];
    UPDATEAREA.clear();

    if (BACKGROUND != NULL)
        SDL_FreeSurface(BACKGROUND);
        
    TTF_Quit();
    return;
}

void GUI::AddTxtWin(int xpos, int ypos, int width, int height,int buffersize,int id,Font* font,bool textinput)
{
    TxtWin *temp=new TxtWin(xpos,ypos,width,height,buffersize,id,font,BACKGROUND,textinput);
    TXTWINS.insert(TXTWINS.begin(), temp);
    return;
}

void GUI::AddTxtToWin(char* text,int id) 
{
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id) {
                TXTWINS[i]->AddText(text);
        }                
    return;
}

void GUI::AddTxtToWin(Text* text,int id) 
{
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id) {
                TXTWINS[i]->AddText(text);
        }
    return;  
}

void GUI::ChangeTxtWinInputCords(int id,int x, int y, int w, int h) 
{
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id)
            TXTWINS[i]->ChangeTextInputCoords(x,y,w,h);
}

void GUI::AddText(char* text,int id,int x, int y,int fontid,int r, int g, int b) 
{
    Text* temp=new Text(text, id,x,y,r,g,b);
    TEXT.insert(TEXT.begin(),temp);

    temp->RenderText(GetFont(fontid));
    return;    
}

void GUI::ChangeText(int id, char* newtext,int fontid) 
{
    for (int i=0;i < TEXT.size();i++)
        if (TEXT[i]->GetID() == id) {
            TEXT[i]->ChangeText(newtext);
            TEXT[i]->RenderText(GetFont(fontid));
        }
    return;
}

void GUI::ChangeTextColor(int id, int fontid, int r, int g, int b)
{
    for (int i=0;i < TEXT.size();i++)
        if (TEXT[i]->GetID() == id) {
            TEXT[i]->SetColor(r,g,b);
            TEXT[i]->RenderText(GetFont(fontid));
        }
    return;  
}

char* GUI::GetText(int id) 
{
    char* temp=NULL;
    for (int i=0;i < TEXT.size();i++)
        if (TEXT[i]->GetID() == id)
            temp=TEXT[i]->GetText();
    return temp;
}

void GUI::AddFont(char* filename,int id, int fsize) 
{
    Font* temp=new Font(filename,fsize,id);
    FONTS.insert(FONTS.begin(),temp);
    return;
}

void GUI::AddButton(int type, int id, int x, int y, SDL_Surface* image,int w, int h) 
{
    Button* temp=new Button(id,type,x,y,image,w,h);
    BUTTONS.insert(BUTTONS.begin(),temp);
    if (BACKGROUND != NULL)
       temp->SetBG(BACKGROUND);
    return;
}
void GUI::AddButton(int type, int id, int x, int y, char* filename, int w, int h) 
{
    Button* temp=new Button(id,type,x,y,filename,w,h);
    BUTTONS.insert(BUTTONS.begin(),temp);
    if (type == Button::text) {
        temp->RenderText(GetFontPtr(w));
        temp->SetFont(GetFontPtr(w));
    }
    if (BACKGROUND != NULL)
       temp->SetBG(BACKGROUND);
    return;
}

void GUI::AddTextInput(int id, int x, int y, int w, int fontid,char *text) 
{
	Button* temp;
    int h=GetFontPtr(fontid)->GetHeight();
	if (text == NULL)
		temp = new Button(id,Button::textinput,x,y,w,h);
	else
		temp = new Button(id,Button::textinput,x,y,text,w,h);
	BUTTONS.insert(BUTTONS.begin(),temp);
	BUTTONS[0]->RenderText(GetFontPtr(fontid));
 	BUTTONS[0]->SetBG(BACKGROUND);
    BUTTONS[0]->SetFont(GetFontPtr(fontid));
	return;
}

void GUI::ChangeButtonText(int id,char* newtext) 
{
	for (int i=0;i < BUTTONS.size();i++)
		if (BUTTONS[i]->GetID() == id) {
			BUTTONS[i]->ChangeText(newtext);
			BUTTONS[i]->Deactivate();
		}
	return;
}

void GUI::ChangeButtonSurface(int id, SDL_Surface* image) 
{
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            BUTTONS[i]->ChangeSurface(image);
    return;
}

void GUI::ChangeButtonSurface(int id, char* filename) 
{
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            BUTTONS[i]->ChangeSurface(filename);
    return;
}

void GUI::ChangeBCords(int id, int x, int y, int w, int h)
{
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id)
            BUTTONS[i]->ChangeCords(x,y,w,h);
    return;
}

bool GUI::CheckActiveButton(int id) 
{
	bool temp=false;
	for (int i=0;i < BUTTONS.size();i++)
		if (BUTTONS[i]->GetID() == id)
  			if (BUTTONS[i]->Active()) 
				temp=true;
	return temp;
}

bool GUI::CheckActiveTxtWin(int id) 
{
	bool temp=false;
	for (int i=0;i < TXTWINS.size();i++)
		if (TXTWINS[i]->GetID() == id)
  			if (TXTWINS[i]->Active()) 
				temp=true;
	return temp;
}

void GUI::SetButtonActive(int id, bool active)
{
    for (int i=0; i < BUTTONS.size();++i)
        if (BUTTONS[i]->GetID() == id)
            if (active)
                BUTTONS[i]->Activate();
            else
                BUTTONS[i]->Deactivate();
    return;
}

int GUI::GetBXcord(int id)
{
    int temp=0;
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            temp=BUTTONS[i]->GetX();
    return temp;
}

int GUI::GetBYcord(int id)
{
    int temp=0;
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            temp=BUTTONS[i]->GetY();
    return temp;
}

int GUI::GetBWidth(int id)
{
    int temp=0;
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            temp=BUTTONS[i]->GetWidth();
    return temp;
}

int GUI::GetBHeight(int id)
{
    int temp=0;
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
            temp=BUTTONS[i]->GetHeight();
    return temp;
}

SDL_Surface* GUI::GetBG()
{
    return BACKGROUND;
}

SDL_Surface* GUI::GetButtonSurface(int id)
{
    SDL_Surface* temp=NULL;
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) 
                temp=BUTTONS[i]->GetSurface();
    return temp;
}

void GUI::Display() 
{
    int i;
    if (DRAW) 
        SDL_BlitSurface(BACKGROUND,NULL,SCREEN,NULL);

    for (i=0;i < TEXT.size();i++)
        UPDATEAREA.insert(UPDATEAREA.end(),TEXT[i]->Display(SCREEN));

    for (i=0;i < TXTWINS.size();i++)
        UPDATEAREA.insert(UPDATEAREA.end(),TXTWINS[i]->Display(SCREEN));

    for (i=0;i < BUTTONS.size();i++)
        UPDATEAREA.insert(UPDATEAREA.end(),BUTTONS[i]->Display(SCREEN));

    for (i=0;i < UPDATEAREA.size();i++) 
        if (UPDATEAREA[i] != NULL) {
            if (DRAW == false) {
                SDL_UpdateRect(SCREEN,UPDATEAREA[i]->x,UPDATEAREA[i]->y,UPDATEAREA[i]->w,UPDATEAREA[i]->h);
            }
            delete UPDATEAREA[i];
        }
    UPDATEAREA.clear();
    if (DRAW) {
        DRAW=false;
        SDL_UpdateRect(SCREEN,0,0,0,0);
    }
}

void GUI::ReDraw() 
{
    DRAW=true;
    // Set all DRAWS for all buttons,text etc etc to TRUE!!!
    int i;
    for (i=0;i < TEXT.size();i++)
        TEXT[i]->Draw(true);
    for (i=0;i < TXTWINS.size();i++)
        TXTWINS[i]->Draw(true);
    for (i=0;i < BUTTONS.size();i++)
        BUTTONS[i]->Draw(true);
    return;
}

void GUI::ReDrawButton(int id)
{
    for (int i=0;i < BUTTONS.size();++i)
        if (BUTTONS[i]->GetID() == id)
            BUTTONS[i]->Draw(true);
    return;
}

int GUI::ProcessEvent(SDL_Event* event) {
    int i,j;
    int info=-1;
/*  Do we really need to do anything with text? We simply use Buttons of type text
       for (i=0; i < TEXT.size();i++)
        if (TEXT[i]->ProcessEvent(event)) {
        }
*/

    for (i=0; i < TXTWINS.size();i++) {
        if (TXTWINS[i]->ProcessEvent(event)) {
        	TXTWINS[i]->Activate();
        	info=TXTWINS[i]->GetID();
        	for (j=0; j < TXTWINS.size(); j++)
        		if (j != i)
        			TXTWINS[j]->Deactivate();
			break;
        }
    }
    for (i=0;i < BUTTONS.size();i++) {
        if (BUTTONS[i]->ProcessEvent(event)) {
        	BUTTONS[i]->Activate();
        	info=BUTTONS[i]->GetID();
        	for (j=0; j < BUTTONS.size(); j++)
        		if (j != i)
        			BUTTONS[j]->Deactivate();
        	break;
        }
	}
    return info;
}

TTF_Font* GUI::GetFont(int id) 
{
    TTF_Font* font=NULL;
    for (int i=0;i < FONTS.size();i++)
        if (FONTS[i]->GetID() == id)
                font=FONTS[i]->GetFont();
    return font;
}

int GUI::GetFontSize(int id) 
{
    int temp=0;
    for (int i=0;i < FONTS.size();i++)
        if (FONTS[i]->GetID() == id)
                temp=FONTS[i]->GetHeight();
    return temp;
}

Font* GUI::GetFontPtr(int id) 
{
    Font* temp=NULL;
    for (int i=0;i < FONTS.size();i++)
        if (FONTS[i]->GetID() == id)
                temp=FONTS[i];
    return temp;
}

char* GUI::GetButtonText(int id) 
{
    char *temp=new char;
    temp[0]='\0';
    for (int i=0;i < BUTTONS.size();i++)
        if (BUTTONS[i]->GetID() == id) {
                delete [] temp;
                temp=BUTTONS[i]->GetText();
        }
    return temp;
}

void GUI::ChangeTextWinBText(int id,char *newtext) 
{
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id)
            TXTWINS[i]->ChangeTextBText(newtext);
    return;
}

char* GUI::GetTxtWinText(int id) 
{
    char *temp=NULL;
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id) {
            temp=TXTWINS[i]->GetTextInput();
        }
    return temp;
}

int GUI::GetTxtWinSize(int id) 
{
    int size=0;
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id) 
            size=TXTWINS[i]->GetSize();
    return size;
}
    
char* GUI::GetTxtWinText(int id, int pos) 
{
    char* temp=NULL;
    for (int i=0;i < TXTWINS.size();i++)
        if (TXTWINS[i]->GetID() == id) {
            temp=TXTWINS[i]->GetText(pos);
        }
    return temp;
}
