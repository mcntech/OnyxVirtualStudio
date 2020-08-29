#include <memory.h>
//#include <ogg/ogg.h>
//#include <vorbis/vorbisfile.h>
//#include <theora/theoradec.h>
#include "VideoClip.h"
#include "VideoManager.h"
#include "VideoFrame.h"
#include "FrameQueue.h"
#include "AudioInterface.h"
#include "Timer.h"
//#include "DataSource.h"
#include "Util.h"
#include "Exception.h"

class InfoStruct
{
public:
	// ogg/vorbis/ variables
	//ogg_sync_state   OggSyncState;
	//ogg_page         OggPage;
	//ogg_stream_state VorbisStreamState;
	//ogg_stream_state StreamState;
	// State
	//th_info        Info;
	//th_comment     Comment;
	//th_setup_info* Setup;
	//th_dec_ctx*    Decoder;
	//Vorbis State
	//vorbis_info      VorbisInfo;
	//vorbis_dsp_state VorbisDSPState;
	//vorbis_block     VorbisBlock;
	//vorbis_comment   VorbisComment;

	InfoStruct()
	{
		//Decoder=0;
		//Setup=0;
	}
};

//! clears a portion of memory with an unsign
void memset_uint(void* buffer,unsigned int colour,unsigned int size_in_bytes)
{
	unsigned int* data=(unsigned int*) buffer;
	for (unsigned int i=0;i<size_in_bytes;i+=4)
	{
		*data=colour;
		data++;
	}
}

VideoClip::VideoClip(
	int          *pResult,
	std::string  filename,
	OutputMode   output_mode,
	int          nPrecachedFrames,
	bool         usePower2Stride,
	int          nHorzCropPercent, 
	int          nVertCropPercent,
	int          nCapWidth, 
	int          nCapHeight
  ):
	mAudioInterface(NULL),
	mNumDroppedFrames(0),
	mNumDisplayedFrames(0),
	mStreams(0),
	mVorbisStreams(0),
	mAudioSkipSeekFlag(0),
	mSeekPos(-1),
	mDuration(-1),
    mName(filename),
    mStride(usePower2Stride),
    mAudioGain(1),
    mOutputMode(output_mode),
    mRequestedOutputMode(output_mode),
    mAutoRestart(1),
    mEndOfFile(0),
    mRestarted(0),
	mIteration(0),
	mLastIteration(0),
	mCountNoFrame(0)
{
	mColorKeyEnable = false;
	mAudioMutex=new Mutex;
	mDsPlayer = NULL;
	mTimer=mDefaultTimer=new Timer();

	mFrameQueue=NULL;
	mAssignedWorkerThread=NULL;
	mNumPrecachedFrames=nPrecachedFrames;

	mInfo=new InfoStruct;
	mDstUpsidedown = false;
	mSrcUpsidedown = false;

	mYStart = 0;
	mYStartMax = 0;
	mXStart = 0;
	mXStartMax = 0;
	mYCropPercent = nVertCropPercent;
	mXCropPercent = nHorzCropPercent;
	*pResult = load(filename, nCapWidth, nCapHeight);
}

VideoClip::~VideoClip()
{
	// wait untill a worker thread is done decoding the frame
	while (mAssignedWorkerThread)
	{
		_psleep(1);
	}

	delete mDefaultTimer;

	if (mDsPlayer) 
		delete mDsPlayer;


	if (mFrameQueue) delete mFrameQueue;

	delete mInfo;

	if (mAudioInterface)
	{
		mAudioMutex->lock(); // ensure a thread isn't using this mutex

	// probably not necesarry because all it does is memset to 0
	//	ogg_stream_clear(&mInfo->VorbisStreamState);
	//	vorbis_block_clear(&mInfo->VorbisBlock);
	//	vorbis_dsp_clear(&mInfo->VorbisDSPState);
	//	vorbis_comment_clear(&mInfo->VorbisComment);
	//	vorbis_info_clear(&mInfo->VorbisInfo);
		mAudioInterface->destroy(); // notify audio interface it's time to call it a day
	}
	delete mAudioMutex;

	//ogg_sync_clear(&mInfo->OggSyncState);
}

Timer* VideoClip::getTimer()
{
	return mTimer;
}

void VideoClip::setTimer(Timer* timer)
{
	if (!timer) mTimer=mDefaultTimer;
	else mTimer=timer;
}

// TODO: Remove this. It is supeseded by callback mechanism
void VideoClip::decodeNextFrame()
{
	int fDone = 0;
	if (mEndOfFile) 
		return;

	VideoFrame* frame=mFrameQueue->requestEmptyFrame();
	if (!frame) return; // max number of precached frames reached

	//frame->mTimeToDisplay=time;
	//frame->mIteration=mIteration;
	//frame->_setFrameNumber(frame_number);
	img_plane f = frame->getBufferYuv();
	while(!fDone){
		// TODO : Verify the buffer size
		int nSize;
		if (mDsPlayer->getFrame((char *)f.data, &nSize) == 0) {
			frame->decode(&f);
			fDone = 1;
		} else {
			_psleep(1);
			continue;
		}
	}
}

int VideoClip::NextFrameCB(unsigned char *pData, long lSize, double Pts)
{

	VideoFrame* frame=mFrameQueue->requestEmptyFrame();
	if (!frame) 
		return -1; // max number of precached frames reached

	frame->mTimeToDisplay=Pts;
	//frame->mIteration=mIteration;
	//frame->_setFrameNumber(frame_number);
	img_plane f = frame->getBufferYuv();

	// TODO : Verify the buffer size
	memcpy((char *)f.data, pData, lSize);
	frame->decode(&f);
	return 0;
}

void VideoClip::restart()
{
	if(mDsPlayer){
		mDsPlayer->rewindMovie();
	}
}


void VideoClip::popFrame()
{
	mNumDisplayedFrames++;
	mFrameQueue->pop(); // after transfering frame data to the texture, free the frame
						// so it can be used again
}

VideoFrame* VideoClip::getNextFrame()
{
	VideoFrame* frame = 0;

	float time=mTimer->getTime();
	for (;;) {
		frame=mFrameQueue->getFirstAvailableFrame();

		if (!frame) { 
			mCountNoFrame++;
			if (mCountNoFrame > 30 && mAutoRestart){
				if(mDsPlayer->isEndOfPlay()){
					mDsPlayer->rewindMovie();
					mCountNoFrame = 0;
				}
			}
			return 0;
		}
#ifdef ENABLE_SYCNH
		if (frame->mTimeToDisplay > time) 
			return 0;
		if (frame->mTimeToDisplay < time-0.1) {
			if (mRestarted && frame->mTimeToDisplay < 2) 
				return 0;
			mNumDroppedFrames++;
			mNumDisplayedFrames++;
			mFrameQueue->pop();
		}
		else 
#endif 
			break;
	}
	mCountNoFrame = 0;
	mLastIteration=frame->mIteration;	// TODO remove
	return frame;
}
void VideoClip::UpdateCropSettings()
{
	if(mXCropPercent > 0) {
		mXStartMax = mWidth * mXCropPercent / 100;
		mXStart = mXStartMax / 2;
	}
	if(mYCropPercent > 0) {
		mYStartMax = mHeight * mYCropPercent / 100 ;
		mYStart = mYStartMax / 2;
	}
}

int VideoClip::load(std::string filename, int nCapWidth, int nCapHeight)
{
	mDsPlayer = new CDsPlayer(this);
	
	int res = mDsPlayer->loadMovie(filename, nCapWidth, nCapHeight);

	if(res == 0) {
		mWidth = mDsPlayer->getWidth();
		mHeight = mDsPlayer->getHeight();
		
		UpdateCropSettings();

		mSrcUpsidedown = mDsPlayer->getVerticalFlip();
		int nWidth = getCropWidth();
		mStride = (mStride == 1) ? mStride = _nextPow2(nWidth) : nWidth;
		mFrameQueue=new FrameQueue(mNumPrecachedFrames,this);
	}
	return res;
}


std::string VideoClip::getName()
{
	return mName;
}

bool VideoClip::isBusy()
{
	return mAssignedWorkerThread || mOutputMode != mRequestedOutputMode;
}

OutputMode VideoClip::getOutputMode()
{
	return mOutputMode;
}

void VideoClip::setOutputMode(OutputMode mode)
{
	if (mOutputMode == mode) return;
	mRequestedOutputMode=mode;
	while (mAssignedWorkerThread) _psleep(1);
	// discard current frames and recreate them
	mFrameQueue->setSize(mFrameQueue->getSize());
	mOutputMode=mRequestedOutputMode;
}

float VideoClip::getTimePosition()
{
	return mTimer->getTime();
}
int VideoClip::getNumPrecachedFrames()
{
	return mFrameQueue->getSize();
}

void VideoClip::setNumPrecachedFrames(int n)
{
	if (mFrameQueue->getSize() != n)
		mFrameQueue->setSize(n);
}

int VideoClip::getNumReadyFrames()
{
	return mFrameQueue->getReadyCount();
}

float VideoClip::getDuration()
{
	return mDuration;
}

int VideoClip::getFPS()
{
	//	return mInfo->Info.fps_numerator;
	return 0;
}

void VideoClip::play()
{
	mTimer->play();
	if(mDsPlayer)
		mDsPlayer->playMovie();
}

void VideoClip::pause()
{
	mTimer->pause();
	if(mDsPlayer)
		mDsPlayer->pauseMovie();
}

bool VideoClip::isPaused()
{
	return mTimer->isPaused();
}

void VideoClip::stop()
{
	pause();
	seek(0);
}

void VideoClip::UpdateInput(std::string filename)
{

}

void VideoClip::setPlaybackSpeed(float speed)
{
    mTimer->setSpeed(speed);
}

float VideoClip::getPlaybackSpeed()
{
    return mTimer->getSpeed();
}

void VideoClip::seek(float time)
{
#if 0
	mSeekPos=time;
	mEndOfFile=false;
#endif
}

float VideoClip::getPriority()
{
	// TODO
	return getNumPrecachedFrames()*10.0f;
}

float VideoClip::getPriorityIndex()
{
	float priority=(float) getNumReadyFrames();
	if (mTimer->isPaused()) 
		priority+=getNumPrecachedFrames()/2;
	return priority;
}

void VideoClip::setAudioInterface(AudioInterface* iface)
{
	mAudioInterface=iface;
}

AudioInterface* VideoClip::getAudioInterface()
{
	return mAudioInterface;
}

void VideoClip::setAudioGain(float gain)
{
	if (gain > 1) mAudioGain=1;
	if (gain < 0) mAudioGain=0;
	else          mAudioGain=gain;
}

float VideoClip::getAudioGain()
{
	return mAudioGain;
}

void VideoClip::setAutoRestart(bool value)
{
	mAutoRestart=value;
}
