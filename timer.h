#ifndef TIMER
#define TIMER

#include<SDL.h>

class Timer
{
private:
	unsigned int pastTicks;
public:
	Timer(){pastTicks=SDL_GetTicks();};
	void setTimer(){pastTicks=SDL_GetTicks();}
	unsigned int getTimeElapsed(){return SDL_GetTicks()-pastTicks;}
};

#endif