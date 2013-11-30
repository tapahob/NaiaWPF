#include "precompiled.h"
#include "NaiaTimer.h"

Time::Time()
{
}

Time::Time(const Time& other)
{

}

Time::~Time()
{
}

bool Time::Initialize()
{
	// check for support of HP timers
	QueryPerformanceFrequency((LARGE_INTEGER*) &m_frequency);
	if (m_frequency == 0)
		return false;

	m_ticksPerMs = (float) (m_frequency / 1000);
	QueryPerformanceCounter((LARGE_INTEGER*) &m_startTime);
	return true;
}

void Time::Update()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);
	timeDifference = (float) (currentTime - m_startTime);
	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
}

float Time::DeltaTime()
{
	return m_frameTime;
}
