#ifndef Timers_H
#define Timers_H

#include "SDL.h"

Uint32 InSeconds(int Seconds);
Uint32 InSeconds(double Seconds);
Uint32 InMinutes(int Minutes);
Uint32 InMinutes(double Minutes);

class Timer {
public:
	Timer(Uint32 Duration);
	Timer();
	void ChangeDelay(Uint32 Duration);
	Uint32 GetDelay();
	Uint32 TimeLeft();
	bool Ready();
	bool Active();
	void Activate();
	void Deactivate();
	
private:
	Uint32 LastT;
	Uint32 Delay;
	bool ACTIVE;
};


#endif
