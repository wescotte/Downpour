#include "Font.h"

// *****************************************************************************
// FONT FUNCTIONS
// *****************************************************************************
Font::Font(char* filename, int fsize,int id) {
    FONT = TTF_OpenFont(filename,fsize);
    ID=id;
    return;
}

Font::~Font() {
    if (FONT != NULL)
        TTF_CloseFont(FONT);
    return;
}

int Font::GetID() {
    return ID;
}
TTF_Font* Font::GetFont() {
    return FONT;
}
int Font::GetHeight() {
    return TTF_FontHeight(FONT);
}

