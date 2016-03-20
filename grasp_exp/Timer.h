#ifndef _TIMER
#define _TIMER

#include <windows.h>

class Timer
{
public:
	Timer(){ reset(); }
	
	void reset()
	{
		nFreq.QuadPart = 0;
		nBefore.QuadPart = 0;
		nAfter.QuadPart = 0;
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nBefore);
		time = 0.0;
	}

	double get()
	{
		// ŽžŠÔŒv‘ª
		QueryPerformanceCounter(&nAfter);
		
		if(nFreq.QuadPart != 0)
		{
			time = ((double)(nAfter.QuadPart - nBefore.QuadPart)) / (double)nFreq.QuadPart;
		}
		return time;
	}
	double time;
	double StartTime;

private:
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBefore;
	LARGE_INTEGER nAfter;
};
#endif





