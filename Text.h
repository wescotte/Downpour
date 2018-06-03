#ifndef Text_H
#define Text_H

#include "SDL.h"
#include "SDL_ttf.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Text {
public:
    Text(char* text,int id,int x, int y,int r=255,int g=255, int b=255);
    Text(Text* text);
    ~Text();

    void ChangeText(char* text);
    void RenderText(TTF_Font* font);
    void RenderTextWrap(TTF_Font* font,int width);
    SDL_Rect* Display(SDL_Surface* dest);
    int GetID();
    char* GetText();
    void ChangeCords(int x, int y);
    bool FindBreak(char* text, int start, int finish, int width,TTF_Font* font);
    SDL_Surface* RenderText(int s, int f, TTF_Font* font);
    int GetHeight();
    int GetWidth();
    void SetColor(Uint8 r, Uint8 g, Uint8 b);
    void Draw(bool draw);
    
   	void CopySurface(SDL_Surface* src,SDL_Rect* Srect,SDL_Surface* dest,SDL_Rect* Drect);
    SDL_Surface* RTEXT;    // Text rendered to a surface
private:

    void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
    Uint32 getpixel(SDL_Surface *surface, int x, int y);
    
    char* TEXT; // The actual text itself
	SDL_Color COLOR; // The Color duh!
    int X, Y;    // X/Y Coords for where on the surface it will be dispalyed
    int ID; // Text ID
    bool DRAW;   // Do we need to redraw this?
};

#endif
