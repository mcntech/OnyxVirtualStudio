#ifndef __OVSCameraMan_H__
#define __OVSCameraMan_H__

#include "Ogre.h"
#include <limits>
#include <SdkCameraMan.h>

class OvsCameraMan : public OgreBites::SdkCameraMan
{
public:
	OvsCameraMan(Ogre::Camera* cam, bool fCheckLinits):
	  SdkCameraMan(cam)
	{
		// Override base class default
		mFastMove = true;
		mfCheckLimits = fCheckLinits;
		mOrbitDirection = 0;
	}
	virtual ~OvsCameraMan(){};

	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
		if(mfCheckLimits)
			CheckLimits();
		// Yaw with keyboard
		if(mOrbiting) {
			mCamera->yaw(Ogre::Degree( 0.15f * mOrbitDirection));
		}
		if(this->mTarget) {
			if (mStyle == OgreBites::CS_FREELOOK) {
				// build our acceleration vector based on keyboard input composite
				Ogre::Vector3 accel = Ogre::Vector3::ZERO;
				if (mGoingForward) accel += mCamera->getDirection();
				if (mGoingBack) accel -= mCamera->getDirection();
				if (mGoingRight) accel += mCamera->getRight();
				if (mGoingLeft) accel -= mCamera->getRight();
				if (mGoingUp) accel += mCamera->getUp();
				if (mGoingDown) accel -= mCamera->getUp();

				// if accelerating, try to reach top speed in a certain time
				Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
				if (accel.squaredLength() != 0)	{
					accel.normalise();
					mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
				}
				// if not accelerating, try to stop in a certain time
				else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

				Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

				// keep camera velocity below top speed and above epsilon
				if (mVelocity.squaredLength() > topSpeed * topSpeed) {
					mVelocity.normalise();
					mVelocity *= topSpeed;
				}
				else if (mVelocity.squaredLength() < tooSmall * tooSmall)
					mVelocity = Ogre::Vector3::ZERO;

				if(mGoingUp || mGoingDown) {
					if (mVelocity != Ogre::Vector3::ZERO) {
						Ogre::Vector3 pos = mCamera->getPosition() + Ogre::Vector3(0,1,0)/*mVelocity * evt.timeSinceLastFrame*/;
						mCamera->setAutoTracking(true, mTarget,  pos);
					}
				} else {
					if (mVelocity != Ogre::Vector3::ZERO) 
						mCamera->move(mVelocity * evt.timeSinceLastFrame);
				}
			}
			return true;
		} else {
			return SdkCameraMan::frameRenderingQueued(evt);
		}
	}

		/*-----------------------------------------------------------------------------
		| Processes key presses for free-look style movement.
		-----------------------------------------------------------------------------*/
		virtual void injectKeyDown(const OIS::KeyEvent& evt)
		{
			if(mfCheckLimits)
				CheckLimits();
			if (mStyle == OgreBites::CS_FREELOOK)
			{
				if (evt.key == OIS::KC_UP) {
					mGoingForward = true;
				}
				else if (evt.key == OIS::KC_DOWN) {
					mGoingBack = true;
				}
				else if (evt.key == OIS::KC_LEFT) mGoingLeft = true;
				else if (evt.key == OIS::KC_RIGHT) mGoingRight = true;
				else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
				else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
				else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
				else if (evt.key == OIS::KC_E) {mOrbiting = true; mOrbitDirection = 1;} //mCamera->yaw(Ogre::Degree(-0.15f));
				else if (evt.key == OIS::KC_R) {mOrbiting = true; mOrbitDirection = -1;}//mCamera->yaw(Ogre::Degree( 0.15f));
			}
		}
		
		/*-----------------------------------------------------------------------------
		| Processes key releases for free-look style movement.
		-----------------------------------------------------------------------------*/
		virtual void injectKeyUp(const OIS::KeyEvent& evt)
		{
			if (mStyle == OgreBites::CS_FREELOOK)
			{
				if (evt.key == OIS::KC_UP) mGoingForward = false;
				else if (evt.key == OIS::KC_DOWN) mGoingBack = false;
				else if (evt.key == OIS::KC_LEFT) mGoingLeft = false;
				else if (evt.key == OIS::KC_RIGHT) mGoingRight = false;
				else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
				else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
				else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
				else if (evt.key == OIS::KC_E) { mOrbiting = false; mOrbitDirection = 0;} //mCamera->yaw(Ogre::Degree(-0.15f));}
				else if (evt.key == OIS::KC_R) { mOrbiting = false; mOrbitDirection = 0;}//mCamera->yaw(Ogre::Degree( 0.15f));
			}
		}

		void CheckLimits()
		{
			Ogre::Real left, top, right, bottom;
			mCamera->projectSphere(mProjectionSphere, &left, &top, &right, &bottom);
			if(left > -1.0 ){
				mGoingBack =false;
				mGoingLeft = false;
			}
			if(right < 1.0 ){
				mGoingBack =false;
				mGoingRight = false;
			}
			if(top < 1.0 ){
				mGoingBack =false;
				mGoingUp = false;
			}
			if(bottom > -1.0 ){
				mGoingBack =false;
				mGoingDown = false;
			}
		}
public:
	Ogre::Sphere  mProjectionSphere;
	bool          mfCheckLimits;
	int           mOrbitDirection;          
};
#endif // __OVSCameraMan_H__
