#include "LTexture.h"
#include "Turret.h"
#include "Bullet.h"
#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <math.h>

using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

double fireRate = 500;
int bulletSpeed = 10;
double gGunTimer = 0;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

LTexture gTurretTexture;
LTexture gBulletTexture;

Turret gTurret;

Bullet bullets[30];

bool init();
bool loadMedia();
void close();

void createElements();
bool handleInput(SDL_Event *e);
void update();
void render();

int main(int argc, char *argv[])
{
    bool quit = false;
    SDL_Event e;

    if(!init())
    {
        printf("init failed\n");
        close();
        return 0;
    }

    if(!loadMedia())
    {
        printf("loadMedia failed\n");
        close();
        return 0;
    }

    createElements();

    SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 20);
    SDL_RenderClear(gRenderer);
    SDL_RenderPresent(gRenderer);

    while(!quit)
	{

		while(SDL_PollEvent(&e))
		{
			if(handleInput(&e))
			{
				quit = true;
			}
		}
		printf("finished handling input %d\n", SDL_GetTicks());
		update();
		printf("finished updating state %d\n", SDL_GetTicks());
		render();
		printf("finished render %d\n", SDL_GetTicks());
	}

	close();
	printf("thanks for playing\n");
	return 0;
}

bool init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("sdl init failed %s\n", SDL_GetError());
		return false;
	}
	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("warning linear filtering not set\n");
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		printf("img init failed %s\n", IMG_GetError());
		return false;
	}

	gWindow = SDL_CreateWindow("Turret game!!!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(gWindow == NULL)
	{
		printf("createwindow failed %s\n", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(gRenderer == NULL)
	{
		printf("createRenderer failed %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool loadMedia()
{
	if(!gTurretTexture.loadFromFile("assets/piskel turret.png"))
	{
		printf("failed to load turret texture\n");
		return false;
	}
	if(!gBulletTexture.loadFromFile("assets/piskel bullet.png"))
	{
		printf("failed to load bullet texture\n");
		return false;
	}

	return true;
}

void close()
{
	gTurretTexture.free();
	gBulletTexture.free();
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	gWindow = NULL;
	gRenderer = NULL;
	IMG_Quit();
	SDL_Quit();
}

void createElements()
{
    gTurret.setPosition((SCREEN_WIDTH - gTurret.getWidth()) / 2, (SCREEN_HEIGHT - gTurret.getHeight()) / 2);
    gTurret.setStopped(false);

    gGunTimer = SDL_GetTicks();
}

bool handleInput(SDL_Event *e)
{
	if(e->type == SDL_QUIT)
	{
		return true;
	}
	if(e->type == SDL_KEYDOWN)
	{
		switch(e->key.keysym.sym)
		{
			case SDLK_SPACE:
				gTurret.setStopped(true);
				break;
			case SDLK_ESCAPE:
				return true;
		}
	}
	if(e->type == SDL_KEYUP)
	{
		switch(e->key.keysym.sym)
		{
			case SDLK_SPACE:
				gTurret.setStopped(false);
				break;
		}
	}
	return false;
}


void update()
{
	int currentTime = SDL_GetTicks();

	gTurret.move();

	for(int i = 0; i < 30; i++)
	{
		bullets[i].move();
	}

	if(currentTime >= gGunTimer + fireRate)
	{
		printf("    time to fire\n");
		gGunTimer = currentTime;
		for(int i = 0; i < 30; i++)
		{
			if(!bullets[i].getAlive())
			{
				double yVel = sin((gTurret.getAngle() - 90) * 3.1416 / 180) * bulletSpeed;
				double xVel = cos((gTurret.getAngle() - 90) * 3.1416 / 180) * bulletSpeed;
				bullets[i].setPosition(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 - 5);
				bullets[i].setVelocity(xVel, yVel);
				bullets[i].setAlive(true);
				break;
			}
		}
	}
}

void render()
{
	SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 255);
    SDL_RenderClear(gRenderer);



    for(int i = 0; i < 30; i++)
	{
		if(bullets[i].getAlive())
		{
			gBulletTexture.render(bullets[i].getXPos(), bullets[i].getYPos(), NULL, 0);
		}
	}
	gTurretTexture.render(gTurret.getXPos(), gTurret.getYPos(), NULL, gTurret.getAngle());

    SDL_RenderPresent(gRenderer);
}
