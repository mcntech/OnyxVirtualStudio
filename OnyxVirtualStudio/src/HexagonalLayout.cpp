#include "HexagonalLayout.h"

const int modelWidth  = 360;
const int modelHeight = 140;

void CHexagonalLayout::createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h)
{
	/* Create Plane Mesh */
	Ogre::MovablePlane  *mPlane;
	mPlane = new Ogre::MovablePlane("BackPlaneXY");
	mPlane->d = 0;
	mPlane->normal = Ogre::Vector3::UNIT_Z;

	Ogre::MeshManager::getSingleton().createPlane(BACK_PLANE_MESH_XY, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*mPlane, w, h, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
}

void CHexagonalLayout::createPlaneMeshZX(Ogre::Real w, Ogre::Real h)
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

void CHexagonalLayout::createPlaneZX(
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

void CHexagonalLayout::createBackPlaneXY(
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

CHexagonalLayout::CHexagonalLayout(	)
{

}


void CHexagonalLayout::createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
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
