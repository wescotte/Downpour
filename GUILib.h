#ifndef GUILib_H
#define GUILib_H

#include "SDL_ttf.h"
#include "Button.h"
#include "Text.h"
#include "TextWin.h"
#include "Font.h"
#include <vector>

class GUI {
public:
    GUI(SDL_Surface *screen);
    GUI(SDL_Surface *screen,char *background);
    GUI(SDL_Surface *screen,SDL_Surface *background);
    ~GUI();

    void AddTxtWin(int xpos, int ypos, int width, int height,int buffersize,int id,Font* font,bool textinput=false);
    void AddTxtToWin(char* text,int id);
    void AddTxtToWin(Text* text,int id);
    void ChangeTxtWinInputCords(int id,int x, int y, int w, int h=-1);
    void AddText(char* text,int id, int x, int y,int fontid,int r=255, int g=255, int b=255);
    void ChangeText(int id, char* newtext, int fontid);
    void ChangeTextColor(int id, int fontid, int r, int g, int b);
    char* GetText(int id);
    void AddFont(char* filename,int id,int fsize);
    void AddButton(int type, int id, int x, int y, SDL_Surface* image,int w=-1, int h=-1);
    void AddButton(int type, int id, int x, int y, char* filename, int w=-1, int h=-1);
	void AddTextInput(int id, int x, int y, int w, int fontid,char *text=NULL);
	void ChangeButtonText(int id,char *newtext);
	void ChangeTextWinBText(int id,char *newtext);
	void ChangeButtonSurface(int id, SDL_Surface* image);
	void ChangeButtonSurface(int id, char* filename);
    void ChangeBCords(int id, int x, int y, int w=-1, int h=-1);
	bool CheckActiveButton(int id);
	bool CheckActiveTxtWin(int id);
	void SetButtonActive(int id, bool active);
    int GetBXcord(int id);
    int GetBYcord(int id);
    int GetBWidth(int id);
    int GetBHeight(int id);
    SDL_Surface* GetBG();
    SDL_Surface* GetButtonSurface(int id);
    
    void Display();
    void ReDraw();
    void ReDrawButton(int id);
    
    int ProcessEvent(SDL_Event* event);
    
    TTF_Font* GetFont(int id);
    int GetFontSize(int id);
    Font* GetFontPtr(int id);
    char* GetButtonText(int id);
    char* GetTxtWinText(int id);
    int GetTxtWinSize(int id);
    char* GetTxtWinText(int id, int pos);

private:
    SDL_Surface* SCREEN;
    SDL_Surface* BACKGROUND;
    bool DRAW;

    std::vector<SDL_Rect*> UPDATEAREA;    // List of areas of the screen that need updating
    std::vector<Button*> BUTTONS;    // List of buttons
    std::vector<TxtWin*> TXTWINS;    // List of text windows
    std::vector<Text*> TEXT;    // List of plain text
    std::vector<Font*> FONTS; // List of loaded fonts
};

#endif

