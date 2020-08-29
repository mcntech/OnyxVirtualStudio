#include "Timer.h"

Timer::Timer()
{
	mTime=0;
	mPaused=0;
    mSpeed=1.0f;
}

Timer::~Timer()
{

}

void Timer::update(float time_increase)
{
	mTime+=time_increase*mSpeed;
}

float Timer::getTime()
{
	return mTime;
}

void Timer::pause()
{
	mPaused=true;
}

void Timer::play()
{
	mPaused=false;
}


bool Timer::isPaused()
{
	return mPaused;
}

void Timer::stop()
{

}

void Timer::seek(float time)
{
	mTime=time;
}

void Timer::setSpeed(float speed)
{
    mSpeed=speed;
}

float Timer::getSpeed()
{
    return mSpeed;
}
