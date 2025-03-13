﻿#pragma once

const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

public:
	void Start();
	void Stop();
	void Reset();
	void Tick(float lockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = nullptr, int nChar = 0);
	float GetTimeElapsed() const;

private:
	bool	m_bHardwareHasPerformanceCounter;
	float	m_fTimeScale;
	float	m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;
	__int64 m_nPauseTime;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long	m_nCurrentFrameRate;
	unsigned long	m_nFramesPerSecond;
	float			m_fFPSTimeElapsed;

	bool m_bStopped;
};

