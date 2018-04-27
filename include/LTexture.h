#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <SDL.h>
#include <string>
#include <SDL_ttf.h>


class LTexture
{
public:
	LTexture();
	virtual ~LTexture();
	void free();
	bool loadFromFile(std::string path);
	bool loadFromRenderedText(TTF_Font *font, std::string text, SDL_Color *color);
	void render(int x, int y, SDL_Rect *source, double angle);
	void render(SDL_Rect *source,  SDL_Rect *dest, double angle);
	void setColorMod(Uint8 r, Uint8 g, Uint8 b);
	void setBlendMode(SDL_BlendMode blend);
	void setAlphaMod(Uint8 a);
	int getWidth();
	int getHeight();

protected:

private:
	SDL_Texture *texture;
	int width;
	int height;
};

#endif // LTEXTURE_H
