#ifndef __BaselLayout_h_
#define __BaselLayout_h_

#include "OnyxVirtualStudio.h"
#define STREAM_MATERAIL_NAME_PREFIX		"display"
#define STREAM_ENTITY_NAME_PREFIX		"display"

#define ANCHOR_MATERAIL_NAME_PREFIX		"anchor"
#define ANCHOR_ENTITY_NAME_PREFIX		"anchor"

class CBaseLayout : public CLayout
{
public:
	CBaseLayout();
	~CBaseLayout(){}

	int InitLayout(Ogre::SceneManager* mSceneMgr, int nSegments, Ogre::Real  AspectRatio, bool fEnable3dSticth);

	virtual void createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h) = 0;
	virtual void createPlaneMeshZX(Ogre::Real w, Ogre::Real h) = 0;
	virtual void createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h) = 0;
	virtual void CropCorner(std::string entName, int nVertNum,  float du, float dv) = 0;
	virtual void SetCrop(std::string entName, float left, float top, float right, float bottom){}
	virtual void SetRotation(std::string entName, int nRot){}
	virtual void createPlaneZX(Ogre::SceneManager* mSceneMgr, Ogre::String name, Ogre::Vector3 pos,	Ogre::Quaternion orient);
	virtual void createBackPlaneXY(Ogre::SceneManager* mSceneMgr,Ogre::String name, Ogre::Vector3 pos, Ogre::Quaternion orient);

	virtual void getPlanePosForHexLayout(
			int              nPlane,
			Ogre::Vector3    &Pos, 
			Ogre::Quaternion &Orient
			);


	virtual virtual void UpdatePlaneSize(
		std::string entName,
		Ogre::Real w, Ogre::Real h, 
		Ogre::Real vidLeft, Ogre::Real vidTop, Ogre::Real vidRight,  Ogre::Real vidBottom)=0;

	void getAdvPanelPos(
		int              nAdvId,
		Ogre::Real    &Left, 
		Ogre::Real    &Top, 
		Ogre::Real     &Width, 
		Ogre::Real     &Height 
		);

	virtual void UpdateMaterial(int option_id);
	virtual void UpdateOption()
	{
		UpdateMaterial(mCrntOption);
	};
	virtual void UpdateLayoutOption();
		
	virtual void EnableStream(std::string &entity, bool fEnable);
	virtual void setMeshRotation(Ogre::String    &entName, Ogre::Real angle);
	virtual void setPitch(Ogre::String    &entName, Ogre::Real angle);

protected:
	void GenerateManualWallMeshes();
	void CreateVideoPlaneMeshInstance(void);
	void ShowGraphics(bool fShow);
	bool Is2DLayout();

protected:
	Ogre::Real      mCylRadius;
	Ogre::Real		mCylHeight;
	Ogre::Real      mVidplaneWidth;
	Ogre::Real		mVidplaneHeight;
	Ogre::Real		mAspectRatio;
	bool            mfEnable3dSticth;
	int             mSegments;
	Ogre::SceneManager* mSceneMgr;
	std::map<std::string,int> mStates;
};

#endif // #ifndef __BaselLayout_h_
