#ifndef __HexagonalLayout_h_
#define __HexagonalLayout_h_

#include "BaseLayout.h"

class CHexagonalLayout : public CBaseLayout
{
public:
	CHexagonalLayout();
	~CHexagonalLayout(){}

	virtual void createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h);
	virtual void createPlaneMeshZX(Ogre::Real w, Ogre::Real h);
	virtual void createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h);

	virtual void CropCorner(std::string entName, int nVertNum,  float du, float dv)
	{
	}
	virtual void UpdatePlaneSize(
		std::string entName,
		Ogre::Real w, Ogre::Real h, 
		Ogre::Real vidLeft, Ogre::Real vidTop, Ogre::Real vidRight,  Ogre::Real vidBottom){}

	void createPlaneZX(
			Ogre::SceneManager* mSceneMgr,
			Ogre::String     name, 
			Ogre::Vector3    pos,
			Ogre::Quaternion orient
			);
	void createBackPlaneXY(
		Ogre::SceneManager* mSceneMgr,
		Ogre::String     name, 
		Ogre::Vector3    pos,
		Ogre::Quaternion orient
		);

protected:
	Ogre::Real      mCylRadius;
	Ogre::Real		mCylHeight;
	Ogre::Real      mVidplaneWidth;
	Ogre::Real		mVidplaneHeight;

	int             mSegments;
	Ogre::SceneManager* mSceneMgr;
	std::map<std::string,int> mStates;
};

#endif // #ifndef __HexagonalLayout_h_
