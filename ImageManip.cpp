#include "ImageManip.h"

SDL_Surface* ResizeImageToFit(SDL_Surface* src, int x, int y)
{
    if (src == NULL)
        return NULL;

    int srcx=src->w; int srcy=src->h;
    double xfact=double(x)/srcx;
    double yfact=double(y)/srcy;

    return ResizeImage(src,xfact,yfact);
}

SDL_Surface* ResizeImage(SDL_Surface* src, double xfact, double yfact) 
{
    if (src == NULL)
        return NULL;
// Create a new surface
    SDL_Surface *dest;
    int x=int(src->w * xfact);
    int y=int(src->h * yfact);

    dest=SDL_CreateRGBSurface(SDL_HWSURFACE, x ,y, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask,src->format->Amask);
    // Might want to check BitsPerPixel.. If 8 copy palette 
    if (src->format->BitsPerPixel == 8)
        SDL_SetPalette(dest, SDL_LOGPAL|SDL_PHYSPAL , src->format->palette->colors, 0, 256);
    
    float xsource = 0; float ysource = 0;
    for (int X=0; X <dest->w; X++) {
        ysource=0;
        for(int Y=0; Y < dest->h; Y++) {
            putpixel(dest, X, Y, getpixel(src, int(xsource), int(ysource)));
            ysource += (1/yfact);
        }
    xsource +=(1/xfact);
    }
    return dest;
}


void FadeImage(SDL_Surface* surface, int percent)
{
    if (surface == NULL)
        return;
    
    Uint32 pixel;
    Uint8 r,g,b;
    for (int x=0; x < surface->w;x++)
        for (int y=0; y < surface->h;y++) {
            pixel=getpixel(surface,x,y);
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            r = r - Uint8((percent/100.0 * r));
            g = g - Uint8((percent/100.0 * g));
            b = b - Uint8((percent/100.0 * b));
            pixel=SDL_MapRGB(surface->format, r, g,b);
            putpixel(surface,x,y,pixel);        
        }
    return;
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{  
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}
