#include "HPRTimer.h"

HPRTimer::HPRTimer()
{
	// Query performance hardware and setup time scaling values
	if (QueryPerformanceFrequency((LARGE_INTEGER * ) &m_Frequency))
	{
		m_PerfHardware = true;
		QueryPerformanceCounter((LARGE_INTEGER * ) &m_LastTime);
		m_TimeScale = 1.0f/ m_Frequency;
	}
	else
	{
		// No hardware counter available, default to using timeGetTime
		m_PerfHardware = false;
		m_LastTime = timeGetTime();
		m_TimeScale = 0.001f;
	}

	m_SampleCount = 0;
	m_FrameRate = 0;
	m_FPSFrameCount = 0;
	m_FPSTimeElapsed = 0.0f;
}

void HPRTimer::Tick( float fLockFPS )
{
	float TimeElapsed; 

	// Is performance hardware is available, query it
	if ( m_PerfHardware ) 

		QueryPerformanceCounter((LARGE_INTEGER *)&m_CurrentTime);
	else 

		m_CurrentTime = timeGetTime();

	// Calculate elapsed time in seconds
	TimeElapsed = (m_CurrentTime - m_LastTime) * m_TimeScale;

	// Should we lock the frame rate ?
	if ( fLockFPS > 0.0f )
	{
		while ( TimeElapsed < (1.0f / fLockFPS))
		{
			if ( m_PerfHardware ) 
				QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTime);
			else 
				m_CurrentTime = timeGetTime();

			// Calculate elapsed time in seconds
			TimeElapsed = (m_CurrentTime - m_LastTime) * m_TimeScale;
		}
	}

	m_LastTime = m_CurrentTime;

	// Filter out values wildly different from current average
	if ( fabs(TimeElapsed - m_TimeElapsed) < 1.0f  )
	{
		// Wrap FIFO frame time buffer.
		memmove( &m_FrameTime[1], m_FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float) );
		m_FrameTime[ 0 ] = TimeElapsed;
		if ( m_SampleCount < MAX_SAMPLE_COUNT ) m_SampleCount++;

	}


	// Calculate Frame Rate
	m_FPSFrameCount++;
	m_FPSTimeElapsed += TimeElapsed;
	if ( m_FPSTimeElapsed > 1.0f) 
	{
		m_FrameRate                     = m_FPSFrameCount;
		m_FPSFrameCount         = 0;
		m_FPSTimeElapsed        = 0.0f;
	}

	// Count up the new average elapsed time
	m_TimeElapsed = 0.0f;
	for ( ULONG i = 0; i < m_SampleCount; i++ ) m_TimeElapsed += m_FrameTime[ i ];
	if ( m_SampleCount > 0 ) m_TimeElapsed /= m_SampleCount;
}

unsigned long HPRTimer::GetFrameRate( LPTSTR lpszString ) const
{
	// Fill string buffer ?
	if ( lpszString )
	{
		// Copy frame rate value into string
		_itot( m_FrameRate, lpszString, 10 );

		// Append with FPS
		_tcscat( lpszString, _T(" FPS") );

	} // End if build FPS string

	return m_FrameRate;
}