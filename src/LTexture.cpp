#include "LTexture.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>

extern SDL_Renderer *gRenderer;

LTexture::LTexture()
{
    texture = NULL;
    width = height = 0;
    //ctor
}

LTexture::~LTexture()
{
    free();
    //dtor
}

void LTexture::free()
{
    if(texture == NULL)
    {
        return;
    }

    SDL_DestroyTexture(texture);
    width = height = 0;
}

bool LTexture::loadFromFile(std::string path)
{
    SDL_Surface *loadedSurface = NULL;
    SDL_Texture *loadedTexture = NULL;
    loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        printf("failed to load %s error %s\n", path.c_str(), IMG_GetError());
        return false;
    }

    loadedTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if(loadedTexture == NULL)
    {
        printf("failed to create texture %s error %s\n", path.c_str(), SDL_GetError());
        SDL_FreeSurface(loadedSurface);
        return false;
    }
    width = loadedSurface->w;
    height = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
    texture = loadedTexture;
    return true;
}

int LTexture::getWidth()
{
    return width;
}

int LTexture::getHeight()
{
    return height;
}

void LTexture::render(int x, int y, SDL_Rect *source, double angle)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	if(source == NULL)
	{
		dest.w = width;
		dest.h = height;
	}
	else
	{
		dest.w = source->w;
		dest.h = source->h;
	}

	SDL_RenderCopyEx(gRenderer, texture, source, &dest, angle, NULL, SDL_FLIP_NONE);
}
