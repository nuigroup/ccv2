#include "nuiTimer.h"

nuiTimer::nuiTimer()
{
	Reset();
}

void nuiTimer::Start()
{
	_last_time = nuiUtils::time();
}

void nuiTimer::Wait()
{
	double curtime = nuiUtils::time();
	_wait_time += curtime - _last_time;
	_last_time = curtime;
}

void nuiTimer::Process()
{
	double curtime = nuiUtils::time();
	_process_time += curtime - _last_time;
	_last_time = curtime;
	_process_frame ++;
	// calculate average fps every 1s
	if ( (_process_time + _wait_time) > 1. )
	{
		// average is on the last period
		average_process_time	= _process_time / (double)_process_frame;
		average_wait_time	= _wait_time / (double)_process_frame;
		// calculate fps
		average_fps			= (double)_process_frame / (_process_time + _wait_time);
		// update totals
		total_wait_time		+= _wait_time;
		total_process_time	+= _process_time;
		total_process_frame	+= _process_frame;
		// reset period
		_process_frame		= 0;
		_process_time		= 0;
		_wait_time			= 0;
	}
}

void nuiTimer::Reset()
{
		_process_frame		= 0;
		_process_time		= 0;
		_wait_time			= 0;
		average_process_time= 0;
		average_wait_time	= 0;
		average_fps			= 0;
		total_wait_time		= 0;
		total_process_time	= 0;
		total_process_frame	= 0;
}

double nuiTimer::getAverageFPS()
{
	return average_fps;
}

double nuiTimer::getAverageWaitTime()
{
	return average_wait_time;
}

double nuiTimer::getAverageProcessTime()
{
	return average_process_time;
}



