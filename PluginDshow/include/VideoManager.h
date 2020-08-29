#ifndef _VideoManager_h
#define _VideoManager_h

#define NOMINMAX // required to stop windows.h messing up std::min

#include <OgreExternalTextureSource.h>
#include <OgreFrameListener.h>
#include <OgreSingleton.h>
#include "Export.h"
#include "Async.h"
#include "VideoClip.h"
#include <list>


// forward class declarations
class WorkerThread;
class VideoClip;
class AudioInterfaceFactory;
/**
	This is the main class that interfaces with Ogre, parses material files
	and distributes decoding jobs among threads.
*/
class PluginExport VideoManager 
{
public:
	friend class WorkerThread;

	typedef std::list<WorkerThread*> ThreadList;
	ThreadList mWorkerThreads;
	typedef std::list<VideoClip*> ClipList;
	ClipList mClips;

	int mDefaultNumPrecachedFrames;
	bool mbInit;

	Mutex* mWorkMutex;

	void createWorkerThreads(int n);
	void destroyWorkerThreads();

	/**
	 * Called by WorkerThread to request a VideoClip instance to work on decoding
	 */
	VideoClip* requestWork(WorkerThread* caller);
public:
	VideoManager();
	~VideoManager();

	static VideoManager& getSingleton(void);
	static VideoManager* getSingletonPtr(void);

	void logMessage(std::string msg);

	VideoClip* getVideoClipByName(Ogre::String name);
	VideoClip* getVideoClipByTextureName(Ogre::String name);
	VideoClip* getVideoClipByMaterialName(Ogre::String name);
	void destroyVideoClip(VideoClip* clip);

	int getNumWorkerThreads();
	void setNumWorkerThreads(int n);

	void setDefaultNumPrecachedFrames(int n);
	int getDefaultNumPrecachedFrames();

	VideoClip* createVideoClip(
		std::string filename,
		OutputMode output_mode,
		int numPrecachedOverride,
		bool usePower2Stride, 
		int nHorzCropPercent, 
		int nVertCropPercent,
		int          nCapWidth, 
		int          nCapHeight
		);

	void getVersion(int* a,int* b,int* c);

	unsigned long mTotalFrames;

};
#endif //_VideoManager_h

