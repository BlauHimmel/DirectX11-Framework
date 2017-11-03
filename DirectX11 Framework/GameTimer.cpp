#include "GameTimer.h"

GameTimer::GameTimer() : 
	m_SecondsPerCount(0.0),
	m_DeltaTime(-1.0), 
	m_BaseTime(0),
	m_PausedTime(0),
	m_PrevTime(0), 
	m_CurrTime(0), 
	m_bStopped(false)
{
	__int64 CountsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)(&CountsPerSec));
	m_SecondsPerCount = 1.0 / static_cast<double>(CountsPerSec);
}


float GameTimer::TotalTime() const
{
	//                     |<--paused time-->|						   ¡ý
	// ----*---------------*-----------------*------------*------------*------> time
	//  m_BaseTime       m_StopTime        StartTime     m_StopTime    m_CurrTime
	if (m_bStopped)
	{
		return static_cast<float>(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}

	//                     |<--paused time-->|			  ¡ý
	// ----*---------------*-----------------*------------*------> time
	//  m_BaseTime       m_StopTime        StartTime     m_CurrTime
	else
	{
		return static_cast<float>(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return static_cast<float>(m_DeltaTime);
}

void GameTimer::Reset()
{
	__int64 CurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)(&CurrTime));

	m_BaseTime = CurrTime;
	m_PrevTime = CurrTime;
	m_StopTime = 0;
	m_bStopped = false;
}

void GameTimer::Start()
{
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  m_BaseTime       m_StopTime        StartTime     
	if (m_bStopped)
	{
		__int64 StartTime;
		QueryPerformanceCounter((LARGE_INTEGER*)(&StartTime));

		m_PausedTime += (StartTime - m_StopTime);

		m_PrevTime = StartTime;
		m_StopTime = 0;
		m_bStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_bStopped)
	{
		__int64 CurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)(&CurrTime));

		m_StopTime = CurrTime;
		m_bStopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_bStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 CurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)(&CurrTime));

	m_CurrTime = CurrTime;
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;
	m_PrevTime = m_CurrTime;

	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}
