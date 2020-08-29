#include "CylindricalLayout.h"

	// define the control point vertices for our patch
static PatchVertex defVertsXY[9] =
{
	{-80,  45,   0,  0.0, 0.0, 1.0, 0.0, 0.0},
	{  0,  45, -40,  0.0, 0.0, 1.0, 0.5, 0.0},
	{ 80,  45,   0,  0.0, 0.0, 1.0, 1.0, 0.0},
	{-80,   0,   0,  0.0, 0.0, 1.0, 0.0, 0.5},
	{  0,   0, -40,  0.0, 0.0, 1.0, 0.5, 0.5},
	{ 80,   0,   0,  0.0, 0.0, 1.0, 1.0, 0.5},
	{-80, -45,   0,  0.0, 0.0, 1.0, 0.0, 1.0},
	{  0, -45, -40,  0.0, 0.0, 1.0, 0.5, 1.0},
	{ 80, -45,   0,  0.0, 0.0, 1.0, 1.0, 1.0}
};

void CylindricalLayout::CreateBezierPatchXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
{
	Ogre::PatchMeshPtr mPatch;
	Ogre::VertexDeclaration* mDecl;
	Ogre::Pass* mPatchPass;
	memcpy(mVertsXY, defVertsXY, sizeof(mVertsXY));
	mVertsXY[0].x = mVertsXY[3].x = mVertsXY[6].x = -w/2;
	mVertsXY[1].x = mVertsXY[4].x = mVertsXY[7].x = 0;
	mVertsXY[2].x = mVertsXY[5].x = mVertsXY[8].x = w/2;
	mVertsXY[0].y = mVertsXY[1].y = mVertsXY[2].y = h/2;
	mVertsXY[3].y = mVertsXY[4].y = mVertsXY[5].y = 0;
	mVertsXY[6].y = mVertsXY[7].y = mVertsXY[8].y = -h/2;
	// specify a vertex format declaration for our patch: 3 floats for position, 3 floats for normal, 2 floats for UV
    mDecl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
    mDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    mDecl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    mDecl->addElement(0, sizeof(float) * 6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	// create a patch mesh using vertices and declaration
	mPatch = Ogre::MeshManager::getSingleton().createBezierPatch(szMeshName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, (float*)mVertsXY, mDecl, 3, 3, 5, 5, Ogre::PatchSurface::VS_BACK);
	mPatch->setSubdivision(1);   // start at 0 detail
}

void CylindricalLayout::createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h)
{
	CreateBezierPatchXY(BACK_PLANE_MESH_XY, w, h);
}
void CylindricalLayout::createPlaneMeshZX(Ogre::Real w, Ogre::Real h)
{
	/* Create Plane Mesh */
	Ogre::MovablePlane  *mPlane;
	mPlane = new Ogre::MovablePlane(PLANE_MESH_ZX);
	mPlane->d = 0;
	mPlane->normal = Ogre::Vector3::UNIT_Y;

	Ogre::MeshManager::getSingleton().createPlane(PLANE_MESH_ZX, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*mPlane, w, h, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_X);
}




void CylindricalLayout::createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
{
	CreateBezierPatchXY(szMeshName, w, h);
}

