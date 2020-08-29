#ifndef _VideoClip_h
#define _VideoClip_h

#include <string>
#include "Export.h"
#include "DirectShowUtil.h"

// forward class declarations
class InfoStruct;
class Mutex;
class FrameQueue;
class Timer;
class AudioInterface;
class WorkerThread;
class DataSource;
class VideoFrame;

/**
    format of the VideoFrame pixels. Affects decoding time
 */
enum OutputMode
{
	// A= full alpha (255), order of letters represents the byte order for a pixel
	TH_RGB=1,
	TH_RGBA=2,
	TH_ARGB=3,
	TH_BGR=4,
	TH_BGRA=5,
	TH_ABGR=6,
	TH_GREY=7,
	TH_GREY3=8, // RGB but all three components are luma
	TH_GREY3A=9,
	TH_AGREY3=10,
	TH_YUV=11,
	TH_YUVA=12,
	TH_AYUV=13
};

/**
	This object contains all data related to video playback, eg. the open source file,
	the frame queue etc.
*/
class PluginExport VideoClip : public CCompositorCB
{
	friend class WorkerThread;
	friend class VideoFrame;
	friend class VideoManager;

	FrameQueue* mFrameQueue;
	AudioInterface* mAudioInterface;

	Timer *mTimer,*mDefaultTimer;

	WorkerThread* mAssignedWorkerThread;

	// benchmark vars
	int mNumDroppedFrames,mNumDisplayedFrames;

	int mStreams, mVorbisStreams;	// Keeps track of  and Vorbis Streams

	int mNumPrecachedFrames;
	int mAudioSkipSeekFlag;

	float mSeekPos; //! stores desired seek position. next worker thread will do the seeking and reset this var to -1
	float mDuration;
    std::string mName;
	int mWidth,mHeight,mStride;
	unsigned long mNumFrames;

	float mAudioGain; //! multiplier for audio samples. between 0 and 1
	OutputMode mOutputMode,mRequestedOutputMode;
	bool mAutoRestart;
	bool mEndOfFile,mRestarted;
	int mIteration,mLastIteration; //! used to detect when the video restarted
	int mCountNoFrame;
	float mUserPriority;

	InfoStruct* mInfo; // a pointer is used to avoid having to include  & vorbis headers

	Mutex* mAudioMutex; //! syncs audio decoding and extraction
	
	CDsPlayer *mDsPlayer;

	bool isBusy();
	
	int load(std::string filename, int nCapWidth = 0, int nCapHeight = 0);
	void UpdateCropSettings();

	int NextFrameCB(unsigned char *pData, long lSize, double Pts);

public:
	VideoClip(
		int *pResult,
		std::string filename,
		OutputMode output_mode,
		int nPrecachedFrames,
		bool usePower2Stride,
		int nHorzCropPercent, 
		int nVertCropPercent,
		int          nCapWidth, 
		int          nCapHeight
		);
	~VideoClip();

	std::string getName();
	std::string getTextureName(){return mTextureName;}
	std::string getMaterialName(){return mTextureName;}
	//! benchmark function
	int getNumDisplayedFrames() { return mNumDisplayedFrames; }
	//! benchmark function
	int getNumDroppedFrames() { return mNumDroppedFrames; }

	//! return width in pixels of the video clip
	int getWidth() { return mWidth;}
	//! return height in pixels of the video clip
	int getHeight() { return mHeight; }
	//! return width in pixels of the video clip
	int getCropWidth() { return mWidth - mXStartMax; }
	//! return height in pixels of the video clip
	int getCropHeight() { return mHeight - mYStartMax; }

	/**
	    \brief return stride in pixels

		If you've specified usePower2Stride when creating the VideoClip object
		then this value will be the next power of two size compared to width,
		eg: w=376, stride=512.

		Otherwise, stride will be equal to width
	 */
	int getStride() { return mStride; }

	//! retur the timer objet associated with this object
	Timer* getTimer();
	//! replace the timer object with a new one
	void setTimer(Timer* timer);

	//! used by WorkerThread, do not call directly
	void decodeNextFrame();


	/**
	    \brief pop the frame from the front of the FrameQueue

		see FrameQueue::pop() for more details
	 */
	void popFrame();

	/**
	    \brief Returns the first available frame in the queue or NULL if no frames are available.

		see FrameQueue::getFirstAvailableFrame() for more details
	*/
	VideoFrame* getNextFrame();

	void setAudioInterface(AudioInterface* iface);
	AudioInterface* getAudioInterface();

	/**
	    \brief resize the frame queues

		Warning: this call discards ready frames in the frame queue
	 */
	void setNumPrecachedFrames(int n);
	//! returns the size of the frame queue
	int getNumPrecachedFrames();
	//! returns the number of ready frames in the frame queue
	int getNumReadyFrames();

	//! if you want to adjust the audio gain. range [0,1]
	void setAudioGain(float gain);
	float getAudioGain();

	//! if you want the video to automatically and smoothly restart when the last frame is reached
	void setAutoRestart(bool value);
	bool getAutoRestart() { return mAutoRestart; }



	/**
	    TODO: user priority. Useful only when more than one video is being decoded
	 */
	void setPriority(float priority);
	float getPriority();

	//! Used by VideoManager to schedule work
	float getPriorityIndex();

	//! get the current time index from the timer object
	float getTimePosition();
	//! get the duration of the movie in seconds
	float getDuration();
	//! return the clips' frame rate
	int getFPS();
	//! get the number of frames in this movie
	int getNumFrames() { return mNumFrames; }

	//! return the current output mode for this video object
	OutputMode getOutputMode();
	/**
	    set a new output mode

		Warning: this discards the frame queue. ready frames will be lost.
	 */
	void setOutputMode(OutputMode mode);

	void play();
	void pause();
	void restart();
	bool isPaused();
	void stop();
    void setPlaybackSpeed(float speed);
    float getPlaybackSpeed();

	void moveUp()
	{
		if (mYStart > 0) mYStart--;
	}
	void moveDn()
	{
		if (mYStart < mYStartMax) mYStart++;
	}

	void moveRight()
	{
		if (mXStart > 0) mXStart--;
	}
	void moveLeft()
	{
		if (mXStart < mXStartMax) mXStart++;
	}

	void setVerticalCrop(int nCrop)
	{
		mYCropPercent = nCrop;
	}
	void setHorizontalCrop(int nCrop)
	{
		mXCropPercent = nCrop;
	}

	//! seek to a given time position
	void seek(float time);
	void setColorKeyValue(unsigned long ulColorKeyEnable, unsigned long ulColorKeyVal, unsigned long ulColorKeyThres)
	{
		mColorKeyEnable = ulColorKeyEnable;
		mColorKeyRatio = ulColorKeyVal;
		mColorKeyThresh = ulColorKeyThres;
	}
	void setUpsideDown(bool fUpsideDown)
	{
		mDstUpsidedown = fUpsideDown;
	}
	void VideoClip::UpdateInput(std::string filename);

	Ogre::String        mMaterialName;
	Ogre::String        mTextureName;
	bool                mDstUpsidedown;
	bool                mSrcUpsidedown;
	unsigned long       mColorKeyEnable;
	unsigned long       mColorKeyRatio;
	unsigned long       mColorKeyThresh;
	int					mYStart;
	int					mXStart;
	int                 mYStartMax;
	int                 mXStartMax;
	int                 mXCropPercent;
	int                 mYCropPercent;
};

#endif
