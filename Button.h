#ifndef Button_H
#define Button_H

#include "SDL.h"
#include "Timer.h"
#include "Text.h"
#include "Font.h"
#include <ctype.h> // For isgraph
#include <iostream>

class Button {
public:
    enum TypesofButtons {pushbutton, text, scrollbar, textinput}; 

    Button(int id, int type, int x, int y,int w=-1, int h=-1);
    Button(int id, int type, int x, int y, SDL_Surface *image,int w=-1, int h=-1);
    Button(int id, int type, int x, int y, char* filename,int w=-1, int h=-1); 
    ~Button();
    
    SDL_Rect* Display(SDL_Surface *dest);
    SDL_Rect* DisplayPushButton(SDL_Surface *dest);
    SDL_Rect* DisplayText(SDL_Surface *dest);
    SDL_Rect* DisplayScrollBar(SDL_Surface *dest);
	SDL_Rect* DisplayTextInput(SDL_Surface *dest);
	
    void ChangeSurface(SDL_Surface* image);
    void ChangeSurface(char* filename);
    void ChangeCords(int x, int y,int w=-1, int h=-1);
    void ChangeText(char* text);
    void Activate();
    void Deactivate();
    bool Active();
    int GetX();
    int GetY();
    int GetHeight();
    int GetWidth();
    int GetID();
    SDL_Surface* GetSurface();
    char* GetText();
	void Draw(bool draw);
    bool ProcessEvent(SDL_Event *event);
    void RenderText(Font* font);
    void SetCursor();
    void FlipCursor();
    void ShowCursor();
    void SetBG(SDL_Surface *bg);
    void EnterText(SDL_Event* event);
    void SetFont(Font* font);
    
private:
    int ID;
    int TYPE;
    SDL_Surface* IMAGE;
    int X,Y;
    int W,H;  
    bool DRAW; // DO we need to redraw this?
    bool DRAWCUR;
    bool ACTIVE;
    Timer CURSOR;
    Text* TEXT;
    Font* FONT;
    SDL_Surface* BG;	// Background not always used
};

#endif
