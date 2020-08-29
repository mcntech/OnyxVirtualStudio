#ifndef __CylindricalLayout_h_
#define __CylindricalLayout_h_

#include "BaseLayout.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(push, 1)
#endif
    struct PatchVertex
	{
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(pop)
#endif

class CylindricalLayout : public CBaseLayout
{
public:
	CylindricalLayout()
	{
	}
	virtual ~CylindricalLayout(){}

	void createBackPlaneMeshXY(Ogre::Real w, Ogre::Real h);
	void createPlaneMeshZX(Ogre::Real w, Ogre::Real h);
	void createPlaneMeshXY(const char *szMeshName, Ogre::Real w, Ogre::Real h);

	void  CreateBezierPatchXY(const char *szMeshName, Ogre::Real w, Ogre::Real h);
	PatchVertex mVertsXY[9];
};

#endif // #ifndef __CylindricalLayout_h_
