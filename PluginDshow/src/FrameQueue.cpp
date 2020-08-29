#include "FrameQueue.h"
#include "VideoFrame.h"
#include "Util.h"


FrameQueue::FrameQueue(int n,VideoClip* parent)
{
	mParent=parent;
	setSize(n);
}

FrameQueue::~FrameQueue()
{
	foreach_l(VideoFrame*,mQueue)
		delete (*it);
	mQueue.clear();
}

void FrameQueue::setSize(int n)
{
	mMutex.lock();
	if (mQueue.size() > 0)
	{
		foreach_l(VideoFrame*,mQueue)
			delete (*it);
		mQueue.clear();
	}
	for (int i=0;i<n;i++)
		mQueue.push_back(new VideoFrame(mParent));

	mMutex.unlock();
}

int FrameQueue::getSize()
{
	return mQueue.size();
}

VideoFrame* FrameQueue::getFirstAvailableFrame()
{
	VideoFrame* frame=0;
	mMutex.lock();
	if (mQueue.front()->mReady) frame=mQueue.front();
	mMutex.unlock();
	return frame;
}

void FrameQueue::clear()
{
	mMutex.lock();
	foreach_l(VideoFrame*,mQueue)
		(*it)->clear();
	mMutex.unlock();
}

void FrameQueue::pop()
{
	mMutex.lock();
	VideoFrame* first=mQueue.front();
	first->clear();
	mQueue.pop_front();
	mQueue.push_back(first);
	mMutex.unlock();
}

VideoFrame* FrameQueue::requestEmptyFrame()
{
	VideoFrame* frame=0;
	mMutex.lock();
	foreach_l(VideoFrame*,mQueue){
		if (!(*it)->mInUse)	{
			(*it)->mInUse=true;
			(*it)->mReady=false;
			frame=(*it);
			break;
		}
	}
	mMutex.unlock();
	return frame;
}

int FrameQueue::getUsedCount()
{
	mMutex.lock();
	int n=0;
	foreach_l(VideoFrame*,mQueue)
		if ((*it)->mInUse) n++;
	mMutex.unlock();
	return n;
}

int FrameQueue::getReadyCount()
{
	mMutex.lock();
	int n=0;
	foreach_l(VideoFrame*,mQueue)
		if ((*it)->mReady) n++;
	mMutex.unlock();
	return n;
}
void FrameQueue::lock()
{
	mMutex.lock();
}

void FrameQueue::unlock()
{
	mMutex.unlock();
}