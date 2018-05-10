#include "scoreTable.h"
#include <SDL.h>
#include <stdio.h>

scoreTable::scoreTable()
{
	for(int i = 0; i < 10; i++)
	{
		scores[i] = 0;
		dates[i] = 0;
		times[i] = 0;
	}
	//ctor
}

scoreTable::~scoreTable()
{
	//dtor
}

time_t scoreTable::getDate(int index)
{
	return dates[index];
}

int scoreTable::getScore(int index)
{
	return scores[index];
}

int scoreTable::getTime(int index)
{
	return times[index];
}

void scoreTable::setDate(int index, time_t date)
{
	dates[index] = date;
}

void scoreTable::setScore(int index, int score)
{
	scores[index] = score;
}

void scoreTable::setTime(int index, int time)
{
	times[index] = time;
}

bool scoreTable::setTable(SDL_RWops *file)
{
	bool ret = true;
	int index = 0;

	for(index = 0; index < 10; index++)
	{
		if(SDL_RWread(file, &dates[index], sizeof(dates[index]), 1) == 0)
		{
			dates[index] = 0;
			ret = false;
			break;
		}
		if(SDL_RWread(file, &scores[index], sizeof(scores[index]), 1) == 0)
		{
			scores[index] = 0;
			ret = false;
			break;
		}
		if(SDL_RWread(file, &times[index], sizeof(times[index]), 1) == 0)
		{
			times[index] = 0;
			ret = false;
			break;
		}
	}

	for(; index < 10; index++)
	{
		dates[index] = 0;
		scores[index] = 0;
		times[index] = 0;
	}
	return ret;
}

bool scoreTable::updateTableFile(SDL_RWops *file)
{
	bool ret = true;
	int index = 0;

	for(index = 0; index < 10; index++)
	{
		if(SDL_RWwrite(file, &dates[index], sizeof(dates[index]), 1) == 0)
		{
			ret = false;
			printf("error updating score file date %d error %s\n", index, SDL_GetError());
			break;
		}
		if(SDL_RWwrite(file, &scores[index], sizeof(scores[index]), 1) == 0)
		{
			ret = false;
			printf("error updating score file score %d error %s\n", index, SDL_GetError());
			break;
		}
		if(SDL_RWwrite(file, &times[index], sizeof(times[index]), 1) == 0)
		{
			ret = false;
			printf("error updating score file time %d error %s\n", index, SDL_GetError());
			break;
		}
	}
	return ret;
}

bool scoreTable::submitNewScore(time_t date, int score, int time)
{
	int index = -1;

	for(int i = 0; i < 10; i++)
	{
		if(score >= scores[i])
		{
			index = i;
			printf("new score at %d\n", index);
			break;
		}
	}
	if(index == -1)
	{
		return false;
	}

	for(int i = index; i < 10; i++)
	{
		if(score > scores[i])
		{
			index = i;
			printf("new score at %d\n", index);
			break;
		}
		if(score == scores[i] && time < times[i])
		{
			index = i;
			printf("new score at %d\n", index);
			break;
		}
		if(i == 9)
		{
			index = -1;
			break;
		}
	}
	if(index == -1)
	{
		return false;
	}

	for(int i = 9; i > index; i--)
	{
		dates[i] = dates[i - 1];
		scores[i] = scores[i - 1];
		times[i] = times[i - 1];
	}
	dates[index] = date;
	scores[index] = score;
	times[index] = time;
	return true;
}
