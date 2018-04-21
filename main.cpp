#include "LTexture.h"
#include "Turret.h"
#include "Bullet.h"
#include "Enemy.h"
#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <math.h>
#include <cstdlib>

using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAX_BULLETS 30
#define MAX_ENEMIES 10

int fireRate = 100; //milliseconds between shots. lower number is faster
int bulletSpeed = 8; //pixels per click. higher number is faster
int gGunTimer = 0; //timer for firing gun. starts at 0

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

LTexture gTurretTexture;
LTexture gBulletTexture;
LTexture gEnemyTexture;
LTexture gBoxTexture;
LTexture gEnergyTexture;
LTexture gHalfEnergyTexture;

Turret gTurret;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
int enemyCount = 1;
int damaged = 0; //ticks since last damage, controls the yellow flash
int health = 10;
int score = 0;

bool init();
bool loadMedia();
void close();

void createElements();
bool handleInput(SDL_Event *e);
void update();
void render();
void generateNewEnemy(int index);
void renderHud();

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
//		printf("finished handling input %d\n", SDL_GetTicks());
		update();
//		printf("finished updating state %d\n", SDL_GetTicks());
		render();
//		printf("turret position %d, %d angle %d\n", gTurret.getXPos(), gTurret.getYPos(), gTurret.getAngle());
//		printf("finished render %d\n", SDL_GetTicks());
		if(health == 0)
		{
			quit = true;
		}
	}

	close();
	printf("thanks for playing\n");
	printf("You earned %d points\n", score);
	return 0;
}

bool init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("sdl init failed %s\n", SDL_GetError());
		return false;
	}
//	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
//	{
//		printf("warning linear filtering not set\n");
//	}

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
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);



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
	if(!gEnemyTexture.loadFromFile("assets/piskel enemy.png"))
	{
		printf("failed to load enemy texture\n");
		return false;
	}
	if(!gBoxTexture.loadFromFile("assets/piskel box.png"))
	{
		printf("failed to load box texture\n");
		return false;
	}
	if(!gEnergyTexture.loadFromFile("assets/piskel energy.png"))
	{
		printf("failed to load energy texture\n");
		return false;
	}
	if(!gHalfEnergyTexture.loadFromFile("assets/piskel half energy.png"))
	{
		printf("failed to load half energy texture\n");
		return false;
	}

	return true;
}

void close()
{
	gTurretTexture.free();
	gBulletTexture.free();
	gEnemyTexture.free();
	gBoxTexture.free();
	gEnergyTexture.free();
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	gWindow = NULL;
	gRenderer = NULL;
	IMG_Quit();
	SDL_Quit();
}

void createElements()
{
	gTurret.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//    gTurret.setPosition(0, 0);
    gTurret.start();


    gGunTimer = SDL_GetTicks();

	srand(SDL_GetTicks());
    generateNewEnemy(0);
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
//				printf("pressed space\n");
				gTurret.stop();
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
//				printf("released space\n");
				gTurret.start();
				break;
		}
	}
	return false;
}


void update()
{
	int createNewEnemy = -1;
	int currentTime = SDL_GetTicks();

	gTurret.move();

	for(int i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].move();
	}

	for(int i = 0; i < MAX_ENEMIES; i++)
	{
		if(!enemies[i].getAlive())
		{
			continue;
		}
		enemies[i].move();
		if((gTurret.getXPos() - enemies[i].getXPos()) * (gTurret.getXPos() - enemies[i].getXPos()) + (gTurret.getYPos() - enemies[i].getYPos()) * (gTurret.getYPos() - enemies[i].getYPos()) <= 1500)
		{
			printf("enemy reached turret\n");
			health--;
			damaged = 50;
			generateNewEnemy(i);
			continue;
		}
		for(int j = 0; j < MAX_BULLETS; j++)
		{
			if(!bullets[j].getAlive())
			{
				continue;
			}
//			if(enemies[i].getXPos() > bullets[j].getXPos() - 25 && enemies[i].getXPos() < bullets[j].getXPos() + 25 &&
//				enemies[i].getYPos() > bullets[j].getYPos() - 25 && enemies[i].getYPos() < bullets[j].getYPos() + 25)
			if((bullets[j].getXPos() - enemies[i].getXPos()) * (bullets[j].getXPos() - enemies[i].getXPos()) + (bullets[j].getYPos() - enemies[i].getYPos()) * (bullets[j].getYPos() - enemies[i].getYPos()) <= 1000)
			{
				if(enemies[i].damage(1)) // if enemy hit and killed by bullet
				{
					score++;
					createNewEnemy = i;
				}
				bullets[j].kill();
				break;
			}
		}
	}

	if(createNewEnemy != -1)
	{
		generateNewEnemy(createNewEnemy);
		if(rand()%5 == 0 && enemyCount < MAX_ENEMIES)
		{
			generateNewEnemy(enemyCount++);
		}
	}

	if(currentTime >= gGunTimer + fireRate)
	{
//		printf("    time to fire\n");
		gGunTimer = currentTime;
		for(int i = 0; i < MAX_BULLETS; i++)
		{
			if(!bullets[i].getAlive())
			{
				double newAngle = gTurret.getAngle() - 5 + 10 * (float)rand() / (float)RAND_MAX;
				double yVel = sin((newAngle - 90) * 3.1416 / 180) * bulletSpeed;
				double xVel = cos((newAngle - 90) * 3.1416 / 180) * bulletSpeed;
				bullets[i].setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				bullets[i].setVelocity(xVel, yVel);
				bullets[i].setAlive(true);
//				bullets[i].move();
				break;
			}
		}
	}
	if(damaged > 0)
	{
		damaged--;
	}



}

void render()
{
	SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 255);
    SDL_RenderClear(gRenderer);

    for(int i = 0; i < MAX_ENEMIES; i++)
	{
		if(enemies[i].getAlive())
		{
			gEnemyTexture.render(enemies[i].getXPos(), enemies[i].getYPos(), NULL, 0);
		}
	}

    for(int i = 0; i < MAX_BULLETS; i++)
	{
		if(bullets[i].getAlive())
		{
			gBulletTexture.render(bullets[i].getXPos(), bullets[i].getYPos(), NULL, 0);
		}
	}
	gTurretTexture.render(gTurret.getXPos(), gTurret.getYPos(), NULL, gTurret.getAngle());

	if(damaged > 0)
	{
//		printf("damaged, drawing flash\n");
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, (int)(255.0 * damaged / 50.0));
//		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderFillRect(gRenderer, NULL);
		SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 255);
	}

	renderHud();

    SDL_RenderPresent(gRenderer);
}

void generateNewEnemy(int index)
{
	if(index < 0 || index >= MAX_ENEMIES)
	{
		return;
	}

	int wall = rand() % 4;
    int location = rand() % 9 + 1;
    switch(wall)
    {
		case 0: //north wall
			enemies[index].setPosition(SCREEN_WIDTH * (float)location / 10, 40);
			break;
		case 1: //east wall
			enemies[index].setPosition(SCREEN_WIDTH - 40, SCREEN_HEIGHT * (float)location / 10);
			break;
		case 2: //south wall
			enemies[index].setPosition(SCREEN_WIDTH * (float)location / 10, SCREEN_HEIGHT - 40);
			break;
		case 3: //west wall
			enemies[index].setPosition(40, SCREEN_HEIGHT * (float)location / 10);
			break;
	}
	enemies[index].setAlive();
	enemies[index].setVelocity((gTurret.getXPos() - enemies[index].getXPos()) / 300, (gTurret.getYPos() - enemies[index].getYPos()) / 300);
}

void renderHud()
{
	SDL_Rect hudLocation{10, 10, 175, 50};
	SDL_RenderSetViewport(gRenderer, &hudLocation);
	gBoxTexture.render(NULL, NULL, 0);

	for(int i = 0; i < health / 2; i++)
	{
		gEnergyTexture.render(30 + i * 29, 25, NULL, 0);
	}
	if(health % 2)
	{
		gHalfEnergyTexture.render(30 + health / 2 * 29, 25, NULL, 0);
	}

	SDL_RenderSetViewport(gRenderer, NULL);



}
