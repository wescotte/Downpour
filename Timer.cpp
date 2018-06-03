#include "Timer.h"

Timer::Timer()
{
	LastT=SDL_GetTicks();
	ACTIVE=false;
	return;
}

Timer::Timer(Uint32 Duration)
{
	Delay=Duration;
	LastT=SDL_GetTicks();
	ACTIVE=false;
	return;
}

void Timer::ChangeDelay(Uint32 Duration)
{
	Delay=Duration;
	return;
}

Uint32 Timer::GetDelay() {
	return Delay;
}

Uint32 Timer::TimeLeft()
{
    Uint32 now;

    now = SDL_GetTicks();
    if(LastT <= now)
        return 0;
    else
        return LastT - now;
}

bool Timer::Ready()
{
	bool temp=false;
	if (TimeLeft() == 0 && ACTIVE == true) {
		temp=true;
		LastT=LastT+Delay;
	}
	return temp;
}

bool Timer::Active()
{
	return ACTIVE;
}

void Timer::Activate()
{
	ACTIVE=true;
	LastT=SDL_GetTicks()+Delay;
	return;
}

void Timer::Deactivate()
{
	ACTIVE=false;
	return;
}

Uint32 InSeconds(int Seconds)
{
	return Seconds * 1000;
}

Uint32 InSeconds(double Seconds)
{
	return Uint32(Seconds * 1000);
}

Uint32 InMinutes(int Minutes)
{
	return InSeconds(Minutes * 60);
}

Uint32 InMinutes(double Minutes)
{
	return InSeconds(Minutes * 60.0);
}
