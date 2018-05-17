#include "Bullet.h"

Bullet::Bullet()
{
	alive = false;
	xPos = yPos = xVel = yVel = 0;
    //ctor
}

Bullet::~Bullet()
{
    //dtor
}

void Bullet::move()
{
	if(alive)
	{
		xPos += xVel;
		yPos += yVel;
		if(!ricochet)
		{
			if(xPos < 0 || xPos > 640 || yPos < 0 || yPos > 480)
			{
				alive = false;
			}
		}
		else
		{
			if(xPos < 0 || xPos > 640)
			{
				xVel *= -1;
				ricochet = false;
				move();
			}
			if(yPos < 0 || yPos > 480)
			{
				yVel *= -1;
				ricochet = false;
				move();
			}

		}
	}
}

void Bullet::kill()
{
	alive = false;
	xPos = yPos = xVel = yVel = 0;
}

void Bullet::setPosition(double x, double y)
{
	xPos = x;
	yPos = y;
}

void Bullet::setVelocity(double x, double y)
{
	xVel = x;
	yVel = y;
}

void Bullet::setAlive(bool instruction)
{
	alive = instruction;
}

double Bullet::getXPos()
{
	return xPos;
}

double Bullet::getYPos()
{
	return yPos;
}

double Bullet::getXVel()
{
	return xVel;
}

double Bullet::getYVel()
{
	return yVel;
}

bool Bullet::getAlive()
{
	return alive;
}

void Bullet::setRicochet(bool value)
{
	ricochet = value;
}
