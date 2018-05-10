#ifndef SCORETABLE_H
#define SCORETABLE_H

#include <ctime>
#include <SDL.h>


class scoreTable
{
	public:
		scoreTable();
		virtual ~scoreTable();
		time_t getDate(int index);
		int getScore(int index);
		int getTime(int index);
		void setDate(int index, time_t date);
		void setScore(int index, int score);
		void setTime(int index, int time);
		bool setTable(SDL_RWops *file);
		bool updateTableFile(SDL_RWops *file);
		bool submitNewScore(time_t date, int score, int time);


	private:
		time_t dates[10];
		int scores[10];
		int times[10];
};

#endif // SCORETABLE_H
