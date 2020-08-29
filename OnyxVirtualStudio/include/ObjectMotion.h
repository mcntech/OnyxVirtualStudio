#ifndef __ObjectMotion_H__
#define __ObjectMotion_H__
#include "BaseApplication.h"

class CObjectMotion
{
public:
	
	enum PROP_T {
		PROP_POS_X,
		PROP_POS_Y,
		PROP_POS_Z,
		PROP_SCALE,
		PROP_YAW
	};

	CObjectMotion():
	mRotating(false)
	, mScalingUp(false)
	, mScalingDn(false)
	, mTopSpeed(10)
	, mVelocity(Ogre::Vector3::ZERO)
	, mGoingForward(false)
	, mGoingBack(false)
	, mGoingLeft(false)
	, mYawClock(false)
	, mYawAntiClock(false)
	, mGoingRight(false)
	, mGoingUp(false)
	, mGoingDown(false)
	, mFastMove(false)
	{
		mTarget = NULL;
	}

	virtual ~CObjectMotion() {}

	virtual void setTarget(Ogre::SceneNode* target)
	{
		if (target != mTarget) {
			mTarget = target;
		}
	}

	virtual Ogre::SceneNode* getTarget()
	{
		return mTarget;
	}

	virtual void setTopSpeed(Ogre::Real topSpeed)
	{
		mTopSpeed = topSpeed;
	}

	virtual Ogre::Real getTopSpeed()
	{
		return mTopSpeed;
	}

	/*-----------------------------------------------------------------------------
	| Manually stops the camera when in free-look mode.
	-----------------------------------------------------------------------------*/
	virtual void manualStop()
	{
		mGoingForward = false;
		mGoingBack = false;
		mGoingLeft = false;
		mYawClock = false;
		mYawAntiClock = false;

		mGoingRight = false;
		mGoingUp = false;
		mGoingDown = false;
		mVelocity = Ogre::Vector3::ZERO;
	}

	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
		// build our acceleration vector based on keyboard input composite
		if(!mTarget)
			return true;

		if(mScalingUp || mScalingDn) {
			Ogre::Vector3 Scale = mTarget->getScale();
			if (mScalingUp) Scale = 1.01 * Scale;
			if (mScalingDn) Scale = 0.99 * Scale;
			mTarget->setScale(Scale);

		} else if (mYawClock || mYawAntiClock){
			Ogre::Quaternion Orientation = mTarget->getOrientation();
			Ogre::Radian yaw;// = Orientation.getYaw();
			if(mYawClock)
				yaw =  Ogre::Radian(Ogre::Math::PI / 180);
			if(mYawAntiClock)
				yaw =  -Ogre::Radian(Ogre::Math::PI / 180);

			mTarget->yaw(yaw);
		} else {
			Ogre::Quaternion Orientation = mTarget->getOrientation();
			Ogre::Vector3 accel = Ogre::Vector3::ZERO;

			if (mGoingForward) 
				accel += Orientation * -Ogre::Vector3::UNIT_Z;
			if (mGoingBack) 
				accel -= Orientation * -Ogre::Vector3::UNIT_Z;

			if (mGoingRight) 
				accel += Orientation * Ogre::Vector3::UNIT_X;
			if (mGoingLeft) 
				accel -= Orientation * Ogre::Vector3::UNIT_X;
			if (mGoingUp) 
				accel += Orientation * Ogre::Vector3::UNIT_Y;
			if (mGoingDown) 
				accel -= Orientation * Ogre::Vector3::UNIT_Y;

			// if accelerating, try to reach top speed in a certain time
			Ogre::Real topSpeed = mFastMove ? mTopSpeed * 5 : mTopSpeed;
			if (accel.squaredLength() != 0)	{
				accel.normalise();
				mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 2;
			}
			// if not accelerating, try to stop in a certain time
			else mVelocity -= mVelocity * evt.timeSinceLastFrame * 2;

			Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

			// keep camera velocity below top speed and above epsilon
			if (mVelocity.squaredLength() > topSpeed * topSpeed) {
				mVelocity.normalise();
				mVelocity *= topSpeed;
			}
			else if (mVelocity.squaredLength() < tooSmall * tooSmall)
				mVelocity = Ogre::Vector3::ZERO;

			if (mVelocity != Ogre::Vector3::ZERO) {
				Ogre::Vector3 Position = mTarget->getPosition();
				mTarget->setPosition(Position + mVelocity * evt.timeSinceLastFrame);
			}
		}
		return true;
	}

	virtual bool SetProp(int nPropId, Ogre::Real Val)
	{
		// build our acceleration vector based on keyboard input composite
		if(!mTarget)
			return true;
		switch(nPropId)
		{
			case PROP_POS_X:
				{
					Ogre::Vector3 Position = mTarget->getPosition();
					Position.x = Val;
					mTarget->setPosition(Position);
				}
				break;
			case PROP_POS_Y:
				{
				Ogre::Vector3 Position = mTarget->getPosition();
				Position.y = Val;
				mTarget->setPosition(Position);
				}
				break;
			case PROP_POS_Z:
				{
				Ogre::Vector3 Position = mTarget->getPosition();
				Position.z = Val;
				mTarget->setPosition(Position);
				}
				break;

			case PROP_YAW:
				{
					Ogre::Radian pitch, yaw, roll;
					Ogre::Quaternion quat =  mTarget->getOrientation();
					Ogre::Matrix3 mat;
					quat.ToRotationMatrix(mat);

					mat.ToEulerAnglesYXZ(yaw, pitch, roll);
					yaw = Ogre::Radian(Val);

					mat.FromEulerAnglesYXZ(yaw, pitch, roll);
					quat.FromRotationMatrix(mat);
					mTarget->setOrientation(quat);
				}
				break;
			case PROP_SCALE:
				{
				Ogre::Vector3 Scale = Ogre::Vector3(Val, Val, Val);
				mTarget->setScale(Scale);
				}
				break;
		}
		return true;
	}

	virtual bool SetLocan(Ogre::Vector3 Position, Ogre::Quaternion Orient)
	{
		// build our acceleration vector based on keyboard input composite
		if(!mTarget)
			return true;
		mTarget->setPosition(Position);
		mTarget->setOrientation(Orient);
		return true;
	}

	virtual void injectKeyDown(const OIS::KeyEvent& evt)
	{
		if (evt.key == OIS::KC_M ) mGoingForward = true;
		else if (evt.key == OIS::KC_U ) mGoingBack = true;
		else if (evt.key == OIS::KC_H ) 
			mGoingLeft = true;
		else if (evt.key == OIS::KC_K ) 
			mGoingRight = true;
		else if (evt.key == OIS::KC_I) mGoingUp = true;
		else if (evt.key == OIS::KC_N) mGoingDown = true;
		else if (evt.key == OIS::KC_Y) mScalingUp = true;
		else if (evt.key == OIS::KC_B) mScalingDn = true;
		else if (evt.key == OIS::KC_T) mYawClock = true;
		else if (evt.key == OIS::KC_G) mYawAntiClock = true;

		else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
	}

	virtual void injectKeyUp(const OIS::KeyEvent& evt)
	{

		if (evt.key == OIS::KC_M) mGoingForward = false;
		else if (evt.key == OIS::KC_U) mGoingBack = false;
		else if (evt.key == OIS::KC_H) mGoingLeft = false;
		else if (evt.key == OIS::KC_K) mGoingRight = false;
		else if (evt.key == OIS::KC_I) mGoingUp = false;
		else if (evt.key == OIS::KC_N) mGoingDown = false;
		else if (evt.key == OIS::KC_Y) mScalingUp = false;
		else if (evt.key == OIS::KC_B) mScalingDn = false;
		else if (evt.key == OIS::KC_T) mYawClock = false;
		else if (evt.key == OIS::KC_G) mYawAntiClock = false;

		else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
	}

    protected:
		Ogre::SceneNode* mTarget;

		bool mRotating;
		bool mScalingUp;
		bool mScalingDn;
		Ogre::Real mTopSpeed;
		Ogre::Vector3 mVelocity;

		bool mYawClock;
		bool mYawAntiClock;

		bool mGoingForward;
		bool mGoingBack;
		bool mGoingLeft;
		bool mGoingRight;
		bool mGoingUp;
		bool mGoingDown;
		bool mFastMove;
};

#endif // __ObjectMotion_H__
