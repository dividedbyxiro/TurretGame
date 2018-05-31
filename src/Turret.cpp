#include "Turret.h"
#include <cmath>

Turret::Turret()
{
	angle = 0;
	xPos = 0;
	yPos = 0;
	width = 32;
	height = 32;
	stopped = false;
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
		angle = std::fmod(angle, 360);
		//angle %= 360.0;
	}
}

void Turret::stop()
{
	if(stopped)
	{
		return;
	}
	stopped = true;
	direction *= -1;
}

void Turret::start()
{
	stopped = false;
}

void Turret::setPosition(int x, int y)
{
	xPos = x;
	yPos = y;
}

double Turret::getAngle()
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

void Turret::setDirection(double newDirection)
{
	direction = newDirection;
}

double Turret::getDirection()
{
	return direction;
}

bool Turret::isStopped()
{
	return stopped;
}
