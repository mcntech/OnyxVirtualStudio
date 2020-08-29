#include "VideoManager.h" 
#include "WorkerThread.h" 
#include "VideoClip.h" 
#include "AudioInterface.h" 
#include "Util.h" 
//#include "DataSource.h" 

#define NUM_WORKER_THREADS	4

VideoManager* g_ManagerSingleton=0; 
// declaring function prototype here so I don't have to put it in a header file 
// it only needs to be used by this plugin and called once 
void createYUVtoRGBtables(); 


void videomanager_writelog(std::string output) 
{ 
	Ogre::LogManager::getSingletonPtr()->logMessage("PlugInDshow:" + output);
} 

void (*g_LogFuction)(std::string)=videomanager_writelog;

void VideoManager::logMessage(std::string msg)
{
	g_LogFuction(msg);
}


VideoManager* VideoManager::getSingletonPtr() 
{ 
	return g_ManagerSingleton; 
} 

VideoManager& VideoManager::getSingleton() 
{ 
	return *g_ManagerSingleton; 
} 

VideoManager::VideoManager() : 
		mDefaultNumPrecachedFrames(3) 
{ 
	g_ManagerSingleton=this; 

	mWorkMutex=new Mutex(); 

#ifdef GRAB_BY_POLLING
	createWorkerThreads(NUM_WORKER_THREADS); 
#endif
	mTotalFrames = 0;
} 

VideoManager::~VideoManager() 
{ 
	destroyWorkerThreads(); 

	ClipList::iterator ci; 
	for (ci=mClips.begin(); ci != mClips.end();ci++) 
		delete (*ci); 
	mClips.clear(); 
	delete mWorkMutex; 
} 

VideoClip* VideoManager::getVideoClipByName(std::string name) 
{ 
	foreach_l(VideoClip*,mClips) 
	if ((*it)->getName() == name) return *it; 

	return 0; 
} 

VideoClip* VideoManager::getVideoClipByTextureName(std::string name) 
{ 
	foreach_l(VideoClip*,mClips) 
	if ((*it)->getTextureName() == name) return *it; 

	return 0; 
} 

VideoClip* VideoManager::getVideoClipByMaterialName(std::string name) 
{ 
	foreach_l(VideoClip*,mClips) 
	if ((*it)->getMaterialName() == name) return *it; 

	return 0; 
} 

VideoClip* VideoManager::createVideoClip(
		std::string filename, 
		OutputMode output_mode, 
		int numPrecachedOverride, 
		bool usePower2Stride,
		int nHorzCropPercent, 
		int nVertCropPercent,
		int          nCapWidth, 
		int          nCapHeight
		) 
{ 
	int result = 0;
	mWorkMutex->lock(); 
	
	VideoClip* clip = NULL; 
	int nPrecached = numPrecachedOverride ? numPrecachedOverride : mDefaultNumPrecachedFrames; 
	clip = new VideoClip(&result, filename,output_mode,nPrecached,usePower2Stride, nHorzCropPercent, nVertCropPercent, nCapWidth, nCapHeight); 
	if(result == 0) {
		mClips.push_back(clip); 
	} else {
		delete clip;
		clip = NULL;
	}
	mWorkMutex->unlock(); 
	return clip;
} 

void VideoManager::destroyVideoClip(VideoClip* clip) 
{ 
	if (clip) 	{ 
		th_writelog("Destroying video clip: "+clip->getName()); 
		mWorkMutex->lock(); 
		bool reported=0; 
		while (clip->mAssignedWorkerThread) { 
			if (!reported) { th_writelog("Waiting for WorkerThread to finish decoding in order to destroy"); 
				reported=1; 
			} 
			_psleep(1); 
		} 
		if (reported) th_writelog("WorkerThread done, destroying.."); 
		foreach_l(VideoClip*,mClips) 
		if ((*it) == clip) 	{ 
			mClips.erase(it); 
			break; 
		} 
		delete clip; 
		th_writelog("Destroyed video."); 
		mWorkMutex->unlock(); 
	} 
} 

VideoClip* VideoManager::requestWork(WorkerThread* caller) 
{ 
	if (!mWorkMutex) return NULL; 
	mWorkMutex->lock(); 
	VideoClip* c=NULL; 

	float priority,last_priority=100000; 

	foreach_l(VideoClip*,mClips) 
	{ 
		if ((*it)->isBusy()) continue; 
		priority=(*it)->getPriorityIndex(); 
		if (priority < last_priority) { 
			last_priority=priority; 
			c=*it; 
		} 
	} 
	if (c) 
		c->mAssignedWorkerThread=caller; 

	mWorkMutex->unlock(); 
	return c; 
} 


int VideoManager::getNumWorkerThreads() 
{ 
	return mWorkerThreads.size(); 
} 

void VideoManager::createWorkerThreads(int n) 
{ 
	WorkerThread* t; 
	for (int i=0;i<n;i++) { 
		t=new WorkerThread(); 
		t->startThread(); 
		mWorkerThreads.push_back(t); 
	} 
} 

void VideoManager::destroyWorkerThreads() 
{ 
	foreach_l(WorkerThread*,mWorkerThreads) { 
		(*it)->waitforThread(); 
		delete (*it); 
	} 
	mWorkerThreads.clear(); 
} 

void VideoManager::setNumWorkerThreads(int n) 
{ 
	if (n == getNumWorkerThreads()) return; 
	th_writelog("changing number of worker threats to: "+str(n)); 

	destroyWorkerThreads(); 
	createWorkerThreads(n); 
} 

void VideoManager::getVersion(int* a,int* b,int* c) 
{ 
	*a=1; 
	*b=0; 
	*c=-2; 
} 
