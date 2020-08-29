#ifndef _VideoFrame_h
#define _VideoFrame_h

#include <Export.h>

typedef struct{
	int            width;
	int            height;
	int            stride;
	unsigned char  *data;
} img_plane;

class VideoClip;

class PluginExport VideoFrame
{
	VideoClip* mParent;
	img_plane  mBufferYuv;
	unsigned char* mBuffer;
	unsigned long mFrameNumber;

public:
	//! global time in seconds this frame should be displayed on
	float mTimeToDisplay;
	//! whether the frame is ready for display or not
	bool mReady;
	//! indicates the frame is being used by WorkerThread instance
	bool mInUse;
	//! used to detect when the video restarted to ensure smooth playback
	int mIteration;

	VideoFrame(VideoClip* parent);
	~VideoFrame();
	
	void CopyToHwTextureARGB(img_plane* yuv,unsigned char* out,int destStride);

	//! internal function, do not use directly
	void _setFrameNumber(int number) { mFrameNumber=number; }
	//! returns the frame number of this frame in the  stream
	int getFrameNumber() { return mFrameNumber; }

	void clear();
	int getWidth();
	int getStride();
	int getHeight();

	unsigned char* getBuffer();
	img_plane      getBufferYuv();
	int            mWidth;
	int            mHeight;
	int            mStride;
	//! Called by VideoClip to decode a YUV buffer onto itself
	void decode(void* yuv);
};
#endif
