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
	free();
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

bool LTexture::loadFromRenderedText(TTF_Font *font, std::string text, SDL_Color *color)
{
	free();
	SDL_Surface *loadedSurface = NULL;
    SDL_Texture *loadedTexture = NULL;
    loadedSurface = TTF_RenderText_Solid(font, text.c_str(), *color);
    if(loadedSurface == NULL)
    {
        printf("failed to load %s error %s\n", text.c_str(), IMG_GetError());
        return false;
    }

    loadedTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if(loadedTexture == NULL)
    {
        printf("failed to create texture %s error %s\n", text.c_str(), SDL_GetError());
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
	SDL_Rect dest{x - width / 2, y - height / 2, width, height};
	if(source != NULL)
	{
		dest.w = source->w;
		dest.h = source->h;
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(gRenderer, texture, source, &dest, angle, NULL, SDL_FLIP_NONE);
}

void LTexture::render(SDL_Rect *source,  SDL_Rect *dest, double angle)
{
//	if(source != NULL)
//	{
//		dest.w = source->w;
//		dest.h = source->h;
//		dest.x -= dest.w / 2;
//		dest.y -= dest.h / 2;
//	}

	SDL_RenderCopyEx(gRenderer, texture, source, dest, angle, NULL, SDL_FLIP_NONE);
}

void LTexture::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(texture, r, g, b);
}

void LTexture::setBlendMode(SDL_BlendMode blend)
{
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

void LTexture::setAlphaMod(Uint8 a)
{
	SDL_SetTextureAlphaMod(texture, a);
}
