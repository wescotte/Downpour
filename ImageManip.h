#ifndef ImageManip_H
#define ImageManip_H

#include "SDL.h"

SDL_Surface* ResizeImage(SDL_Surface* src, double xfact, double yfact);
SDL_Surface* ResizeImageToFit(SDL_Surface* src, int x, int y);
void FadeImage(SDL_Surface* src, int percent);

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);

#endif
