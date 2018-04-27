#ifndef TIMER_H
#define TIMER_H


class Timer
{
public:
	//Initializes variables
	Timer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	unsigned long getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	unsigned long mStartTicks;

	//The ticks stored when the timer was paused
	unsigned long mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};


#endif // TIMER_H
