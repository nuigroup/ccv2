/** 
 * \file      nuiThread.cpp
 *            
 * Basic abstraction of thread/semaphore for WIN32 and Posix
 *            
 * Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * \author    Scott Halstvedt
 * \author    Anatoly Lushnikov
 * \date      2011-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#include "nuiThread.h"

nuiThread::nuiThread(thread_process_t _process, void *_userdata) : pt::thread(false)
{
	this->process = _process;
	this->userdata = _userdata;
	this->want_quit = false;
	this->thread_trigger = new pt::trigger(true,false);
}

nuiThread::~nuiThread() 
{
	delete this->thread_trigger;
}

void nuiThread::execute() 
{
	this->process(this);
}

void *nuiThread::getUserData() 
{
	return this->userdata;
}

void nuiThread::stop() 
{
	this->want_quit = true;
}

bool nuiThread::wantQuit() 
{
	return this->want_quit;
}

bool nuiThread::relax(int m) 
{
	return pt::thread::relax(m);
}

void nuiThread::wait()
{
	this->thread_trigger->wait();
}

void nuiThread::post()
{
	this->thread_trigger->post();
}

// void nuiMultithreadable::lock()
// {
// 	mtx->lock();
// }
// 
// void nuiMultithreadable::unlock()
// {
// 	mtx->unlock();
// }
// 
// nuiMultithreadable::nuiMultithreadable()
// {
// 	mtx = new pt::mutex();
// }
// 
// nuiMultithreadable::~nuiMultithreadable()
// {
// 	delete mtx;
// }
