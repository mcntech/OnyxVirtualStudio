#ifndef _AudioInterface_h
#define _AudioInterface_h

#include "Export.h"

class VideoClip;


/**
    This is the class that serves as an interface between the library's audio
    output and the audio playback library of your choice.
    The class gets mono or stereo PCM data in in floating point data
 */
class PluginExport AudioInterface
{
public:
	//! PCM frequency, usualy 44100 Hz
	int mFreq;
	//! Mono or stereo
	int mNumChannels;
	//! Pointer to the parent VideoClip object
	VideoClip* mClip;

	AudioInterface(VideoClip* owner,int nChannels,int freq);
	virtual ~AudioInterface();

    //! A function that the VideoClip object calls once more audio packets are decoded
    /*!
      \param data contains one or two channels of float PCM data in the range [-1,1]
      \param nSamples contains the number of samples that the data parameter contains in each channel
    */
	virtual void insertData(float** data,int nSamples)=0;

	virtual void destroy() = 0;

};

class PluginExport AudioInterfaceFactory
{
public:
	//! VideoManager calls this when creating a new VideoClip object
	virtual AudioInterface* createInstance(VideoClip* owner,int nChannels,int freq)=0;
};


#endif

