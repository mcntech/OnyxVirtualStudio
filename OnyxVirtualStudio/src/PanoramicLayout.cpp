#include "PanoramicLayout.h"

#if 0
static UVCord_T defVertsUV[MESH_NUM_COL*MESH_NUM_ROW] =
{
	{ 0.00, 0.00},
	{ 0.50, 0.00},
	{ 1.00, 0.00},
	{ 0.00, 0.50},
	{ 0.50, 0.50},
	{ 1.00, 0.50},
	{ 0.00, 1.00},
	{ 0.50, 1.00},
	{ 1.00, 1.00}
};
#endif
#if 0
static UVCord_T defVertsUV[MESH_NUM_COL*MESH_NUM_ROW] =
{
	{ 0.00, 1.00},
	{ 0.50, 1.00},
	{ 1.00, 1.00},
	{ 0.00, 0.50},
	{ 0.50, 0.50},
	{ 1.00, 0.50},
	{ 0.00, 0.00},
	{ 0.50, 0.00},
	{ 1.00, 0.00}
};
#endif
#if 1
static UVCord_T defVertsUV[MESH_NUM_COL*MESH_NUM_ROW] =
{
	{ 0.00, 0.00},
	{ 0.00, 0.50},
	{ 0.00, 1.00},
	{ 0.50, 0.00},
	{ 0.50, 0.50},
	{ 0.50, 1.00},
	{ 1.00, 0.00},
	{ 1.50, 0.50},
	{ 1.00, 1.00}
};
#endif

void CUVMap::GenUvCord()
{
	int   nCols=MESH_NUM_COL; 
	int   nRows=MESH_NUM_ROW;
	UVCord_T *pUvCord = mUuVerts;

	float du = (mUEnd - mUStart - mCropLeft - mCropRight) / (nCols - 1);
	float dv = (mVEnd - mVStart - mCropTop - mCropBottom) / (nRows - 1);
	float v = mVStart + mCropTop;
	for(int i=0; i < nRows; i++) {
		float u = mUStart + mCropLeft;
		for(int j=0; j < nCols; j++){
			pUvCord->u = u;
			pUvCord->v = v;
			pUvCord++;
			u += du;
		}
		v += dv;
	}
}

void CUVMap::RotateUvCord(int nNewAngle)
{
	UVCord_T *pUvCord = mUuVerts;
	int nCols=MESH_NUM_COL;
	int nRows=MESH_NUM_ROW;

	if(nNewAngle == mUvAngle)
		return;
	int nAngle = (360 + (nNewAngle - mUvAngle)) % 360;
	UVCord_T *pTmp = (UVCord_T *)malloc(nCols * nRows * sizeof(UVCord_T));
	memcpy(pTmp, pUvCord, nCols * nRows * sizeof(UVCord_T));
	for (int i=0; i < nRows; i++) {
		int ySrc = i - (nRows - 1) / 2; // Translate center to middle coordinate
		for (int j=0; j < nCols; j++) {
			int xSrc = j - (nCols - 1) / 2; // Translate center to middle coordinate

			int xDst = xSrc;
			int yDst = ySrc;
			if( nAngle == 90) {
				xDst = ySrc;
				yDst = -xSrc;
			} else if( nAngle == 180) {
				xDst = -xSrc;
				yDst = -ySrc;
			} else if( nAngle == 270) {
				xDst = -ySrc;
				yDst = xSrc;
			}

			xDst += (nCols - 1) / 2;
			yDst += (nRows - 1) / 2;
			*(pUvCord + yDst * nCols + xDst) = *(pTmp + i * nCols + j);
		}
	}
	mUvAngle = nNewAngle;
	free(pTmp);
}

void CUVMap::GetMeshCtrlPts(PatchVertex *pVertsXY)
{
	int nCtrlXPts = MESH_NUM_COL;
	int nCtrlYPts = MESH_NUM_ROW;
	int dx = mWidth / (nCtrlXPts - 1);
	int dy = mHeight / (nCtrlYPts - 1);
	
	for (int row=0; row < nCtrlXPts; row++) {
		for (int col=0; col < nCtrlYPts; col++) {
			int n = row * nCtrlXPts + col;
			pVertsXY[n].x = -mWidth/2 + col* dx;
			pVertsXY[n].y = mHeight/2 - row * dy;
			if(n == 1 || n == 4 || n == 7)
				pVertsXY[n].z = mDepth;
			else 
				pVertsXY[n].z = 0;

			pVertsXY[n].nx = pVertsXY[n].ny = 0.0; 
			pVertsXY[n].nz = 1.0;

			pVertsXY[n].u = mUuVerts[n].u;
			pVertsXY[n].v = mUuVerts[n].v;

		}
	}
}

void CUVMap::MoveUvEdgeRight(float du)
{
	UVCord_T *pVertsUV = mUuVerts;
	int numVerts = MESH_NUM_COL * MESH_NUM_ROW;
	for (int i=MESH_NUM_COL-1; i < numVerts; i = i + MESH_NUM_COL) {
		pVertsUV[i].u += du;
	}
	for (int i=MESH_NUM_COL-2; i < numVerts; i = i + MESH_NUM_COL) {
		pVertsUV[i].u = (pVertsUV[i - 1].u + pVertsUV[i +1].v) / 2;
	}
}

void CUVMap::MoveUvEdgeLeft(float du)
{
	UVCord_T *pVertsUV = mUuVerts;
	int numVerts = MESH_NUM_COL * MESH_NUM_ROW;
	for (int i=0; i < numVerts; i = i + MESH_NUM_COL) {
		pVertsUV[i].u += du;
	}
	for (int i=1; i < numVerts; i = i + MESH_NUM_COL) {
		pVertsUV[i].u = (pVertsUV[i - 1].u + pVertsUV[i +1].v) / 2;
	}
}

void CUVMap::MoveUvEdgeUp(float dv)
{
	UVCord_T *pVertsUV = mUuVerts;
	int numVerts = MESH_NUM_COL * MESH_NUM_ROW;
	for (int i=0; i < MESH_NUM_COL; i++) {
		pVertsUV[i].v += dv;
	}
	for (int i=MESH_NUM_COL; i < MESH_NUM_COL*2; i++) {
		pVertsUV[i].v = (pVertsUV[i - MESH_NUM_COL].v + pVertsUV[i + MESH_NUM_COL].v) / 2;
	}
}

void CUVMap::MoveUvEdgeDn(float dv)
{
	UVCord_T *pVertsUV = mUuVerts;
	int numVerts = MESH_NUM_COL * MESH_NUM_ROW;
	for (int i=numVerts - MESH_NUM_COL; i < numVerts; i++) {
		pVertsUV[i].v += dv;
	}
	for (int i=numVerts - 2 * MESH_NUM_COL; i < numVerts - MESH_NUM_COL; i++) {
		pVertsUV[i].v = (pVertsUV[i - MESH_NUM_COL].v + pVertsUV[i + MESH_NUM_COL].v) / 2;
	}
}

void CUVMap::MoveUvVert(int nVertNum,  float du, float dv)
{
	UVCord_T *pVertsUV = mUuVerts;
	switch(nVertNum)
	{
		case 0:
		case 2:
		case 6:
		case 8:
		{
			pVertsUV[nVertNum].u += du; 
			pVertsUV[nVertNum].v += dv; 
		}
		break;
		case 1:
		case 7:
		{
			for (int i=nVertNum-1; i <= nVertNum + 1; i++) {
				pVertsUV[i].u += du;
				pVertsUV[i].v += dv;
			}
		}
		break;
		case 3:
		case 5:
		{
			for (int i=nVertNum-3; i <= nVertNum + 3; i=i+3) {
				pVertsUV[i].u += du;
				pVertsUV[i].v += dv;
			}
		}
		break;
		case 4:
		{
			for (int i=0; i < 9 ; i++) {
				pVertsUV[i].u += du;
				pVertsUV[i].v += dv;
			}
		}
		break;
	}
}

void CPanoramicLayout::SetCrop(std::string entName, float left, float top, float right, float bottom)
{
	Ogre::PatchMeshPtr pPatch = Ogre::MeshManager::getSingleton().getByName(entName);
	if(pPatch.isNull()) {
		return;
	}
	CUVMap &UVMap = mUVMap[entName];
	
	UVMap.mCropLeft = left;
	UVMap.mCropTop = top;
	UVMap.mCropRight = right;
	UVMap.mCropBottom = bottom;

	UVMap.GenUvCord();
	UpdateBezierPatchUV(pPatch, &UVMap);
}

void CPanoramicLayout::SetRotation(std::string entName, int nRot)
{
	Ogre::PatchMeshPtr pPatch = Ogre::MeshManager::getSingleton().getByName(entName);
	if(pPatch.isNull()) {
		return;
	}
	CUVMap &UVMap = mUVMap[entName];
	
	UVMap.RotateUvCord(nRot);
	UpdateBezierPatchUV(pPatch, &UVMap);
}

void CPanoramicLayout::CropCorner(std::string entName, int nVertNum,  float du, float dv)
{
	Ogre::PatchMeshPtr pPatch = Ogre::MeshManager::getSingleton().getByName(entName);
	if(pPatch.isNull()) {
		return;
	}
	
	CUVMap &UVMap = mUVMap[entName];
	UVMap.MoveUvVert(nVertNum, du, dv);
	UpdateBezierPatchUV(pPatch, &UVMap);
}

void CPanoramicLayout::UpdateCurvature(bool fCurve)
{
	for (std::map<std::string,CUVMap>::iterator it=mUVMap.begin();it != mUVMap.end(); it++) {
		std::string entName = it->first;
		Ogre::PatchMeshPtr pPatch = Ogre::MeshManager::getSingleton().getByName(entName);
		if(!pPatch.isNull()) {
			CUVMap &UVMap = it->second;
			UVCord_T *pVertsUV = UVMap.mUuVerts;
			UVMap.mDepth =  getCurvedPlaneDepth(entName.c_str());
			UpdateBezierPatchUV(pPatch, &UVMap);
		}
	}
}

void CPanoramicLayout::UpdatePlaneSize(
	std::string entName,
	Ogre::Real w, Ogre::Real h, 
	Ogre::Real vidLeft, Ogre::Real vidTop, Ogre::Real vidRight,  Ogre::Real vidBottom)
{
	Ogre::PatchMeshPtr pPatch = Ogre::MeshManager::getSingleton().getByName(entName);
	if(pPatch.isNull()) {
		return;
	}

	CUVMap &UVMap = mUVMap[entName];
	
	UVMap.mWidth = w;
	UVMap.mHeight = h;
	UVMap.mUStart = vidLeft;
	UVMap.mVStart = vidTop;
	UVMap.mUEnd = vidRight;
	UVMap.mVEnd = vidBottom;
	UVMap.GenUvCord();
	UpdateBezierPatchUV(pPatch, &UVMap);
}

void CPanoramicLayout::CreateBezierPatchXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
{
	//Ogre::Pass* mPatchPass;
	Ogre::PatchMeshPtr     Patch;
	Ogre::VertexDeclaration *pDecl;
	PatchVertex            VertsXY[9];
	CUVMap UVMap(w,h);

	UVMap.mDepth = getCurvedPlaneDepth(szMeshName);

	UVMap.GenUvCord();
	UVMap.GetMeshCtrlPts(VertsXY);

	// specify a vertex format declaration for our patch: 3 floats for position, 3 floats for normal, 2 floats for UV
    pDecl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
    pDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    pDecl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    pDecl->addElement(0, sizeof(float) * 6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	// create a patch mesh using vertices and declaration
	Patch = Ogre::MeshManager::getSingleton().createBezierPatch(szMeshName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, (float*)VertsXY, pDecl, 3, 3, 5, 5, Ogre::PatchSurface::VS_BACK);
	Patch->setSubdivision(1);   // start at 0 detail

	mUVMap[szMeshName] = UVMap;
}


void CPanoramicLayout::UpdateBezierPatchUV(Ogre::PatchMeshPtr pPatch,  CUVMap *pUVMap)
{
	//Ogre::Pass* mPatchPass;
	PatchVertex       VertsXY[9];
	pUVMap->GetMeshCtrlPts(VertsXY);

	// create a patch mesh using vertices and declaration
	pPatch->update((float*)VertsXY, 3, 3, 5, 5, Ogre::PatchSurface::VS_BACK);
	pPatch->setSubdivision(1);   // start at 0 detail
}


void CPanoramicLayout::createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h)
{
	CreateBezierPatchXY(BACK_PLANE_MESH_XY, w, h);
}

void CPanoramicLayout::createPlaneMeshZX(Ogre::Real w, Ogre::Real h)
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


void CPanoramicLayout::createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h)
{
	CreateBezierPatchXY(szMeshName, w, h);
}


float CPanoramicLayout::getCurvedPlaneDepth(const char *szMeshName)
{
	switch(mCrntLayoutOption)
	{
		case LAYOUT_3D_CURVE:
			if(strncmp(szMeshName, ANCHOR_ENTITY_NAME_PREFIX, strlen(ANCHOR_ENTITY_NAME_PREFIX)) == 0)
				return 0.0;
			else
				return -40.0;
			break;
		default:
			return 0;
			break;
	}
}