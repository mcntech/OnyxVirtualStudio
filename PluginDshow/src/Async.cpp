#include <stdio.h>
#include "Async.h"

#ifdef _WIN32
#include <windows.h>

unsigned long WINAPI Async_Call(void* param)
{
#else
void *Async_Call(void* param)
{
#endif

	Thread* t=(Thread*) param;
	t->executeThread();
#ifndef _WIN32
    pthread_exit(NULL);
#endif
	return 0;
}


Mutex::Mutex()
{
#ifdef _WIN32
	mHandle=0;
#else
    pthread_mutex_init(&mHandle,0);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
	if (mHandle) CloseHandle(mHandle);
#else
    pthread_mutex_destroy(&mHandle);
#endif
}

void Mutex::lock()
{
#ifdef _WIN32
	if (!mHandle) mHandle=CreateMutex(0,0,0);
	WaitForSingleObject(mHandle,INFINITE);
#else
    pthread_mutex_lock(&mHandle);
#endif
}

void Mutex::unlock()
{
#ifdef _WIN32
	ReleaseMutex(mHandle);
#else
    pthread_mutex_unlock(&mHandle);
#endif
}


Thread::Thread()
{
	mThreadRunning=false;
	mHandle=0;
}

Thread::~Thread()
{
#ifdef _WIN32
	if (mHandle) CloseHandle(mHandle);
#endif
}

void Thread::startThread()
{
	mThreadRunning=true;

#ifdef _WIN32
	mHandle=CreateThread(0,0,&Async_Call,this,0,0);
#else
    int ret=pthread_create(&mHandle,NULL,&Async_Call,this);
    if (ret) printf("ERROR: Unable to create thread!\n");
#endif
}

void Thread::waitforThread()
{
	mThreadRunning=false;
#ifdef _WIN32
	WaitForSingleObject(mHandle,INFINITE);
	if (mHandle) { CloseHandle(mHandle); mHandle=0; }
#else
    pthread_join(mHandle,0);
#endif
}
