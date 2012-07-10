/* nuiTimer.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_TIMER_H_
#define _NUI_TIMER_H_

#include "nuiUtils.h"

class nuiTimer
{
public:
	nuiTimer();
	void Start();
	void Wait();
	void Process();
	void Reset();
	double getAverageFPS();
	double getAverageWaitTime();
	double getAverageProcessTime();
private:
	double average_fps;
	double average_process_time;
	double average_wait_time;
	double total_process_time;
	double total_wait_time;
	unsigned long long total_process_frame;
	unsigned long long _process_frame;
	double _process_time;
	double _wait_time;
	double _last_time;
};



#endif//_NUI_TIMER_H_