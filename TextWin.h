#ifndef TextWin_H
#define TextWin_H

#include "SDL.h"
#include "Text.h"
#include "Font.h"
#include "Button.h"
#include "Timer.h"
#include <vector>

class TxtWin {
public:
    TxtWin(int xpos, int ypos,int width, int height, int buffersize,int id, Font* font,SDL_Surface* background, bool textinput);
    ~TxtWin();
    int GetID();
    void AddText(char* text);
    void AddText(Text* text);
    SDL_Rect* Display(SDL_Surface* dest);
    void DisplayText(SDL_Surface* dsurface);
    bool ProcessEvent(SDL_Event *event);
    void SetScrollBar();
    void SetButtons();
    
    void Activate();
    void Deactivate();
    bool Active();
    void Draw(bool draw);
    int GetSize();
    char* GetText(int pos);
    char* GetTextInput();
    void ChangeTextBText(char* newtext);
    void ChangeTextInputCoords(int x,int y,int w,int h);
    
private:
    Font* FONT;
    SDL_Surface* BACKGROUND;

    Button* UPARROW;
    Button* DOWNARROW;
	Button* SCROLLBAR;
    Button* TEXTINPUTB;

    int ID;   
    int XPOS, YPOS;
    int WIDTH, HEIGHT;
    
    int TBX, TBY;
    int TBW, TBH;

	int POSITION;
 	int MAXPOS;
   	
    int BSIZE;    // Buffer size

    int FHEIGHT; // Font Height
    std::vector<Text*> TEXT;
    bool DRAW;  // Do we need to redraw this?
    bool DRAWCUR;
    bool ACTIVE;
    bool TEXTINPUT;
    bool DRAG;  
};

#endif
