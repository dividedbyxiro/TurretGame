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

void Enemy::setAlive()
{
	health = 3;
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
	}
	return false;
}
