#ifndef _Timer_h
#define _Timer_h

#include "Export.h"

/**
    This is a Timer object, it is used to control the playback of a VideoClip.

	You can inherit this class and make a timer that eg. plays twice as fast,
	or playbacks an audio track and uses it's time offset for syncronizing Video etc.
 */
class PluginExport Timer
{
protected:
	//! Current time in seconds
	float mTime,mSpeed;
	//! Is the timer paused or not
	bool mPaused;
public:
	Timer();
	virtual ~Timer();

	virtual float getTime();
	/**
	    \brief advance the time.

		If you're using another synronization system, eg. an audio track,
		then you can ignore this call or use it to perform other updates.

		NOTE: this is called by VideoManager from the main thread
	 */
	virtual void update(float time_increase);

	virtual void pause();
	virtual void play();
	virtual bool isPaused();
	virtual void stop();
	/**
	    \brief set's playback speed

        1.0 is the default. The speed factor multiplies time advance, thus
        setting the value higher will increase playback speed etc.
    
        NOTE: depending on Timer implementation, it may not support setting the speed
         
	 */
    virtual void setSpeed(float speed);
    //! return the update speed 1.0 is the default
    virtual float getSpeed();

	/**
	    \brief change the current time.

		if you're using another syncronization mechanism, make sure to adjust
		the time offset there
	 */
	virtual void seek(float time);
};
#endif

