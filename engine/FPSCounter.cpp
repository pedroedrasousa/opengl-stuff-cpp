
#include "FPSCounter.h"

#include "Timer.h"

FPSCounter::FPSCounter()
{
	m_framesRendered = 0;
	m_elapsedTime = 0.0f;
}

void  FPSCounter::MarkFrameStart()
{
	m_timer.Start();
}

void  FPSCounter::MarkFrameEnd()
{
	m_frameInterval = m_timer.Stop();

	m_elapsedTime += m_frameInterval;
	m_framesRendered++;

	if(m_elapsedTime >= 1.0f)
	{
		m_fps				= m_framesRendered / m_elapsedTime;
		m_framesRendered	= 0;
		m_elapsedTime		= 0.0f;
    }

}

