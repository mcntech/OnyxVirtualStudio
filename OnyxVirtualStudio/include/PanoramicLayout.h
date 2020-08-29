#ifndef __CPanoramicLayout_h_
#define __CPanoramicLayout_h_

#define MESH_NUM_COL     3
#define MESH_NUM_ROW     3
#define MESH_NUM_CTRL    (MESH_NUM_COL * MESH_NUM_ROW)

#include "CylindricalLayout.h"
#include "OnyxControl.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(push, 1)
#endif
    typedef struct _UVCord_T
	{
        float u, v;
    } UVCord_T;
    
class CUVMap
{
public:
	CUVMap(){}
	CUVMap(	
		float    _w,
		float    _h,
		float    _d = 0.0,
		int      nUvAngle = 0,			// Rotation 0, 90, 180, 270
		float    _cropLeft  = 0.0,
		float    _cropTop   = 0.0,
		float    _cropRight = 0.0,
		float    _cropBottom= 0.0 )
	{
		mWidth = _w;
		mHeight = _h;
		mDepth = _d;
		mUvAngle = nUvAngle;
		mUStart = 0;
		mVStart = 0;
		mUEnd = 1.0;
		mVEnd = 1.0;
		mCropLeft = _cropLeft;
		mCropTop = _cropTop;
		mCropRight = _cropRight;
		mCropBottom = _cropBottom;
	};

	void GetMeshCtrlPts(PatchVertex *pVertsXY);
	void RotateUvCord(int nAngle=90);
	void MoveUvVert(int nVertNum,  float du, float dv);
	void MoveUvEdgeRight(float du);
	void MoveUvEdgeLeft(float du);
	void MoveUvEdgeUp(float dv);
	void MoveUvEdgeDn(float dv);

	float    mWidth;
	float    mHeight;
	float    mDepth;
	int      mUvAngle;			// Rotation 0, 90, 180, 270
	float    mUStart;
	float    mVStart;
	float    mUEnd;
	float    mVEnd;

	float    mCropLeft;
	float    mCropTop;
	float    mCropRight;
	float    mCropBottom;

public:
	void GenUvCord();
	UVCord_T mUuVerts[9];
};

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(pop)
#endif

class CPanoramicLayout : public CBaseLayout
{
public:
	CPanoramicLayout()
	{
		mfCurve = false;
	}
	virtual ~CPanoramicLayout(){}

	void createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h);
	void createPlaneMeshZX(Ogre::Real w, Ogre::Real h);
	void createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h);

	void CropCorner(std::string entName, int nVertNum,  float du, float dv);
	void SetCrop(std::string entName, float left, float top, float right, float bottom);
	void SetRotation(std::string entName, int nRot);
	void UpdateLayoutOption()
	{
		CBaseLayout::UpdateLayoutOption();
		switch(mCrntLayoutOption)
		{
			case LAYOUT_3D_CURVE:
				mfCurve = true;
				break;
			default:
				mfCurve = false;
				break;
		}
		UpdateCurvature(mfCurve);
	}
private:
	void UpdateCurvature(bool fCurve);
	void UpdatePlaneSize(std::string entName,
		Ogre::Real w, Ogre::Real h, 
		Ogre::Real vidLeft, Ogre::Real vidTop, Ogre::Real vidRight,  Ogre::Real vidBottom);

	void CreateBezierPatchXY(const char *szMeshName, Ogre::Real w, Ogre::Real h);
	void UpdateBezierPatchUV(Ogre::PatchMeshPtr pPatch,  CUVMap *pUVMap);
	float getCurvedPlaneDepth(const char *szMeshName);

	bool                   mfCurve;
	std::map<std::string, CUVMap> mUVMap;
};

#endif // #ifndef __CPanoramicLayout_h_
