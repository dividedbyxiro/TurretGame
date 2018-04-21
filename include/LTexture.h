#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <SDL.h>
#include <string>


class LTexture
{
    public:
        LTexture();
        virtual ~LTexture();
        void free();
        bool loadFromFile(std::string path);
        void render(int x, int y, SDL_Rect *source, double angle);
        void render(SDL_Rect *source,  SDL_Rect *dest, double angle);
        int getWidth();
        int getHeight();

    protected:

    private:
        SDL_Texture *texture;
        int width;
        int height;
};

#endif // LTEXTURE_H
