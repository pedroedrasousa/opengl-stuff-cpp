
#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>

class Timer
{
public:

	Timer()
  {
	Initialize();
  }

  void Initialize()
  {
    QueryPerformanceFrequency(&m_frequency);
  }

  void Start()
  {
    QueryPerformanceCounter(&m_start);
  }
  
  float Stop()
  {
    // Return duration in seconds...
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return float(end.QuadPart - m_start.QuadPart) / m_frequency.QuadPart;
  }

private:
  
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_start;
};

#endif /*__TIMER_H__*/