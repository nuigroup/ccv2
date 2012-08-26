/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxThread.h
// Author:      Scott Halstvedt
// Purpose:     Basic abstraction of thread/semaphore for WIN32 and Posix
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef NUI_THREAD_H
#define NUI_THREAD_H

#include "pasync.h"

class nuiThread;

typedef void (*thread_process_t)(nuiThread *thread);

class nuiThread : public pt::thread {
public:
	nuiThread(thread_process_t _process, void *_userdata);
	virtual ~nuiThread();
	void *getUserData();
	void execute();
	void stop();
	void wait();
	void post();
	bool wantQuit();
	bool relax(int);
private:
	thread_process_t process;
	pt::trigger *thread_trigger;
	void *userdata;
	bool want_quit;
};



class nuiMultithreadable
{
public:
	nuiMultithreadable();
	virtual ~nuiMultithreadable();
	virtual void lock();
	virtual void unlock();
protected:
	thread_process_t threaded_func;
	pt::mutex *mtx;
};

#endif
