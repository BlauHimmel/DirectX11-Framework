#pragma once

#ifndef __GAME_TIMER_H__
#define __GAME_TIMER_H__

#include <Windows.h>

class GameTimer
{
public:

	GameTimer();

public:

	float TotalTime() const;
	float DeltaTime() const; 

	void Reset();
	void Start(); 
	void Stop();  
	void Tick();  

private:

	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_bStopped;
};

#endif