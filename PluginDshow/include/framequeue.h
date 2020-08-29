
#ifndef _FrameQueue_h
#define _FrameQueue_h

#include "Async.h"
#include <list>

class VideoFrame;
class VideoClip;

/**
	This class handles the frame queue. contains frames and handles their alloctation/deallocation
	it is designed to be thread-safe
*/
class FrameQueue
{
	std::list<VideoFrame*> mQueue;
	VideoClip* mParent;
	Mutex mMutex;
public:
	FrameQueue(int n,VideoClip* parent);
	~FrameQueue();

	/**
	    \brief Returns the first available frame in the queue or NULL if no frames are available.

		This function DOES NOT remove the frame from the queue, you have to do it manually
		when you want to mark the frame as used by calling the pop() function.
	*/
	VideoFrame* getFirstAvailableFrame();

	//! return the number of used (not ready) frames
	int getUsedCount();

	//! return the number of ready frames
	int getReadyCount();

	/**
	    \brief remove the first available frame from the queue.

	    Use this every time you display a frame	so you can get the next one when the time comes.
		This function marks the frame on the front of the queue as unused and it's memory then
		get's used again in the decoding process.
		If you don't call this, the frame queue will fill up with precached frames up to the
		specified amount in the VideoManager class and you won't be able to advance the video.
	*/
	void pop();
	//! frees all decoded frames for reuse (does not destroy memory, just marks them as free)
	void clear();
	//! Called by WorkerThreads when they need to unload frame data, do not call directly!
	VideoFrame* requestEmptyFrame();

	/** 
	    \brief set's the size of the frame queue.
		
		Beware, currently stored ready frames will be lost upon this call
	*/
	void setSize(int n);
	//! return the size of the queue
	int getSize();

	//! lock the queue's mutex manually
	void lock();
	//! unlock the queue's mutex manually
	void unlock();
};

#endif
