#include "BaseLayout.h"
#include "OnyxControl.h"

const int modelWidth  = 360;
const int modelHeight = 140;


void getWallPosForHexLayout(
		int              nPlane,
		Ogre::Real       CylRadius,
		Ogre::Real       Offset,
		Ogre::Vector3    &Pos, 
		Ogre::Quaternion &Orient,
		Ogre::Real       originAngle = 240              // From Z Axis on ZX plane
		)
{
	Ogre::Real Radius = CylRadius + Offset;
	Ogre::Degree angleCentral = Ogre::Degree(360.0 / 6);
	Ogre::Real planeDist = Radius * Ogre::Math::Cos(angleCentral.valueRadians() / 2);
	
	// Rotate with reference form Z - Axis (outwards to screen)
	Ogre::Degree planeNormal = Ogre::Degree(originAngle/*90.0*/) - nPlane * angleCentral;
	Pos = Ogre::Vector3(planeDist *  Ogre::Math::Sin(planeNormal.valueRadians()), 0, planeDist *  Ogre::Math::Cos(planeNormal.valueRadians()));
	
	Ogre::Vector3 Origin(0, 0, 0);
	Ogre::Vector3 Direction = Origin - Pos;			// new direction of the plane normal
	Ogre::Vector3 src = Ogre::Vector3::UNIT_Z;		// Current direction of the plane normal

	Orient = src.getRotationTo(Direction);
}

CBaseLayout::CBaseLayout(	)
{

}

int CBaseLayout::InitLayout(
		Ogre::SceneManager* _SceneMgr,
		int         nSegemnts,
		Ogre::Real  AspectRatio,
		bool        fEnable3dSticth
		)
{
	mCylRadius = modelWidth / 2;
	mCylHeight = modelHeight;
	mAspectRatio = AspectRatio;
	mfEnable3dSticth = fEnable3dSticth;


	mVidplaneWidth = mCylRadius - (mfEnable3dSticth ? 0 : 20);
	mVidplaneHeight = mVidplaneWidth / mAspectRatio;

	mSegments = nSegemnts;
	mSceneMgr = _SceneMgr;

	GenerateManualWallMeshes();
	CreateVideoPlaneMeshInstance();
	
	Ogre::Real sphereRadius = mCylRadius > mCylHeight ? mCylRadius : mCylHeight;
	mProjectionSphere.setRadius(sphereRadius * 3);

	mMaxLayoutOptions = LAYOUT_OPTION_LAST;
	mCrntLayoutOption = LAYOUT_3D_PLANE;
	return 0;
}

void CBaseLayout::ShowGraphics(bool fShow)
{
    Ogre::Entity*       ent;
    Ogre::SceneNode*    node;
	char ent_name[128];
	for (int i=0; i < mSegments; i++) {
		sprintf (ent_name,"wall%d",i);
		ent = mSceneMgr->getEntity(ent_name);
		ent->setVisible(fShow);
	}
	sprintf (ent_name,"roof");
	ent = mSceneMgr->getEntity(ent_name);
	ent->setVisible(fShow);

	sprintf (ent_name,"floor");
	ent = mSceneMgr->getEntity(ent_name);
	ent->setVisible(fShow);

}

void CBaseLayout::getPlanePosForHexLayout(
		int              nPlane,
		Ogre::Vector3    &Pos, 
		Ogre::Quaternion &Orient
		)
{
	getWallPosForHexLayout(nPlane, mCylRadius, 0.0, Pos, Orient);
}

void CBaseLayout::getAdvPanelPos(
		int           nAdvId,
		Ogre::Real    &Left, 
		Ogre::Real    &Top, 
		Ogre::Real     &Width, 
		Ogre::Real     &Height 
		)
{
	nAdvId++; // Base 1
	Top = 1.0 - nAdvId * 0.1;
	Left = 0.0;
	Width = 1.0;
	Height = 0.1;
}

/*
 * Normalized plane
 */
class CPlane
{
 public:
	CPlane()
	{
		mUStart = mVStart = 0.0;
		mUEnd = mVEnd = 1.0;
		mWidth = mHeight = 1.0;
	}
	Ogre::Vector3     mPos;
	Ogre::Quaternion  mOrient;
	Ogre::Real        mWidth;
	Ogre::Real        mHeight;

	Ogre::Real        mUStart;
	Ogre::Real        mUEnd;
	Ogre::Real        mVStart;
	Ogre::Real        mVEnd;

	bool              mfUsed;
};

class CPlaneLayout
{
public:
	CPlaneLayout()
	{
		for (int i=0; i < 6; i++) {
			mPlanes[i].mfUsed = false;
			mPlanes[i].mOrient = Ogre::Quaternion::IDENTITY;
		}
		for (int i=0; i < 2; i++) {
			mAnchors[i].mfUsed = false;
			mAnchors[i].mOrient = Ogre::Quaternion::IDENTITY;
		}
	}
	virtual void getPlanePos(int nPlane, Ogre::Vector3 &pos, Ogre::Quaternion &orient)
	{
		pos =  mPlanes[nPlane].mPos;
		orient = mPlanes[nPlane].mOrient;

	}
	virtual void getAnchorPos(int nPlane, Ogre::Vector3 &pos, Ogre::Quaternion &orient)
	{
		pos =  mAnchors[nPlane].mPos;
		orient = mAnchors[nPlane].mOrient;
	}

	CPlane            mPlanes[6];
	CPlane            mAnchors[2];
	//std::set<int>  mPlanes;
	//std::set<int>  mAnchors;
};

class CFlatLayoutDDxDA : public CPlaneLayout
{
public:
	CFlatLayoutDDxDA()
	{
		mPlanes[0].mPos    = Ogre::Vector3(-0.5, 0.5, -1.0);
		mPlanes[0].mfUsed = true;
		mPlanes[1].mPos = Ogre::Vector3( 0.5, 0.5, -1.0);
		mPlanes[1].mfUsed = true;
		mPlanes[2].mPos = Ogre::Vector3( 0.5,-0.5, -1.0);
		mPlanes[2].mfUsed = true;
		mAnchors[0].mPos = Ogre::Vector3(-0.5,-0.5, -1.0);
		mAnchors[0].mfUsed = true;
	}
};

class CFlatLayoutDDxD : public CPlaneLayout
{
public:
	CFlatLayoutDDxD()
	{
		mPlanes[0].mPos = Ogre::Vector3(-0.5, 0.5, -1.0);
		mPlanes[0].mfUsed = true;

		mPlanes[1].mPos = Ogre::Vector3( 0.5, 0.5, -1.0);
		mPlanes[1].mfUsed = true;

		mPlanes[2].mPos = Ogre::Vector3( 0.0,-0.5, -1.0);
		mPlanes[2].mfUsed = true;
	}
};

class CFlatLayoutDDxA : public CPlaneLayout
{
public:
	CFlatLayoutDDxA()
	{
		Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;

		mPlanes[0].mPos = Ogre::Vector3(-0.5, 0.5, -1.0);
		mPlanes[0].mfUsed = true;

		mPlanes[1].mPos = Ogre::Vector3( 0.5, 0.5, -1.0);
		mPlanes[1].mfUsed = true;

		mAnchors[0].mPos = Ogre::Vector3(0.0,-0.5, -1.0);
		mAnchors[0].mfUsed = true;
	}
};

class CFlatLayoutDD : public CPlaneLayout
{
public:
	CFlatLayoutDD()
	{
		Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;
		mPlanes[0].mPos = Ogre::Vector3(-0.5, 0.0, -1.0);
		mPlanes[0].mfUsed = true;
		mPlanes[1].mPos = Ogre::Vector3( 0.5, 0.0, -1.0);
		mPlanes[1].mfUsed = true;
	}
};

class CFlatLayoutDA : public CPlaneLayout
{
public:
	CFlatLayoutDA(int nStrmId)
	{
		mPlanes[nStrmId].mPos = Ogre::Vector3(-0.5, 0.0, -1.0);
		mPlanes[nStrmId].mfUsed = true;
		mAnchors[0].mPos = Ogre::Vector3(0.5, 0.0, -1.0);
		mAnchors[0].mfUsed = true;
	}
};


class CFlatLayoutD : public CPlaneLayout
{
public:
	CFlatLayoutD(int StreamId)
	{
		mPlanes[StreamId].mPos = Ogre::Vector3(0.0, 0.0, -1.0);
		mPlanes[StreamId].mfUsed = true;
	}
};

class CFlatLayoutA : public CPlaneLayout
{
public:
	CFlatLayoutA(int StreamId)
	{
		mAnchors[StreamId].mPos = Ogre::Vector3(0.0, 0.0, -1.0);
		mAnchors[StreamId].mfUsed = true;
	}
};

class CFlatLayoutDDDClip : public CPlaneLayout
{
public:
	void SetPlaneDim(int n)
	{
		Ogre::Real w = 1.0 / 3;
		Ogre::Real h = (3 * w) / (2.39 / mAspectRatio);

		mPlanes[n].mWidth = w;
		mPlanes[n].mHeight = h;

		mPlanes[n].mUStart = (1.0 - w) / 2;
		mPlanes[n].mVStart = (1.0 - h) / 2;
		mPlanes[n].mUEnd = mPlanes[n].mUStart + w;
		mPlanes[n].mVEnd = mPlanes[n].mVStart + h;
	}
	CFlatLayoutDDDClip(Ogre::Real AspectRatio)
	{
		Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;
		mAspectRatio = AspectRatio;
		SetPlaneDim(0);
		mPlanes[0].mPos = Ogre::Vector3(-1.0, 0.0, -1.0);
		mPlanes[0].mfUsed = true;

		SetPlaneDim(1);
		mPlanes[1].mPos = Ogre::Vector3( 0.0, 0.0, -1.0);
		mPlanes[1].mfUsed = true;

		SetPlaneDim(2);
		mPlanes[2].mPos = Ogre::Vector3( 1.0,0.0, -1.0);
		mPlanes[2].mfUsed = true;
	}

	Ogre::Real mAspectRatio;
};

class CPlaneLayoutHex : public CPlaneLayout
{
public:
	CPlaneLayoutHex(Ogre::Real CylRadius, Ogre::Real Height)
	{
		Ogre::Vector3     pos;
		Ogre::Quaternion  orient(Ogre::Quaternion::IDENTITY);

		for (int i=0; i < 6; i++) {
			getWallPosForHexLayout(i, CylRadius, 0.0, pos, orient);
			mPlanes[i].mPos = pos;
			mPlanes[i].mOrient = orient;
			mPlanes[i].mfUsed = true;
		}
		for (int i=0; i < 2; i++) {
			mAnchors[i].mPos = Ogre::Vector3((MAX_ANCHORS - i) * (CylRadius / 4), - Height / 3, 0);
			mAnchors[i].mPos = pos;
			mAnchors[i].mOrient = orient;
			mAnchors[i].mfUsed = true;

		}
	}
};


void CBaseLayout::createPlaneZX(
		Ogre::SceneManager* mSceneMgr,
		Ogre::String     name, 
		Ogre::Vector3    pos,
		Ogre::Quaternion orient
		)
{
    Ogre::Entity*       ent;
    Ogre::SceneNode*    node;

	ent = mSceneMgr->createEntity(name, PLANE_MESH_ZX);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(pos);
	node->setOrientation(orient);
}

void CBaseLayout::createBackPlaneXY(
	Ogre::SceneManager* mSceneMgr,
	Ogre::String     name, 
	Ogre::Vector3    pos,
	Ogre::Quaternion orient
	)
{
    Ogre::Entity*       ent;
    Ogre::SceneNode*    node;

	ent = mSceneMgr->createEntity(name, BACK_PLANE_MESH_XY);
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(pos);
	node->setOrientation(orient);
}

void CBaseLayout::GenerateManualWallMeshes()
{
	Ogre::Real backplaneWidth = mCylRadius;
	Ogre::Real backplaneHeight = mCylHeight;
	Ogre::Real Offset = 1.5;
	Ogre::Real floorPlaneWidth = 400;
	Ogre::Real floorPlaneDepth = 400;
	char materail_name[128];

	Ogre::Vector3     pos(0,-backplaneHeight / 2, 0);
	Ogre::Quaternion  orient(Ogre::Quaternion::IDENTITY);

	createBackPlaneMeshXY(backplaneWidth + 2 * Offset, backplaneHeight);
    createPlaneMeshZX(floorPlaneWidth,floorPlaneDepth);

	// Floor
	createPlaneZX(mSceneMgr, "floor", pos, orient);

	// Roof
	pos = Ogre::Vector3 (0, backplaneHeight / 2, 0);
	Ogre::Vector3 Origin(0, 0, 0);
	Ogre::Vector3 Direction = Origin - pos;			// new direction of the plane normal
	Ogre::Vector3 src = Ogre::Vector3::UNIT_Y;		// Current direction of the plane normal

	orient = src.getRotationTo(Direction);

	createPlaneZX(mSceneMgr, "roof", pos, orient);

	// Background

	int material_id = 1;
	sprintf (materail_name,"Cylindrical/Option%d/Wall",material_id);
	for (int i=0; i < mSegments; i++) {
		char entity_name[128];
		sprintf (entity_name,"wall%d",i);
		getWallPosForHexLayout(i, mCylRadius, Offset, pos, orient);
		createBackPlaneXY(mSceneMgr,entity_name, pos, orient);
	}
	UpdateMaterial(mCrntOption);
}

void CBaseLayout::CreateVideoPlaneMeshInstance()
{
	Ogre::Vector3 pos;
	Ogre::Quaternion orient;
	int nStrmId;
	for (nStrmId = 0;nStrmId < mSegments; nStrmId++){
		Ogre::Entity*       ent;
		Ogre::SceneNode*    node;
		Ogre::String ent_name = STREAM_ENTITY_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);
		Ogre::String material_name = STREAM_MATERAIL_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);

		createPlaneMeshXY(ent_name.c_str(), mVidplaneWidth,mVidplaneHeight);

		ent = mSceneMgr->createEntity(ent_name, ent_name);
		ent->setMaterialName(material_name);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);

		getPlanePosForHexLayout(nStrmId, pos, orient);
		node->setPosition(pos);
		node->setOrientation(orient);
		ent->setVisible(false);
	}

	for (nStrmId = 0;nStrmId < MAX_ANCHORS;nStrmId++){
		Ogre::Entity*       ent;
		Ogre::SceneNode*    node;

		Ogre::String ent_name = ANCHOR_ENTITY_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);
		Ogre::String material_name = ANCHOR_ENTITY_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);

		createPlaneMeshXY(ent_name.c_str(), mVidplaneWidth,mVidplaneHeight);
		ent = mSceneMgr->createEntity(ent_name, ent_name);
		ent->setMaterialName(material_name);
		
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);

		Ogre::Vector3 scale(0.5, 0.5, 1.0);
		node->setScale(scale);
		pos = Ogre::Vector3((MAX_ANCHORS - nStrmId) * (mCylRadius / 4), - mCylHeight / 3, 0);
		node->setPosition(pos);
		ent->setVisible(false);
	}
}

bool CBaseLayout::Is2DLayout()
{
	switch(mCrntLayoutOption)
	{
		case LAYOUT_3D_PLANE:
		case LAYOUT_3D_CURVE:
			return false;
		default:
			return true;
	}
}

void CBaseLayout::UpdateLayoutOption()
{
	Ogre::Vector3 pos;
	Ogre::Quaternion orient;
	int nStrmId;

	CPlaneLayout *pPlaneLayout = NULL;

	switch(mCrntLayoutOption)
	{
		case LAYOUT_3D_PLANE:
		case LAYOUT_3D_CURVE:
			ShowGraphics(true);
			pPlaneLayout = new CPlaneLayoutHex(mCylRadius, mCylHeight);
			break;
		case LAYOUT_2D_D1D2XD3A1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDDxDA;
			break;

		case LAYOUT_2D_D1D2XD3:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDDxD;
			break;

		case LAYOUT_2D_D1D2XA1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDDxA;
			break;

		case LAYOUT_2D_D1D2:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDD;
			break;

		case LAYOUT_2D_D1D2D3:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDDDClip(mAspectRatio);
			break;

		case LAYOUT_2D_D1A1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDA(0);
			break;

		case LAYOUT_2D_D2A1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDA(1);
			break;

		case LAYOUT_2D_D3A1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutDA(2);
			break;

		case LAYOUT_2D_D1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutD(0);
			break;

		case LAYOUT_2D_D2:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutD(1);
			break;

		case LAYOUT_2D_D3:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutD(2);
			break;

		case LAYOUT_2D_A1:
			ShowGraphics(false);
			pPlaneLayout = new CFlatLayoutA(0);
			break;

			
		default:
			return;
	}

	float horzBoarder = (mfBoarder ? 0.01 : 0.00);
	float vertBoarder = (mfBoarder ? 0.01 : 0.00);
	mVidplaneHeight -= (mfBoarder ? 0.01 : 0.00);

	for (nStrmId = 0;nStrmId < mSegments; nStrmId++){
		Ogre::Entity*       ent;
		Ogre::Node *node;
		Ogre::String ent_name = STREAM_ENTITY_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);

		ent = mSceneMgr->getEntity(ent_name);
		node = ent->getParentNode();
		bool fState = mStates[ent_name];

		CPlane &Plane = pPlaneLayout->mPlanes[nStrmId];
		if(Plane.mfUsed){
			pPlaneLayout->getPlanePos(nStrmId, pos, orient);

			if(Is2DLayout()) {
				pos.x = pos.x * mVidplaneWidth * Plane.mWidth;
				pos.y = pos.y * mVidplaneHeight * Plane.mHeight;
				pos.z = pos.z * mCylRadius;	
			}

			UpdatePlaneSize(ent_name, mVidplaneWidth * Plane.mWidth - horzBoarder, mVidplaneHeight * Plane.mHeight - vertBoarder, Plane.mUStart, Plane.mVStart, Plane.mUEnd, Plane.mVEnd);
			node->setPosition(pos);
			node->setOrientation(orient);
			ent->setVisible(fState);

		} else {
			ent->setVisible(false);
		}
	}
	for (nStrmId = 0;nStrmId < MAX_ANCHORS;nStrmId++){
		Ogre::Entity*  ent;
		Ogre::Node*    node;

		Ogre::String ent_name = ANCHOR_ENTITY_NAME_PREFIX + Ogre::StringConverter::toString(nStrmId+1);
		bool fState = mStates[ent_name];
		ent = mSceneMgr->getEntity(ent_name);
		node = ent->getParentNode();
		CPlane &Plane = pPlaneLayout->mAnchors[nStrmId];
		if(Plane.mfUsed){

			pPlaneLayout->getAnchorPos(nStrmId, pos, orient);

			if(!Is2DLayout()) {
				Ogre::Vector3 scale(0.5, 0.5, 1.0);
				node->setScale(scale);

			} else {
				pos.x = pos.x * mVidplaneWidth * Plane.mWidth;
				pos.y = pos.y * mVidplaneHeight * Plane.mHeight;
				pos.z = pos.z * mCylRadius;

				Ogre::Vector3 scale(1.0, 1.0, 1.0);
				node->setScale(scale);
			}
			UpdatePlaneSize(ent_name, mVidplaneWidth * Plane.mWidth - horzBoarder, mVidplaneHeight * Plane.mHeight - vertBoarder, Plane.mUStart, Plane.mVStart, Plane.mUEnd, Plane.mVEnd);
			node->setPosition(pos);
			ent->setVisible(fState);
		} else {
			ent->setVisible(false);
		}
	}
	if(pPlaneLayout)
		delete pPlaneLayout;
}

void CBaseLayout::UpdateMaterial(int option_id)
{
	char material_name[128];
	char entity_name[128];
	int material_id = option_id + 1;
	sprintf (material_name,"Cylindrical/Option%d/Wall",material_id);
	for (int i=0; i < mSegments; i++) {
		sprintf (entity_name,"wall%d",i);
		Ogre::Entity *ent = mSceneMgr->getEntity(entity_name);
		if(ent) {
			ent->setMaterialName(material_name);
		}
	}
	// Roof
	{
		sprintf (material_name,"Cylindrical/Option%d/Roof",material_id);
		sprintf (entity_name,"roof");
		Ogre::Entity *ent = mSceneMgr->getEntity(entity_name);
		if(ent) {
			ent->setMaterialName(material_name);
		}
	}
	// Floor
	{
		sprintf (material_name,"Cylindrical/Option%d/Floor",material_id);
		sprintf (entity_name,"floor");
		Ogre::Entity *ent = mSceneMgr->getEntity(entity_name);
		if(ent) {
			ent->setMaterialName(material_name);
		}
	}

}

void CBaseLayout::createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
{
	/* Create Plane Mesh */
	Ogre::MovablePlane  *mPlane;
	mPlane = new Ogre::MovablePlane("PlaneXY");
	mPlane->d = 0;
	mPlane->normal = Ogre::Vector3::UNIT_Z;

	Ogre::MeshManager::getSingleton().createPlane(szMeshName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*mPlane, w, h, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
}

void CBaseLayout::EnableStream(std::string &entity, bool fEnable)
{
	Ogre::Entity *ent =  mSceneMgr->getEntity(entity);
	if(ent) {
		ent->setVisible(fEnable);
		mStates[entity] = fEnable;
	}

}

void CBaseLayout::setMeshRotation(
	Ogre::String    &entName, 
	Ogre::Real		angle)
{
	Ogre::Entity *ent =  mSceneMgr->getEntity(entName);
	if(ent) {
		Ogre::Node *node = ent->getParentNode();
		if(node){
			Ogre::Radian pitch, yaw, roll;
			Ogre::Quaternion quat =  node->getOrientation();
			Ogre::Matrix3 mat;
			
			quat.ToRotationMatrix(mat);
			mat.ToEulerAnglesYXZ(yaw, pitch, roll);

			roll = Ogre::Radian(Ogre::Math::PI / 180 * angle);

			mat.FromEulerAnglesYXZ(yaw, pitch, roll);
			quat.FromRotationMatrix(mat);
			node->setOrientation(quat);
		}
	}
}
void CBaseLayout::setPitch(Ogre::String    &entName, Ogre::Real angle)
{
	Ogre::Entity *ent =  mSceneMgr->getEntity(entName);
	if(ent) {
		Ogre::Node *node = ent->getParentNode();
		if(node){
			Ogre::Radian radPitch = Ogre::Radian(Ogre::Math::PI / 180 * angle);
			node->pitch(radPitch);
		}
	}
}

