#ifndef _Async_h
#define _Async_h

#ifndef _WIN32
#include <pthread.h>
#endif

/**
    This is a Mutex object, used in thread syncronization.
 */
class Mutex
{
protected:
#ifdef _WIN32
	void* mHandle;
#else
    pthread_mutex_t mHandle;
#endif
public:
	Mutex();
	~Mutex();
	//! Lock the mutex. If another thread has lock, the caller thread will wait until the previous thread unlocks it
	void lock();
	//! Unlock the mutex. Use this when you're done with thread-safe sections of your code
	void unlock();
};

/**
    This is a Mutex object, used in thread syncronization.
 */
class Thread
{
protected:
#ifdef _WIN32
	void* mHandle;
#else
    pthread_t mHandle;
#endif
	//! Indicates whether the thread is running. As long as this is true, the thread runs in a loop
	volatile bool mThreadRunning;
public:
	Thread();
	virtual ~Thread();

	//! Creates the thread object and runs it
	void startThread();
	//! The main thread loop function
	virtual void executeThread()=0;
	//! sets mThreadRunning to false and waits for the thread to complete the last cycle
	void waitforThread();

};

#endif
