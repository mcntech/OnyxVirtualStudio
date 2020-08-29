#ifdef _WIN32
#pragma warning( disable: 4251 ) // MSVC++
#endif
#include "WorkerThread.h"
#include "VideoManager.h"
#include "VideoClip.h"
#include "Util.h"


WorkerThread::WorkerThread() : Thread()
{
	mClip=NULL;
}

WorkerThread::~WorkerThread()
{

}

void WorkerThread::executeThread()
{
	mThreadRunning = true;
	while (mThreadRunning)	{
		mClip=VideoManager::getSingleton().requestWork(this);
		if (!mClip)	{
			_psleep(1);
			continue;
		}

		mClip->decodeNextFrame();

		mClip->mAssignedWorkerThread=NULL;
		mClip=0;
		_psleep(1);
	}
}
