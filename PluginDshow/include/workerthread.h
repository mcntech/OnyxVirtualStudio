#ifndef _WorkerThread_h
#define _WorkerThread_h

#include "Async.h"

class VideoClip;

/**
	This is the worker thread, requests work from VideoManager
	and decodes assigned VideoClip objects
*/
class WorkerThread : public Thread
{
	VideoClip* mClip;
public:
	WorkerThread();
	~WorkerThread();

	VideoClip* getAssignedClip() { return mClip; }

    //! Main Thread Body - do not call directly!
	void executeThread();
};
#endif
