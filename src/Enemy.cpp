#include "Enemy.h"

Enemy::Enemy()
{
    kill();
    //ctor
}

Enemy::~Enemy()
{
    //dtor
}

void Enemy::move()
{
    if(alive)
    {
        xPos += xVel;
        yPos += yVel;
        state--;
    }
}

void Enemy::kill()
{
    health = xPos = yPos =  xVel =  yVel = -100;
    alive = false;
}

double Enemy::getXPos()
{
    return xPos;
}

double Enemy::getYPos()
{
    return yPos;
}

double Enemy::getXVel()
{
    return xVel;
}

double Enemy::getYVel()
{
    return yVel;
}

void Enemy::setVelocity(double x, double y)
{
    xVel = x;
    yVel = y;
}

void Enemy::setPosition(double x, double y)
{
    xPos = x;
    yPos = y;
}

bool Enemy::getAlive()
{
    return alive;
}

void Enemy::setAlive(int value)
{
    health = value;
    alive = true;
}

int Enemy::getHealth()
{
    return health;
}

bool Enemy::damage(int amount)
{
    if(alive)
    {
        health -= amount;
        if(health <= 0)
        {
            kill();
            return true;
        }
        state = 12;
        xPos -= xVel * 10;
        yPos -= yVel * 10;
        if(type == 3)
        {
        	xVel *= .8;
        	yVel *= .8;
        }
    }
    return false;
}

int Enemy::getSize()
{
    switch(type)
    {
    case 1:
    case 2:
    case 3:
        return 32;
    case 4:
        return 16;
    case 5:
        return 48;
    default:
        return 0;

    }
    return 0;
}

int Enemy::getState()
{
    if(state < 0)
    {
        return 0;
    }
    return state / 4;
}

void Enemy::setType(int value)
{
    if(value > 5)
    {
        value = 5;
    }
    else if(value < 1)
    {
        value = 1;
    }
    type = value;
    switch(type)
    {
    case 1:
        return;
    case 2:
		health *= 1.3;
		xVel *= .6;
		yVel *= .6;
		return;
	case 3:
		health *= 2;
		xVel *= .5;
		yVel *= .5;
		return;
	case 4:
		health = 1;
		return;
	case 5:
		health *= 2.5;
		xVel *= .2;
		yVel *= .2;
		return;
    }
}

int Enemy::getType()
{
    return type;
}
