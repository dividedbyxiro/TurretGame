#include "LTexture.h"
#include "Turret.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Timer.h"
#include "scoreTable.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <math.h>
#include <cstdlib>

using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAX_BULLETS 30
#define MAX_ENEMIES 10
#define TITLE_SCREEN_DURATION 2000.0
#define ENEMY_SPEED 200 //milliseconds it takes for enemy to reach turret. smaller number is faster
#define TURRET_SPEED 2.0
#define MENU_TURRET_SPEED 1.0

enum GAME_STATE
{
	GAME_INTRO_STATE,
	GAME_TITLE_STATE,
	GAME_PAUSED_STATE,
	GAME_PLAY_STATE,
	GAME_LEVELUP_STATE,
	GAME_OVER_STATE,
	GAME_HISCORE_STATE
};

bool easyMode = false; //decreases gap between levels for testing purpose
int fireRate = 100; //milliseconds between shots. lower number is faster
int bulletSpeed = 8; //pixels per click. higher number is faster
int bulletPower = 1; //damage inflicted by each bullet
int gGunTimer = 0; //timer for firing gun. starts at 0
int currentTurretSpeed = TURRET_SPEED;
int currentEnemySpeed = ENEMY_SPEED;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;
Mix_Chunk *gGunSound = NULL;
Mix_Chunk *gEnemyDeath = NULL;

LTexture gTurretTexture;
LTexture gBulletTexture;
LTexture gEnemyTexture;
LTexture gBoxTexture;
LTexture gEnergyTexture;
LTexture gHalfEnergyTexture;
LTexture gScoreTexture;
LTexture gLevelTexture;
LTexture gTimerTexture;
LTexture gTitleTexture;
LTexture gLogoTexture;
LTexture gPowerUpTexture;
LTexture gPowerDownTexture;
LTexture gSpeedUpTexture;
LTexture gSpeedDownTexture;

Turret gTurret;
Turret gCursorTurret;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
int enemyCount = 1;
int damaged = 0; //ticks since last damage, controls the yellow flash
int health = 10;
int score = 0;
SDL_Color scoreColor{255, 255, 255, 255};
SDL_Color backgroundColor{0, 0, 0, 255};
SDL_Color titleColor{255, 255, 255, 255};
SDL_Color green{0, 255, 0, 255};
bool mute = true;
GAME_STATE currentState;
int currentLevel = 0;
bool quit;
SDL_RWops *scoreFile = NULL;
scoreTable scores;


Timer titleTimer;
Timer gunTimer;
Timer playTimer;

bool init();
bool loadMedia();
void close();
void createElements();

void handleInput(SDL_Event *e);
void handleInputIntroState(SDL_Event *e);
void handleInputPlayState(SDL_Event *e);
void handleInputLevelupState(SDL_Event *e);
void update();
void updateIntroState();
void updatePlayState();
void updateLevelupState();
void render();
void renderIntroState();
void renderPlayState();
void renderLevelupState();
void renderHud();
void generateNewEnemy(int index);
int getGameTime();
void levelUp();



int main(int argc, char *argv[])
{
	quit = false;
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
	currentState = GAME_INTRO_STATE;

	createElements();

//    SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 20);
//    SDL_RenderClear(gRenderer);
//    SDL_RenderPresent(gRenderer);

	getGameTime();
	titleTimer.start();
	while(!quit)
	{

		while(SDL_PollEvent(&e))
		{
			handleInput(&e);
		}
//		printf("finished handling input %d\n", SDL_GetTicks());
		update();
//		printf("finished updating state %d\n", SDL_GetTicks());
		render();
//		printf("turret position %d, %d angle %d\n", gTurret.getXPos(), gTurret.getYPos(), gTurret.getAngle());
//		printf("finished render %d\n", SDL_GetTicks());
		if(health <= 0)
		{

			quit = true;
		}
	}
	if(scores.submitNewScore(0, score, playTimer.getTicks()))
	{
		printf("you made a new hi score\n");
	}
	scoreFile = SDL_RWFromFile("scores.xhs", "w+b");
	scores.updateTableFile(scoreFile);
	SDL_RWclose(scoreFile);

	close();
	printf("thanks for playing\n");
	printf("You earned %d points\n", score);
	printf("hi scores:\n");
	for(int i = 0; i < 10; i++)
	{
		printf("%d %d %d\n", scores.getDate(i), scores.getScore(i), scores.getTime(i));
	}
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

	if(TTF_Init() == -1)
	{
		printf("TTF init failed %s\n", TTF_GetError());
		return false;
	}

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("mix openaudio failed %s\n", Mix_GetError());
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

	scoreFile = SDL_RWFromFile("scores.xhs", "r+b");
	if(scoreFile == NULL)
	{
		printf("unable to open file error %s\n", SDL_GetError());
		scoreFile = SDL_RWFromFile("scores.xhs", "w+b");
		if(scoreFile == NULL)
		{
			printf("failed to create new scores file %s\n", SDL_GetError());
			SDL_RWclose(scoreFile);
			return false;
		}
		scores.updateTableFile(scoreFile);
	}
	else
	{
		scores.setTable(scoreFile);
	}
	SDL_RWclose(scoreFile);


	return true;
}

bool loadMedia()
{
//	if(!gTurretTexture.loadFromFile("assets/piskel turret.png"))
	if(!gTurretTexture.loadFromFile("assets/turret2.png"))
	{
		printf("failed to load turret texture\n");
		return false;
	}
//	if(!gBulletTexture.loadFromFile("assets/piskel bullet.png"))
	if(!gBulletTexture.loadFromFile("assets/bullet2.png"))
	{
		printf("failed to load bullet texture\n");
		return false;
	}
//	if(!gEnemyTexture.loadFromFile("assets/piskel enemy.png"))
	if(!gEnemyTexture.loadFromFile("assets/enemy2.png"))
	{
		printf("failed to load enemy texture\n");
		return false;
	}
//	if(!gBoxTexture.loadFromFile("assets/piskel box.png"))
	if(!gBoxTexture.loadFromFile("assets/box2.png"))
	{
		printf("failed to load box texture\n");
		return false;
	}
//	if(!gEnergyTexture.loadFromFile("assets/piskel energy.png"))
	if(!gEnergyTexture.loadFromFile("assets/energy2.png"))
	{
		printf("failed to load energy texture\n");
		return false;
	}
//	if(!gHalfEnergyTexture.loadFromFile("assets/piskel half energy.png"))
	if(!gHalfEnergyTexture.loadFromFile("assets/halfenergy2.png"))
	{
		printf("failed to load half energy texture\n");
		return false;
	}

	gFont = TTF_OpenFont("assets/xiro.ttf", 28);
	if(gFont == NULL)
	{
		printf("failed to open font %s\n", TTF_GetError());
		return false;
	}

	if(!gScoreTexture.loadFromRenderedText(gFont, "0", &scoreColor))
	{
		printf("failed to create score texture\n");
		return false;
	}
	if(!gTimerTexture.loadFromRenderedText(gFont, "00:00.0", &scoreColor))
	{
		printf("failed to create timerTexture\n");
		return false;
	}

	if(!gTitleTexture.loadFromRenderedText(gFont, "The Turret Button", &titleColor))
//	if(!gTitleTexture.loadFromFile("assets/dbx logo.png"))
	{
		printf("failed to create title texture\n");
		return false;
	}
	if(!gLevelTexture.loadFromRenderedText(gFont, "1", &scoreColor))
	{
		printf("failed to create level texture\n");
		return false;
	}

	if(!gLogoTexture.loadFromFile("assets/dbx logo.png"))
	{
		printf("failed to load logo texture\n");
		return false;
	}

	if(!gSpeedDownTexture.loadFromFile("assets/speedDown.png"))
	{
		printf("failed to load speedDown texture\n");
		return false;
	}

	if(!gSpeedUpTexture.loadFromFile("assets/speedUp.png"))
	{
		printf("failed to load speedUp texture\n");
		return false;
	}

	if(!gPowerDownTexture.loadFromFile("assets/powerDown.png"))
	{
		printf("failed to load powerDown texture\n");
		return false;
	}

	if(!gPowerUpTexture.loadFromFile("assets/powerUp.png"))
	{
		printf("failed to load powerUp texture\n");
		return false;
	}

	gGunSound = Mix_LoadWAV("assets/low.wav");
	if(gGunSound == NULL)
	{
		printf("failed to load gun sound %s\n", Mix_GetError());
		return false;
	}

	gEnemyDeath = Mix_LoadWAV("assets/boom3.wav");
	if(gEnemyDeath == NULL)
	{
		printf("failed to load enemy death sound %s\n", Mix_GetError());
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
	gScoreTexture.free();
	gTitleTexture.free();
	gLevelTexture.free();
	gLogoTexture.free();
	gTimerTexture.free();
	gPowerDownTexture.free();
	gPowerUpTexture.free();
	gSpeedDownTexture.free();
	gSpeedUpTexture.free();
//	SDL_RWclose(scoreFile);
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	Mix_FreeChunk(gGunSound);
	Mix_FreeChunk(gEnemyDeath);
	gEnemyDeath = NULL;
	gGunSound = NULL;
	gWindow = NULL;
	gRenderer = NULL;
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void createElements()
{
	gTurret.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	gTurret.setDirection(currentTurretSpeed);
	gTurret.start();
	gunTimer.start();


	gGunTimer = SDL_GetTicks();

	srand(SDL_GetTicks());
	generateNewEnemy(0);
	currentLevel = 1;
}

void handleInput(SDL_Event *e)
{
	switch(currentState)
	{
	case GAME_INTRO_STATE:
		handleInputIntroState(e);
		break;
	case GAME_PLAY_STATE:
		handleInputPlayState(e);
		break;
	case GAME_LEVELUP_STATE:
		handleInputLevelupState(e);
		break;
	}
}

void handleInputIntroState(SDL_Event *e)
{
	return;
}

void handleInputPlayState(SDL_Event *e)
{
	if(e->type == SDL_QUIT)
	{
		quit = true;
		return;
	}
	if(e->type == SDL_KEYDOWN)
	{
		switch(e->key.keysym.sym)
		{
		case SDLK_SPACE:
			gTurret.stop();
			break;
		case SDLK_m:
			mute = !mute;
			break;
		case SDLK_k:
			score++;
			break;
		case SDLK_h:
			health = 10;
			break;
		case SDLK_ESCAPE:
			quit = true;
			return;
		}
	}
	if(e->type == SDL_KEYUP)
	{
		switch(e->key.keysym.sym)
		{
		case SDLK_SPACE:
			gTurret.start();
			break;
		}
	}
}

void handleInputLevelupState(SDL_Event *e)
{
	if(e->type == SDL_QUIT)
	{
		quit = true;
		return;
	}
	if(titleTimer.getTicks() < 2000)
	{
		return;
	}
	if(e->type == SDL_KEYDOWN)
	{
		switch(e->key.keysym.sym)
		{
		case SDLK_SPACE:
			titleTimer.stop();
			currentState = GAME_PLAY_STATE;
			currentState = GAME_PLAY_STATE;
			if(gCursorTurret.getAngle() < 180)
			{
				bulletPower++;
			}
			else
			{
				gTurret.setDirection(gTurret.getDirection() * 1.1);
			}
//			printf("levelup timer done, re-entering playstate\n");
			playTimer.unpause();
//			update();
			gTurret.start();
			break;
		}
	}
}

void update()
{
//	printf("entering update at state=%d\n", currentState);
	switch(currentState)
	{
	case GAME_INTRO_STATE:
		updateIntroState();
		break;
	case GAME_PLAY_STATE:
		updatePlayState();
		break;
	case GAME_LEVELUP_STATE:
		updateLevelupState();
		break;
	}
}

void updatePlayState()
{
//	printf("entering updatePlayState\n");
	int createNewEnemy = -1;
	int currentTime = 0;
	char timerString[8] = "00:00.0";

	if(!playTimer.isStarted())
	{
		playTimer.start();
	}
	currentTime = playTimer.getTicks();
	sprintf(timerString, "%02d:%02d.%d", currentTime / 60000, (currentTime / 1000) % 60, (currentTime / 100) % 10);
	gTimerTexture.loadFromRenderedText(gFont, timerString, &scoreColor);

	gTurret.move();
	for(int i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].move();
	}

//	for(int i = 0; i < MAX_ENEMIES; i++)
	for(int i = 0; i < currentLevel; i++)
	{
		if(!enemies[i].getAlive())
		{
			continue;
		}
		enemies[i].move();
		if((gTurret.getXPos() - enemies[i].getXPos()) * (gTurret.getXPos() - enemies[i].getXPos()) + (gTurret.getYPos() - enemies[i].getYPos()) * (gTurret.getYPos() - enemies[i].getYPos()) <= 1500)	//enemy reaches the player's turret
		{
//			printf("enemy reached turret\n");
			if(!mute)
			{
				Mix_PlayChannel(-1, gEnemyDeath, 0);
			}
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

			if(pow(bullets[j].getXPos() - enemies[i].getXPos(), 2) + pow(bullets[j].getYPos() - enemies[i].getYPos(), 2) <= pow(enemies[i].getSize(), 2))	//enemy hit by bullet
			{
				if(enemies[i].damage(bulletPower)) // if enemy hit and killed by bullet
				{
					char scoreString[3];
					score++;
					sprintf(scoreString, "%d", score);
					createNewEnemy = i;
					if(!mute)
					{
						Mix_PlayChannel(-1, gEnemyDeath, 0);
					}
					if(!gScoreTexture.loadFromRenderedText(gFont, scoreString, &scoreColor))
					{
						printf("failed to update score texture\n");
						return;
					}
				}
				bullets[j].kill();
				break;
			}
		}
	}

	if(createNewEnemy != -1 && score <= 2.5 * (currentLevel * currentLevel + currentLevel) - currentLevel)	//Enemy was killed, gotta replace
	{
		generateNewEnemy(createNewEnemy);
	}

	if(gunTimer.getTicks() >= fireRate)	//time to fire the gun again
	{
//		printf("    time to fire\n");
		gunTimer.start();
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
				if(!mute)
				{
					Mix_PlayChannel(-1, gGunSound, 0);
				}
				break;
			}
		}
	}

	if((easyMode && score >= 3 * currentLevel) || (score >= 2.5 * (currentLevel * currentLevel + currentLevel)))
	{
		levelUp();
		currentState = GAME_LEVELUP_STATE;
		updateLevelupState();
		return;
	}

	if(damaged > 0)		//controls the yellow flash
	{
		damaged--;
	}
}

void updateIntroState()
{
	if(getGameTime() >= TITLE_SCREEN_DURATION)
	{
		currentState = GAME_PLAY_STATE;
		updatePlayState();
	}
}

void updateLevelupState()
{
	if(titleTimer.getTicks() > 3000)
	{
		gCursorTurret.move();
	}

}

void render()
{
	switch(currentState)
	{
	case GAME_INTRO_STATE:
		renderIntroState();
		break;
	case GAME_PLAY_STATE:
		renderPlayState();
		break;
	case GAME_LEVELUP_STATE:
		renderLevelupState();
		break;
	}

}

void renderIntroState()
{
	int timeLapse = titleTimer.getTicks();
	int opacity;

	SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(gRenderer);


//	SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
//	SDL_RenderFillRect(gRenderer, NULL);
	if(timeLapse < TITLE_SCREEN_DURATION / 2)
	{
		gTitleTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, NULL, 0);
		opacity = 255 - (255 * timeLapse / (TITLE_SCREEN_DURATION / 2));
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
		SDL_RenderFillRect(gRenderer, NULL);
	}
	else if(timeLapse < TITLE_SCREEN_DURATION)
	{
		gTitleTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, NULL, 0);
		opacity = (255 * timeLapse / (TITLE_SCREEN_DURATION / 2) - 255);
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
		SDL_RenderFillRect(gRenderer, NULL);
	}
	else if(timeLapse < TITLE_SCREEN_DURATION * 1.5)
	{
		gLogoTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, NULL, 0);
		opacity = (255 * (timeLapse - TITLE_SCREEN_DURATION) / (TITLE_SCREEN_DURATION / 2) - 255);
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
		SDL_RenderFillRect(gRenderer, NULL);
	}
	else if(timeLapse < TITLE_SCREEN_DURATION * 2)
	{
		gLogoTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, NULL, 0);
		opacity = (255 * (timeLapse - TITLE_SCREEN_DURATION) / (TITLE_SCREEN_DURATION / 2) - 255);
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
		SDL_RenderFillRect(gRenderer, NULL);
	}
//	printf("current opacity %d\n", opacity);
	SDL_RenderPresent(gRenderer);
}

void renderPlayState()
{
//	SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 255);
//	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
//	printf("entering play render\n");
	SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(gRenderer);

//	printf("entering play render\n");
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
//		SDL_SetRenderDrawColor(gRenderer, 20, 20, 40, 255);
//		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	}

	renderHud();

	SDL_RenderPresent(gRenderer);
}

void renderLevelupState()
{
	SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(gRenderer);
	if(titleTimer.getTicks() < 3000)
	{
		gTitleTexture.setAlphaMod(titleTimer.getTicks() / 3000.0 * 255);
		gTitleTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, NULL, 0);
	}
	else
	{
		SDL_Rect highlighter;
		gTurretTexture.render(gCursorTurret.getXPos(), gCursorTurret.getYPos(), NULL, gCursorTurret.getAngle());
		gPowerUpTexture.render(gCursorTurret.getXPos() + 100, gCursorTurret.getYPos(), NULL, 0);
		gSpeedUpTexture.render(gCursorTurret.getXPos() - 100, gCursorTurret.getYPos(), NULL, 0);
		if(gCursorTurret.getAngle() < 180)
		{
			highlighter.h = gPowerUpTexture.getHeight();
			highlighter.w = gPowerUpTexture.getWidth();
			highlighter.x = gCursorTurret.getXPos() + 100 - highlighter.w / 2;
			highlighter.y = gCursorTurret.getYPos() - highlighter.h / 2;

		}
		else
		{
			highlighter.h = gSpeedUpTexture.getHeight();
			highlighter.w = gSpeedUpTexture.getWidth();
			highlighter.x = gCursorTurret.getXPos() - 100 - highlighter.w / 2;
			highlighter.y = gCursorTurret.getYPos() - highlighter.h / 2;
		}
		SDL_SetRenderDrawColor(gRenderer, green.r, green.g, green.b, green.a);
		SDL_RenderDrawRect(gRenderer, &highlighter);
		SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	}
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
	int velocityMod = rand() % 100;
	switch(wall)
	{
	case 0: //north wall
		enemies[index].setPosition(SCREEN_WIDTH * (float)location / 10, -40);
		break;
	case 1: //east wall
		enemies[index].setPosition(SCREEN_WIDTH + 40, SCREEN_HEIGHT * (float)location / 10);
		break;
	case 2: //south wall
		enemies[index].setPosition(SCREEN_WIDTH * (float)location / 10, SCREEN_HEIGHT + 40);
		break;
	case 3: //west wall
		enemies[index].setPosition(-40, SCREEN_HEIGHT * (float)location / 10);
		break;
	}
	enemies[index].setAlive();
	enemies[index].setVelocity((gTurret.getXPos() - enemies[index].getXPos()) / (300 + velocityMod), (gTurret.getYPos() - enemies[index].getYPos()) / (300 + velocityMod));
	enemies[index].setSize(32);
}

void renderHud()
{
	SDL_Rect healthLocation{10, 10, 175, 50};
	SDL_Rect scoreLocation{(SCREEN_WIDTH - gScoreTexture.getWidth()) / 2 - 80, 10, gScoreTexture.getWidth() + 60, 50};
	SDL_Rect levelLocation{(SCREEN_WIDTH - gLevelTexture.getWidth()) / 2 + 60, 10, gLevelTexture.getWidth() + 60, 50};
//	SDL_Rect timerLocation{SCREEN_WIDTH - gTimerTexture.getWidth() - 70, 10, gTimerTexture.getWidth() + 60, 50};
	SDL_Rect timerLocation{SCREEN_WIDTH - 185, 10, 175, 50};
	SDL_RenderSetViewport(gRenderer, &healthLocation);
	gBoxTexture.render(NULL, NULL, 0);

	for(int i = 0; i < health / 2; i++)
	{
		gEnergyTexture.render(30 + i * 29, 25, NULL, 0);
	}
	if(health % 2)
	{
		gHalfEnergyTexture.render(30 + health / 2 * 29, 25, NULL, 0);
	}

	SDL_RenderSetViewport(gRenderer, &scoreLocation);
	gBoxTexture.render(NULL, NULL, 0);
	gScoreTexture.render(scoreLocation.w / 2, scoreLocation.h / 2, NULL, 0);

	SDL_RenderSetViewport(gRenderer, &levelLocation);
	gBoxTexture.render(NULL, NULL, 0);
	gLevelTexture.render(levelLocation.w / 2, levelLocation.h / 2, NULL, 0);

	SDL_RenderSetViewport(gRenderer, &timerLocation);
	gBoxTexture.render(NULL, NULL, 0);
	gTimerTexture.render(timerLocation.w / 2, timerLocation.h / 2, NULL, 0);

	SDL_RenderSetViewport(gRenderer, NULL);
}

int getGameTime()
{
	static int startTime = SDL_GetTicks();

	return SDL_GetTicks() - startTime;
}

void levelUp()
{
	char levelString[30];
	currentLevel++;
	printf("reached score %d, increasing level from %d to %d\n", score, currentLevel - 1, currentLevel);
	for(int i = 0; i < currentLevel && i < MAX_ENEMIES; i++)
	{
		printf("generated enemy %d of %d\n", i, currentLevel);
		generateNewEnemy(i);
	}
	sprintf(levelString, "%d", currentLevel);
	gLevelTexture.loadFromRenderedText(gFont, levelString, &scoreColor);
	sprintf(levelString, "Entering level %d", currentLevel);
	gTitleTexture.loadFromRenderedText(gFont, levelString, &titleColor);
	gTitleTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	gCursorTurret.setDirection(.5);
	gCursorTurret.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3);
	titleTimer.start();
	playTimer.pause();
}
