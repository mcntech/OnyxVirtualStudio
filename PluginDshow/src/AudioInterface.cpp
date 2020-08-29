#include "AudioInterface.h"

AudioInterface::AudioInterface(VideoClip* owner,int nChannels,int freq)
{
	mFreq=freq;
	mNumChannels=nChannels;
	mClip=owner;
}

AudioInterface::~AudioInterface()
{

}