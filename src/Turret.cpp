#include "Turret.h"

Turret::Turret()
{
	angle = 0;
	health = 10;
	xPos = 0;
	yPos = 0;
	width = 32;
	height = 32;
	stopped = false;
	direction = 1;
    //ctor
}

Turret::~Turret()
{
    //dtor
}

void Turret::move()
{
	if(!stopped)
	{
		angle += direction;
		angle %= 360;
	}
}

void Turret::setStopped(bool instruction)
{
	stopped = instruction;
	if(stopped == true)
	{
		direction *= -1;
	}
}

void Turret::setPosition(int x, int y)
{
	xPos = x;
	yPos = y;
}

int Turret::getHealth()
{
	return health;
}

int Turret::getAngle()
{
	return angle;
}

int Turret::getXPos()
{
	return xPos;
}

int Turret::getYPos()
{
	return yPos;
}

int Turret::getWidth()
{
	return width;
}

int Turret::getHeight()
{
	return height;
}
