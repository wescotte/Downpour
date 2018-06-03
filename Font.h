#ifndef Font_H
#define Font_H
#include "SDL_ttf.h"

class Font {
public:
    Font(char* filename, int fsize,int id);
    ~Font();
    int GetID();
    TTF_Font* GetFont();
    int GetHeight();
private:
    TTF_Font *FONT;
    int ID;
};

#endif
