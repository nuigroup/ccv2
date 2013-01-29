/** 
 * \file      nuiTimer.h 
 * \author    Anatoly Churikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_TIMER_H
#define NUI_TIMER_H

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



#endif// NUI_TIMER_H