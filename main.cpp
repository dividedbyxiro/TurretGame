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
#define MAX_BULLETS 50
#define MAX_ENEMIES 10
#define TITLE_SCREEN_DURATION 2000.0
#define ENEMY_SPEED 300 //milliseconds it takes for enemy to reach turret. smaller number is faster
#define TURRET_SPEED 2.0	//degrees moved per 1/60 second
#define MENU_TURRET_SPEED 1.0	//degrees moved per 1/60 second
#define ENEMY_HEALTH 10 //default amount of health an enemy starts with
#define FIRE_RATE 100	//milliseconds between firing turret
#define BULLET_STRENGTH 3	//default amount of health taken away by a single bullet

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

SDL_Color scoreColor{255, 255, 255, 255};
SDL_Color backgroundColor{0, 0, 0, 255};
SDL_Color titleColor{255, 255, 255, 255};
SDL_Color highlightColor{0, 255, 0, 255};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;
Mix_Chunk *gGunSound = NULL;
Mix_Chunk *gEnemyDeath = NULL;

LTexture gTurretTexture;
LTexture gBulletSheet;
LTexture gEnemy1Sheet;
LTexture gEnemy2Sheet;
LTexture gEnemy3Sheet;
LTexture gEnemy4Sheet;
LTexture gEnemy5Sheet;
LTexture gBoxTexture;
LTexture gEnergyTexture;
LTexture gHalfEnergyTexture;
LTexture gScoreTexture;	//text label to display score for HUD
LTexture gScoreLabel;	//the little "SCORE" label that appears in the HUD
LTexture gLevelTexture;	//text label to display current level for HUD
LTexture gLevelLabel;	//the little "LEVEL" label that appears in the HUD
LTexture gTimerTexture;	//text label to display current time for HUD
LTexture gTimerLabel;	//the little "TIMER" label that appears in the HUD
LTexture gLevelupTexture;	//text label to display what changes for next level. Also used temporarily to display title and gameover screens
LTexture gLevelupTexture2;	//text label to display instruction for new levelup. Also used for gameover screen
LTexture gLogoTexture;
LTexture gPowerUpTexture;
LTexture gPowerDownTexture;
LTexture gSpeedUpTexture;
LTexture gSpeedDownTexture;
LTexture gDamageTexture;
LTexture gEnemySpeedDownTexture;
LTexture gEnemySpeedUpTexture;
LTexture gFireRateDownTexture;
LTexture gFireRateUpTexture;
LTexture gFullHealTexture;
LTexture gRicochetPowersTexture;
LTexture gAutofirePowersTexture;
LTexture gPausedTexture;
LTexture gQuitTexture;
LTexture gPlayAgainTexture;
LTexture *levelupChoice1 = NULL;	/**< controls which option appears on left */
LTexture *levelupChoice2 = NULL;	/**< controls which option appears on right */

Timer titleTimer;
Timer gunTimer;
Timer playTimer;
Timer enemyTimer;
Turret gTurret;
Turret gCursorTurret;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

bool easyMode = false;			//decreases gap between levels for testing purpose
bool ricochetEnabled = false;	//whether player has unlocked ricochet powers. starts false
bool autofireEnabled = false;	//whether player has unlocked autofire powers. starts false
bool mute = true;
int bulletSpeed = 8; //pixels per click. higher number is faster
int currentFireRate = FIRE_RATE;				//milliseconds between shots. May be modified by in-game power-ups
int bulletPower = BULLET_STRENGTH; //damage inflicted by each bullet
int currentTurretSpeed = TURRET_SPEED;	//current turret speed that may be modified by in-game power-ups
int currentEnemySpeed = ENEMY_SPEED;	//current enemy speed that may be modified by in-game power-ups
int currentEnemyHealth = ENEMY_HEALTH;	//amount of health new enemies start with, may be modified on higher levels
int damaged = 0; //ticks since last damage, controls the yellow flash
int health = 10;
int score = 0;
int currentLevel = 0;
int bulletMode = 0;
bool quit;
GAME_STATE currentState;
char timerString[8];	//string used to hold formatted timer
char scoreString[3];	//string used to hold formatted score
//SDL_RWops *scoreFile = NULL;
//scoreTable scores;

int levelScores[10] =	//scores required to move to the next level
{
    5,
    15,
    30,
    50,
    75,
    105,
    140,
    180,
    225,
    300
};

bool init();
bool loadMedia();
void close();
void createElements();

void handleInput(SDL_Event *e);
void handleInputIntroState(SDL_Event *e);
void handleInputPlayState(SDL_Event *e);
void handleInputLevelupState(SDL_Event *e);
void handleInputPausedState(SDL_Event *e);
void handleInputGameoverState(SDL_Event *e);
void update();
void updateIntroState();
void updatePlayState();
void updateLevelupState();
void updatePausedState();
void updateGameoverState();
void render();
void renderIntroState();
void renderPlayState();
void renderLevelupState();
void renderPausedState();
void renderGameoverState();
void renderHud();
void generateNewEnemy(int index);
int getGameTime();
void levelUp();
void pause();
void unpause();



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

    getGameTime();
    titleTimer.start();
    while(!quit)
    {

        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            handleInput(&e);
        }
        update();
        render();
//		if(health <= 0)
//		{
//			if(scores.submitNewScore(0, score, playTimer.getTicks()))
//			{
//				printf("you made a new hi score\n");
//				scoreFile = SDL_RWFromFile("scores.xhs", "w+b");
//				scores.updateTableFile(scoreFile);
//				SDL_RWclose(scoreFile);
//				printf("thanks for playing\n");
//				printf("You earned %d points\n", score);
//			}
//			quit = true;
//		}
    }



    close();
//	printf("hi scores:\n");
//	for(int i = 0; i < 10; i++)
//	{
//		printf("%d %d %d\n", scores.getDate(i), scores.getScore(i), scores.getTime(i));
//	}
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

//	scoreFile = SDL_RWFromFile("scores.xhs", "r+b");
//	if(scoreFile == NULL)
//	{
//		printf("unable to open file error %s\n", SDL_GetError());
//		scoreFile = SDL_RWFromFile("scores.xhs", "w+b");
//		if(scoreFile == NULL)
//		{
//			printf("failed to create new scores file %s\n", SDL_GetError());
//			SDL_RWclose(scoreFile);
//			return false;
//		}
//		scores.updateTableFile(scoreFile);
//	}
//	else
//	{
//		scores.setTable(scoreFile);
//	}
//	SDL_RWclose(scoreFile);


    return true;
}

bool loadMedia()
{
//	if(!gTurretTexture.loadFromFile("assets/piskel turret.png"))
    if(!gTurretTexture.loadFromFile("assets/turret3.png"))
    {
        printf("failed to load turret texture\n");
        return false;
    }
    gTurretTexture.setFrameCount(8);

    if(!gBulletSheet.loadFromFile("assets/bulletsheet.png"))
    {
        printf("failed to load bullet sheet\n");
        return false;
    }
    gBulletSheet.setFrameCount(5);

//	if(!gEnemyTexture.loadFromFile("assets/piskel enemy.png"))
    if(!gEnemy1Sheet.loadFromFile("assets/enemy1sheet.png"))
    {
        printf("failed to load enemy1 sheet\n");
        return false;
    }
    gEnemy1Sheet.setFrameCount(3);

    if(!gEnemy2Sheet.loadFromFile("assets/enemy2sheet.png"))
    {
        printf("failed to load enemy2 sheet\n");
        return false;
    }
    gEnemy2Sheet.setFrameCount(3);

    if(!gEnemy3Sheet.loadFromFile("assets/enemy3sheet.png"))
    {
        printf("failed to load enemy3 sheet\n");
        return false;
    }
    gEnemy3Sheet.setFrameCount(3);

    if(!gEnemy4Sheet.loadFromFile("assets/enemy4sheet.png"))
    {
        printf("failed to load enemy4 sheet\n");
        return false;
    }
    gEnemy4Sheet.setFrameCount(3);

    if(!gEnemy5Sheet.loadFromFile("assets/enemy5sheet.png"))
    {
        printf("failed to load enemy5 sheet\n");
        return false;
    }
    gEnemy5Sheet.setFrameCount(3);

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

    if(!gScoreLabel.loadFromRenderedText(gFont, "SCORE", &scoreColor))
	{
		printf("failed to create score label\n");
		return false;
	}

	if(!gTimerLabel.loadFromRenderedText(gFont, "SCORE", &scoreColor))
	{
		printf("failed to create timer label\n");
		return false;
	}

	if(!gLevelLabel.loadFromRenderedText(gFont, "SCORE", &scoreColor))
	{
		printf("failed to create level label\n");
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

    if(!gLevelupTexture.loadFromRenderedText(gFont, "The Turret Button", &titleColor))
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

    if(!gPausedTexture.loadFromRenderedText(gFont, "PAUSED", &scoreColor))
    {
        printf("failed to create paused texture\n");
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

    if(!gDamageTexture.loadFromFile("assets/damage.png"))
    {
        printf("failed to load damage texture\n");
        return false;
    }

    if(!gEnemySpeedDownTexture.loadFromFile("assets/enemySpeedDown.png"))
    {
        printf("failed to load enemyspeeddown texture\n");
        return false;
    }

    if(!gEnemySpeedUpTexture.loadFromFile("assets/enemySpeedUp.png"))
    {
        printf("failed to load enemyspeedup texture\n");
        return false;
    }

    if(!gFireRateDownTexture.loadFromFile("assets/fireRateDown.png"))
    {
        printf("failed to load fireRateDown texture\n");
        return false;
    }

    if(!gFireRateUpTexture.loadFromFile("assets/fireRateUp.png"))
    {
        printf("failed to load fireRateUp texture\n");
        return false;
    }

    if(!gFullHealTexture.loadFromFile("assets/fullHeal.png"))
    {
        printf("failed to load fullHeal texture\n");
        return false;
    }

    if(!gRicochetPowersTexture.loadFromFile("assets/ricochet.png"))
    {
        printf("failed to load ricochet powers texture\n");
        return false;
    }

    if(!gAutofirePowersTexture.loadFromFile("assets/autofire.png"))
    {
        printf("failed to load autofire powers texture\n");
        return false;
    }

    if(!gQuitTexture.loadFromFile("assets/quit.png"))
    {
        printf("failed to load quit texture\n");
        return false;
    }

    if(!gPlayAgainTexture.loadFromFile("assets/playAgain.png"))
    {
        printf("failed to load playAgain texture\n");
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
    gBulletSheet.free();
    gEnemy1Sheet.free();
    gBoxTexture.free();
    gEnergyTexture.free();
    gScoreTexture.free();
    gLevelupTexture.free();
    gLevelupTexture2.free();
    gLevelTexture.free();
    gLogoTexture.free();
    gTimerTexture.free();
    gPowerDownTexture.free();
    gPowerUpTexture.free();
    gSpeedDownTexture.free();
    gSpeedUpTexture.free();
    gDamageTexture.free();
    gEnemySpeedDownTexture.free();
    gEnemySpeedUpTexture.free();
    gFireRateDownTexture.free();
    gFireRateUpTexture.free();
    gFullHealTexture.free();
    gRicochetPowersTexture.free();
    gPausedTexture.free();
    gQuitTexture.free();
    gPlayAgainTexture.free();
    gScoreLabel.free();
    gLevelLabel.free();
    gTimerLabel.free();
    levelupChoice1 = NULL;	//which levelup choice will appear on left
    levelupChoice2 = NULL;	//which levelup choice will appear on right
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
    playTimer.stop();
    enemyTimer.start();
    gCursorTurret.setDirection(1);
    gCursorTurret.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3);

    currentFireRate = FIRE_RATE;
    bulletPower = BULLET_STRENGTH;
    currentTurretSpeed = TURRET_SPEED;
    currentEnemySpeed = ENEMY_SPEED;
    currentEnemyHealth = ENEMY_HEALTH;
    autofireEnabled = false;
    ricochetEnabled = false;
    health = 10;
    score = 0;
    bulletMode = 0;
    currentLevel = 0;
    for(int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].kill();
    }

    srand(SDL_GetTicks());
    generateNewEnemy(0);
    currentLevel = 1;

    if(!gLevelTexture.loadFromRenderedText(gFont, "1", &scoreColor))
    {
        printf("failed to reset level texture\n");
    }
    if(!gScoreTexture.loadFromRenderedText(gFont, "0", &scoreColor))
    {
        printf("failed to reset score texture\n");
    }


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
    case GAME_PAUSED_STATE:
        handleInputPausedState(e);
        break;
    case GAME_OVER_STATE:
        handleInputGameoverState(e);
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
    if(e->type == SDL_KEYDOWN && e->key.repeat == false)
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
            health = 0;
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
        case SDLK_p:
            pause();
            currentState = GAME_PAUSED_STATE;
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
    if(e->type == SDL_KEYDOWN && e->key.repeat == false)
    {
        switch(e->key.keysym.sym)
        {
        case SDLK_SPACE:
            titleTimer.stop();
            currentState = GAME_PLAY_STATE;
            currentState = GAME_PLAY_STATE;
            if(gCursorTurret.getAngle() < 180)		//choose option on right
            {
                switch(currentLevel)
                {
                case 2:
                case 3:
                case 4:
                    bulletPower++;
                    bulletMode++;
                    break;
				case 5:
					currentEnemySpeed *= 1.1;
					break;
				case 6:
					ricochetEnabled = true;
					break;
				case 7:
				case 8:
					bulletPower++;
                    bulletMode++;
                    break;
				case 9:
					autofireEnabled = true;
					break;
				case 10:
					gTurret.setDirection(gTurret.getDirection() * 1.1);
                    break;
                default:
                    break;
                }
            }
            else		//choose option on left
            {
                switch(currentLevel)
                {
                case 2:
                case 3:
                case 4:
                    gTurret.setDirection(gTurret.getDirection() * 1.1);
                    break;
				case 5:
					currentFireRate *= .9;
					break;
				case 6:
					health = 10;
					break;
				case 7:
					gTurret.setDirection(gTurret.getDirection() * 1.1);
                    break;
				case 8:
					currentFireRate *= .9;
					break;
				case 9:
					health = 10;
					break;
				case 10:
					currentFireRate *= .9;
					break;
                default:
                    break;
                }
            }
//			printf("levelup timer done, re-entering playstate\n");
            unpause();
            enemyTimer.start();
//			update();
//			gTurret.start();
            break;
        }
    }
}

void handleInputPausedState(SDL_Event *e)
{
    if(e->type == SDL_KEYUP && e->key.repeat == false)
    {
        switch(e->key.keysym.sym)
        {
        case SDLK_p:
            currentState = GAME_PLAY_STATE;
            unpause();
            break;
        case SDLK_ESCAPE:
            health = 0;
            currentState = GAME_PLAY_STATE;
            break;
        }
    }
}

void handleInputGameoverState(SDL_Event *e)
{
    if(e->type == SDL_KEYDOWN && e->key.repeat == false && e->key.keysym.sym == SDLK_SPACE && titleTimer.getTicks() > 5000)
    {
        if(gCursorTurret.getAngle() < 180)
        {
            createElements();
            currentState = GAME_PLAY_STATE;
        }
        else
        {
            quit = true;
        }
    }
    return;
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
    case GAME_PAUSED_STATE:
        updatePausedState();
        break;
    case GAME_OVER_STATE:
        updateGameoverState();
        break;
    }
}

void updatePlayState()
{
//	printf("entering updatePlayState\n");
    int createNewEnemy = -1;
    int currentTime = 0;
    int enemyCount = 0;

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
        enemyCount++;
        if((gTurret.getXPos() - enemies[i].getXPos()) * (gTurret.getXPos() - enemies[i].getXPos()) + (gTurret.getYPos() - enemies[i].getYPos()) * (gTurret.getYPos() - enemies[i].getYPos()) <= enemies[i].getSize() * enemies[i].getSize())	//enemy reaches the player's turret
        {
//			printf("enemy reached turret\n");
            if(!mute)
            {
                Mix_PlayChannel(-1, gEnemyDeath, 0);
            }
            health--;
            if(health <= 0)
            {
                currentState = GAME_OVER_STATE;
                pause();
                gLevelupTexture.loadFromRenderedText(gFont, "SCORE: ", &titleColor);
                gLevelupTexture2.loadFromRenderedText(gFont, "TIME: ", &titleColor);
                titleTimer.start();
                return;
            }
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
        enemyCount++;
    }

    if(enemyCount < currentLevel && score < levelScores[currentLevel - 1] - enemyCount)
	{
		for(int i = 0; i < MAX_ENEMIES; i++)
		{
			if(!enemies[i].getAlive())
			{
				generateNewEnemy(i);
				break;
			}
		}
	}


    if(gunTimer.getTicks() >= currentFireRate && (gTurret.isStopped() || autofireEnabled))	//time to fire the gun again
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
                bullets[i].setRicochet(ricochetEnabled);
//				bullets[i].move();
                if(!mute)
                {
                    Mix_PlayChannel(-1, gGunSound, 0);
                }
                break;
            }
        }
    }

//	if((easyMode && score >= 3 * currentLevel) || (score >= 2.5 * (currentLevel * currentLevel + currentLevel)))
    if((easyMode && score >= 3 * currentLevel) || (score >= levelScores[currentLevel - 1]))
    {
        pause();
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

void updatePausedState()
{
    return;
}

void updateGameoverState()
{
    if(titleTimer.getTicks() > 5000)
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
    case GAME_PAUSED_STATE:
        renderPausedState();
        break;
    case GAME_OVER_STATE:
        renderGameoverState();
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
        gLevelupTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
        opacity = 255 - (255 * timeLapse / (TITLE_SCREEN_DURATION / 2));
        SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
        SDL_RenderFillRect(gRenderer, NULL);
    }
    else if(timeLapse < TITLE_SCREEN_DURATION)
    {
        gLevelupTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
        opacity = (255 * timeLapse / (TITLE_SCREEN_DURATION / 2) - 255);
        SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
        SDL_RenderFillRect(gRenderer, NULL);
    }
    else if(timeLapse < TITLE_SCREEN_DURATION * 1.5)
    {
        gLogoTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
        opacity = (255 * (timeLapse - TITLE_SCREEN_DURATION) / (TITLE_SCREEN_DURATION / 2) - 255);
        SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, opacity);
        SDL_RenderFillRect(gRenderer, NULL);
    }
    else if(timeLapse < TITLE_SCREEN_DURATION * 2)
    {
        gLogoTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
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
            switch(enemies[i].getType())
            {
            case 1:
                gEnemy1Sheet.render(enemies[i].getXPos(), enemies[i].getYPos(), enemies[i].getState(), enemies[i].getXPos() < SCREEN_WIDTH / 2 ? enemies[i].getState() * -5 : enemies[i].getState() * 5);
                break;
            case 2:
                gEnemy2Sheet.render(enemies[i].getXPos(), enemies[i].getYPos(), enemies[i].getState(), enemies[i].getXPos() < SCREEN_WIDTH / 2 ? enemies[i].getState() * -5 : enemies[i].getState() * 5);
                break;
            case 3:
                gEnemy3Sheet.render(enemies[i].getXPos(), enemies[i].getYPos(), enemies[i].getState(), enemies[i].getXPos() < SCREEN_WIDTH / 2 ? enemies[i].getState() * -5 : enemies[i].getState() * 5);
                break;
            case 4:
                gEnemy4Sheet.render(enemies[i].getXPos(), enemies[i].getYPos(), enemies[i].getState(), enemies[i].getXPos() < SCREEN_WIDTH / 2 ? enemies[i].getState() * -5 : enemies[i].getState() * 5);
                break;
            case 5:
                gEnemy5Sheet.render(enemies[i].getXPos(), enemies[i].getYPos(), enemies[i].getState(), enemies[i].getXPos() < SCREEN_WIDTH / 2 ? enemies[i].getState() * -5 : enemies[i].getState() * 5);
                break;
            }
        }
    }

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].getAlive())
        {
            gBulletSheet.render(bullets[i].getXPos(), bullets[i].getYPos(), bulletMode, 0);
        }
    }
    gTurretTexture.render(gTurret.getXPos(), gTurret.getYPos(), gTurretTexture.getFrameCount() * gunTimer.getTicks() / currentFireRate, gTurret.getAngle());
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
    if(titleTimer.getTicks() <= 1500)
    {
        gLevelupTexture.setAlphaMod(titleTimer.getTicks() / 1500.0 * 255);
        gLevelupTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
    }
    else if(titleTimer.getTicks() < 3000)
    {
        gLevelupTexture2.setAlphaMod((titleTimer.getTicks() / 1500.0 - 1.0) * 255);
        gLevelupTexture2.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
    }
    else
    {
        SDL_Rect highlighter;
        gTurretTexture.render(gCursorTurret.getXPos(), gCursorTurret.getYPos(), 0, gCursorTurret.getAngle());
        levelupChoice2->render(gCursorTurret.getXPos() + 100, gCursorTurret.getYPos(), 0, 0);
        levelupChoice1->render(gCursorTurret.getXPos() - 100, gCursorTurret.getYPos(), 0, 0);
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
        SDL_SetRenderDrawColor(gRenderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
        SDL_RenderDrawRect(gRenderer, &highlighter);
        SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    }
    SDL_RenderPresent(gRenderer);
}

void renderPausedState()
{
    SDL_Rect pausePromptLocation{(SCREEN_WIDTH - gPausedTexture.getWidth()) / 2 - 30, (SCREEN_HEIGHT - gPausedTexture.getHeight()) / 2 - 30, gPausedTexture.getWidth() + 60, gPausedTexture.getHeight() + 60};

    SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(gRenderer);

    gBoxTexture.render(0, &pausePromptLocation, 0);
    gPausedTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);

    renderHud();

    SDL_RenderPresent(gRenderer);
}

void renderGameoverState()
{
    int gameoverTimer = titleTimer.getTicks();
    SDL_SetRenderDrawColor(gRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(gRenderer);

    if(gameoverTimer > 1000)
    {
        gLevelupTexture.render((SCREEN_WIDTH - gLevelupTexture.getWidth()) / 2, SCREEN_HEIGHT / 2 - 60, 0, 0);
    }
    if(gameoverTimer > 2000)
    {
        gScoreTexture.render((SCREEN_WIDTH + gScoreTexture.getWidth()) / 2, SCREEN_HEIGHT / 2 - 60, 0, 0);
    }
    if(gameoverTimer > 3000)
    {
        gLevelupTexture2.render((SCREEN_WIDTH - gLevelupTexture2.getWidth()) / 2, SCREEN_HEIGHT / 2, 0, 0);
    }
    if(gameoverTimer > 4000)
    {
        gTimerTexture.render((SCREEN_WIDTH + gTimerTexture.getWidth()) / 2, SCREEN_HEIGHT / 2, 0, 0);
    }
    if(gameoverTimer > 5000)
    {
        gTurretTexture.render(gCursorTurret.getXPos(), gCursorTurret.getYPos(), 0, gCursorTurret.getAngle());
        gPlayAgainTexture.render(gCursorTurret.getXPos() + 100, gCursorTurret.getYPos(), 0, 0);
        gQuitTexture.render(gCursorTurret.getXPos() - 100, gCursorTurret.getYPos(), 0, 0);
    }

    SDL_RenderPresent(gRenderer);

}

void generateNewEnemy(int index)
{
    if(index < 0 || index >= MAX_ENEMIES)
    {
        return;
    }

//	int wall = rand() % 4;
//	int location = rand() % 9 + 1;
//	int velocityMod = rand() % 100;
    int enemySeed = rand();
    switch(enemySeed % 4)
    {
    case 0: //north wall
        enemies[index].setPosition(SCREEN_WIDTH * (float)(enemySeed % 9) / 10, -40);
        break;
    case 1: //east wall
        enemies[index].setPosition(SCREEN_WIDTH + 40, SCREEN_HEIGHT * (float)(enemySeed & 9) / 10);
        break;
    case 2: //south wall
        enemies[index].setPosition(SCREEN_WIDTH * (float)(enemySeed % 9) / 10, SCREEN_HEIGHT + 40);
        break;
    case 3: //west wall
        enemies[index].setPosition(-40, SCREEN_HEIGHT * (float)(enemySeed % 9) / 10);
        break;
    }


    enemies[index].setAlive(currentEnemyHealth);
    enemies[index].setVelocity((gTurret.getXPos() - enemies[index].getXPos()) / (currentEnemySpeed + enemySeed % 100), (gTurret.getYPos() - enemies[index].getYPos()) / (currentEnemySpeed + enemySeed % 100));
    enemies[index].setType(enemySeed % ((currentLevel - 1) / 2 + 1) + 1);
//	printf("enemyseed %d currentlevel %d\n", enemySeed, currentLevel);
//	printf("ran setType(%d)\n", enemySeed % ((currentLevel - 1) / 2 + 1));
//	printf("created enemy type %d\n", enemies[index].getType());
}

void renderHud()
{
    SDL_Rect healthLocation{10, 10, 175, 50};
    SDL_Rect scoreLocation{(SCREEN_WIDTH - gScoreTexture.getWidth()) / 2 - 80, 10, gScoreTexture.getWidth() + 60, 50};
    SDL_Rect levelLocation{(SCREEN_WIDTH - gLevelTexture.getWidth()) / 2 + 60, 10, gLevelTexture.getWidth() + 60, 50};
//	SDL_Rect timerLocation{SCREEN_WIDTH - gTimerTexture.getWidth() - 70, 10, gTimerTexture.getWidth() + 60, 50};
    SDL_Rect timerLocation{SCREEN_WIDTH - 185, 10, 175, 50};
    SDL_RenderSetViewport(gRenderer, &healthLocation);
    gBoxTexture.render(0, NULL, 0);

    for(int i = 0; i < health / 2; i++)
    {
        gEnergyTexture.render(30 + i * 29, 25, 0, 0);
    }
    if(health % 2)
    {
        gHalfEnergyTexture.render(30 + health / 2 * 29, 25, 0, 0);
    }

    SDL_RenderSetViewport(gRenderer, &scoreLocation);
    gBoxTexture.render(0, NULL, 0);
    gScoreTexture.render(scoreLocation.w / 2, scoreLocation.h / 2, 0, 0);

    SDL_RenderSetViewport(gRenderer, &levelLocation);
    gBoxTexture.render(0, NULL, 0);
    gLevelTexture.render(levelLocation.w / 2, levelLocation.h / 2, 0, 0);

    SDL_RenderSetViewport(gRenderer, &timerLocation);
    gBoxTexture.render(0, NULL, 0);
    gTimerTexture.render(timerLocation.w / 2, timerLocation.h / 2, 0, 0);

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
//	printf("reached score %d, increasing level from %d to %d\n", score, currentLevel - 1, currentLevel);
    for(int i = 0; i < currentLevel && i < MAX_ENEMIES; i++)
    {
//		printf("generated enemy %d of %d\n", i, currentLevel);
        generateNewEnemy(i);
    }
    sprintf(levelString, "%d", currentLevel);
    gLevelTexture.loadFromRenderedText(gFont, levelString, &scoreColor);
    sprintf(levelString, "Entering level %d", currentLevel);
    gLevelupTexture.loadFromRenderedText(gFont, levelString, &titleColor);
    gLevelupTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    switch(currentLevel)
    {
    case 2:
        gLevelupTexture2.loadFromRenderedText(gFont, "Stronger enemies. Faster Enemies.", &titleColor);
        currentEnemyHealth += 2;
        currentEnemySpeed -= 20;
        levelupChoice1 = &gSpeedUpTexture;
        levelupChoice2 = &gPowerUpTexture;
        break;
    case 3:
        gLevelupTexture2.loadFromRenderedText(gFont, "Even more enemies. Stronger and faster!", &titleColor);
        currentEnemyHealth += 3;
        currentEnemySpeed -= 20;
        levelupChoice1 = &gSpeedUpTexture;
        levelupChoice2 = &gPowerUpTexture;
        break;
    case 4:
        gLevelupTexture2.loadFromRenderedText(gFont, "Tutorial over. Things are going to get serious!", &titleColor);
        levelupChoice1 = &gSpeedUpTexture;
        levelupChoice2 = &gPowerUpTexture;
        break;
    case 5:
        gLevelupTexture2.loadFromRenderedText(gFont, "Still ok? Things are only going to get harder.", &titleColor);
        currentEnemyHealth += 2;
        levelupChoice1 = &gFireRateUpTexture;
        levelupChoice2 = &gEnemySpeedDownTexture;
        break;
    case 6:
        gLevelupTexture2.loadFromRenderedText(gFont, "A cool power-up? Or a heal, if your health is low.", &titleColor);
        currentEnemyHealth += 1;
        currentEnemySpeed -= 10;
        levelupChoice1 = &gFullHealTexture;
        levelupChoice2 = &gRicochetPowersTexture;
        break;
    case 7:
        gLevelupTexture2.loadFromRenderedText(gFont, "I think they're getting angry. What did you do?", &titleColor);
        levelupChoice1 = &gSpeedUpTexture;
        levelupChoice2 = &gPowerUpTexture;
        break;
    case 8:
        gLevelupTexture2.loadFromRenderedText(gFont, "Good job making it this far. Can you keep it up?", &titleColor);
        levelupChoice1 = &gFireRateUpTexture;
        levelupChoice2 = &gPowerUpTexture;
        break;
    case 9:
        gLevelupTexture2.loadFromRenderedText(gFont, "You're almost done. Do you deserve another cool power?", &titleColor);
        levelupChoice1 = &gFullHealTexture;
        levelupChoice2 = &gAutofirePowersTexture;
        break;
    case 10:
        gLevelupTexture2.loadFromRenderedText(gFont, "Last level. I don't think you can make it to 300.", &titleColor);
        levelupChoice1 = &gFireRateUpTexture;
        levelupChoice2 = &gSpeedUpTexture;
        break;
    default:
        break;
    }

    titleTimer.start();
}

void pause()
{
    playTimer.pause();
    gunTimer.pause();
}

void unpause()
{
    playTimer.unpause();
    gunTimer.unpause();
}
