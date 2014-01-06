#ifndef __FPSCOUNTER_H__
#define __FPSCOUNTER_H__

#include "Timer.h"

class FPSCounter
{
public:
	FPSCounter();

	float GetFPS()				const { return m_fps; };
	float GetFrameInterval()	const { return m_frameInterval; };

	void  MarkFrameStart();
	void  MarkFrameEnd();

private:
	Timer m_timer;

	int		m_framesRendered;
	float	m_elapsedTime;
	float	m_fps;
	float	m_frameInterval;

};

#endif /*__FPSCOUNTER_H__*/
