// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <stdio.h>

#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreStringConverter.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreExternalTextureSourceManager.h>
#include <OgreDataStream.h>
#include "SkypeUi.h"
#include "PluginUtil.h"
#include "PluginParams.h"
#include <OISEvents.h>
#include <OISKeyboard.h>
#include "OnyxControl.h"

#define OGREVIDEO_EXPORT extern "C" __declspec(dllexport)
#define OGRE_PLUGIN_EXPORT          __declspec(dllexport)

#define PLUGIN_TYPE "skype_video"

class OGRE_PLUGIN_EXPORT CAvStream
{
public:
	CAvStream(
		int *pResult,
		std::string filename
		);
	~CAvStream(){}

	std::string getName(){return mName;}
	int getWidth() 
	{ 
		CSkypeUi *pSkypeUi  = CSkypeUi::Instance();
		CVideoRx *pVideoRx = pSkypeUi->getVidRxStream();

		if(pVideoRx) 
			return pVideoRx->getWidth(); 
		else 
			return 0; 
	}
	
	int getHeight() 
	{ 
		CSkypeUi *pSkypeUi  = CSkypeUi::Instance();
		CVideoRx *pVideoRx = pSkypeUi->getVidRxStream();

		if(pVideoRx) 
			return pVideoRx->getHeight(); 
		else 
			return 0;
	}

	int getStride() 
	{ 
		CSkypeUi *pSkypeUi  = CSkypeUi::Instance();
		CVideoRx *pVideoRx = pSkypeUi->getVidRxStream();

		if(pVideoRx) {
			int nWidth = pVideoRx->getWidth();
			return PluginUtil::nextPow2(nWidth);
		} else {
			return 0;
		}
	}

	void update(float time_increase){}

	CVideoFrame* getNextFrame()
	{ 
		CSkypeUi *pSkypeUi  = CSkypeUi::Instance();
		CVideoRx *pVideoRx = pSkypeUi->getVidRxStream();

		if(pVideoRx) {
			if(pVideoRx->getFrame(mFrame) == 0){
				mFrame->mReady = 1;
				return mFrame;
			}
		} 
		return NULL;
	}
	void freeFrame(CVideoFrame* pFrm)
	{ 
		mFrame->clear();
	}
	void setAudioGain(float gain){}
	float getAudioGain(){ return 0.0;}

	void play(){}
	void pause(){}
	void stop(){}
	void setColorKeyValue(unsigned long ulColorKeyVal, unsigned long ulColorKeyThres)
	{
		if(ulColorKeyVal > 0) {
			fProcessColorKey = true;
			mColorKeyRatio = ulColorKeyVal;
			mColorKeyThresh = ulColorKeyThres;
		}
	}
	void setUpsideDown(bool fUpsideDown)
	{
		mUpsidedown = fUpsideDown;
	}
//private:
	//int Connect(std::string filename);

public:
	bool                mTextureInit;
	CVideoFrame*        mFrame;
	bool                mUpsidedown;
	bool                fProcessColorKey;
	unsigned long       mColorKeyRatio;
	unsigned long       mColorKeyThresh;
    std::string         mName;
	// Cache Ogre parameters
	Ogre::String       mMaterialName;
	Ogre::String       mGroupName;
};

class OGRE_PLUGIN_EXPORT CAvStreamManager
{
public:

	typedef std::list<CAvStream*> LIST_SREAMS_T;
	LIST_SREAMS_T mStreams;
	CSkypeUi*     mSkypeUi;

public:
	CAvStreamManager()
	{
		mSkypeUi = NULL;
	}
	~CAvStreamManager(){}

	void logMessage(std::string msg){}
	CAvStream* getAvStreamByName(Ogre::String name){ return NULL;}
	void destroyAvStream(CAvStream* strm){}
	CAvStream* createAvStream(std::string filename)
	{
		int Result = 0;
		CAvStream *pStream = new CAvStream(&Result, filename);
		mStreams.push_back(pStream);
		return pStream;
	}
	void getVersion(int* a,int* b,int* c)
	{ 
		*a=1; 
		*b=0; 
		*c=-2; 
	}
	void update(float time_increase)
	{ 
		foreach_l(CAvStream*,mStreams) { 
			(*it)->update(time_increase); 
		} 
	} 

};

class OGRE_PLUGIN_EXPORT CPlugin : 
						public CAvStreamManager,
						public Ogre::FrameListener, 
						public Ogre::ExternalTextureSource
{
public:

	std::map<std::string,Ogre::TexturePtr> mTextures;
	bool mbInit;
	bool mUpsideDown;
	public:
		/* Ogre Plugin API */
		bool initialise();
		void shutDown();
		void createDefinedTexture(const Ogre::String& material_name,
                                  const Ogre::String& group_name = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		
		void AdjustTextureIfNeeded(CAvStream* pStrm,	Ogre::TexturePtr pTexture);

		void createTextureDeferred(CAvStream* pStrm);

		void destroyAdvancedTexture(const Ogre::String& material_name,
                                    const Ogre::String& groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		bool frameStarted(const Ogre::FrameEvent& evt);

        bool setParameter(const Ogre::String &name,const Ogre::String &value);
        Ogre::String getParameter(const Ogre::String &name) const;

		/*  */
		static CPlugin* Instance(); 
		virtual ~CPlugin();

		unsigned long mColorKeyRatio;
		unsigned long mColorKeyThresh;
		std::string mUserId;
		std::string mPasswd;
private:
		CPlugin();
		static CPlugin* mInstance; 
};

CPlugin* CPlugin::mInstance = NULL;
CPlugin* CPlugin::Instance()
{
	if (!mInstance)
		mInstance = new CPlugin();
	return mInstance;
}

CAvStream::CAvStream(
		int *pResult,
		std::string filename
		)
{
	mTextureInit = false;
	mFrame = new CVideoFrame();
	mName = filename;
}


//int CAvStream::Connect(std::string filename)
//{
//	return 0;
//}
CPlugin::CPlugin() : CAvStreamManager()
{
	mPlugInName = "SkypeVideoPlugin";
	mDictionaryName = mPlugInName;
	mbInit = false;
	mUpsideDown = false;
	mTechniqueLevel=mPassLevel=mStateLevel=0;

	initialise();
}

bool CPlugin::initialise()
{
	if (mbInit) return false;
	mbInit=true;
	addBaseParams(); // ExternalTextureSource's function
	return true;
}

CPlugin::~CPlugin()
{
	shutDown();
}

void CPlugin::shutDown()
{
	if (!mbInit) return;

	mbInit=false;
}

bool CPlugin::setParameter(const Ogre::String &name,const Ogre::String &value)
{
	if(strcmp(name.c_str(), KEY_NAME_COLORKEY_RATIO) == 0){
		sscanf(value.c_str(),"%d",&mColorKeyRatio);
	} else 	if(strcmp(name.c_str(), KEY_NAME_COLORKEY_THRESH) == 0){
		sscanf(value.c_str(),"%d",&mColorKeyThresh);
	} else 	if(strcmp(name.c_str(), KEY_NAME_UPSIDEDOWN) == 0){
		sscanf(value.c_str(),"%d",&mUpsideDown);
	}

    return ExternalTextureSource::setParameter(name, value);
}

Ogre::String CPlugin::getParameter(const Ogre::String &name) const
{
    return ExternalTextureSource::getParameter(name);
}

#if 0
void CPlugin::createDefinedTexture(const Ogre::String& material_name,const Ogre::String& group_name)
{
	std::string name=mInputFileName;

	CAvStream* clip = createAvStream(mInputFileName);
	if(clip != NULL) {
		int w = PluginUtil::nextPow2(clip->getWidth());
		int h = PluginUtil::nextPow2(clip->getHeight());

		Ogre::TexturePtr t = Ogre::TextureManager::getSingleton().createManual(name,group_name,Ogre::TEX_TYPE_2D,w,h,1,0,Ogre::PF_A8R8G8B8,Ogre::TU_DYNAMIC_WRITE_ONLY);
		
		if (t->getFormat() != Ogre::PF_A8R8G8B8) {
			logMessage("ERROR: Pixel format is not PF_A8R8G8B8 which is what was requested!");
		}
		// clear it to black

		unsigned char* texData=(unsigned char*) t->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		memset(texData,0,w*h*4);
		t->getBuffer()->unlock();
		mTextures[name] = t;

		// attach it to a material
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(material_name);
		Ogre::TextureUnitState* ts = material->getTechnique(mTechniqueLevel)->getPass(mPassLevel)->getTextureUnitState(mStateLevel);

		//Now, attach the texture to the material texture unit (single layer) and setup properties
		ts->setTextureName(name,Ogre::TEX_TYPE_2D);
		ts->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
		ts->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		// scale tex coords to fit the 0-1 uv range
		Ogre::Matrix4 mat=Ogre::Matrix4::IDENTITY;
		mat.setScale(Ogre::Vector3((float) clip->getWidth()/w, (float) clip->getHeight()/h,1));
		ts->setTextureTransform(mat);
		
		clip->setColorKeyValue(mColorKeyValue, mColorKeyThresh);
		clip->setUpsideDown(mUpsideDown);
		clip->play();
	} else {
		// Creation failed
	}
}
#else
void CPlugin::createDefinedTexture(const Ogre::String& material_name,const Ogre::String& group_name)
{
	// Temporary
	size_t nCount = mInputFileName.find("@skype");
	if(nCount != std::string::npos) {
		mUserId = mInputFileName.substr(0, nCount);
		nCount = mInputFileName.find(":");
		if(nCount != std::string::npos) {
			mPasswd = mInputFileName.substr(nCount + 1);
		}
		CSkypeUi *pSkypeUi  = CSkypeUi::Instance();
		pSkypeUi->Login(mUserId.c_str(), mPasswd.c_str());
	}
	CAvStream* pStrm = createAvStream(mInputFileName);
	pStrm->mMaterialName = material_name;
	pStrm->mGroupName = group_name;
}

void CPlugin::AdjustTextureIfNeeded(CAvStream* pStrm,	Ogre::TexturePtr pTexture)
{
	int srcWidth = pStrm->getWidth();
	int srcHeight = pStrm->getHeight();
	int srcStride =  PluginUtil::nextPow2(srcWidth);
	int srcHeightPow2 =  PluginUtil::nextPow2(srcHeight);
	int destWidth = pTexture->getWidth();
	int destHeight = pTexture->getHeight();
	if(destWidth != srcStride || destHeight != srcHeightPow2) {
		pTexture->freeInternalResources();
		pTexture->setWidth(srcStride);
		pTexture->setHeight(srcHeightPow2);
		pTexture->createInternalResources();

		// attach it to a material
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(pStrm->mMaterialName);
		Ogre::TextureUnitState* ts = material->getTechnique(mTechniqueLevel)->getPass(mPassLevel)->getTextureUnitState(mStateLevel);

		// scale tex coords to fit the 0-1 uv range
		Ogre::Matrix4 mat=Ogre::Matrix4::IDENTITY;
		float sx = (float)srcWidth/srcStride;
		float sy = (float)srcHeight/srcHeightPow2;
		mat.setScale(Ogre::Vector3(sx, (float)sy,1));
		ts->setTextureTransform(mat);
	}
}

void CPlugin::createTextureDeferred(CAvStream* pStrm )
{
	if(pStrm != NULL) {
		int w = PluginUtil::nextPow2(pStrm->getWidth());
		int h = PluginUtil::nextPow2(pStrm->getHeight());

		Ogre::TexturePtr t = Ogre::TextureManager::getSingleton().createManual(pStrm->mName, pStrm->mGroupName,Ogre::TEX_TYPE_2D,w,h,1,0,Ogre::PF_A8R8G8B8,Ogre::TU_DYNAMIC_WRITE_ONLY);
		
		if (t->getFormat() != Ogre::PF_A8R8G8B8) {
			logMessage("ERROR: Pixel format is not PF_A8R8G8B8 which is what was requested!");
		}
		// clear it to black

		unsigned char* texData=(unsigned char*) t->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		memset(texData,0,w*h*4);
		t->getBuffer()->unlock();
		mTextures[pStrm->mName] = t;

		// attach it to a material
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(pStrm->mMaterialName);
		Ogre::TextureUnitState* ts = material->getTechnique(mTechniqueLevel)->getPass(mPassLevel)->getTextureUnitState(mStateLevel);

		//Now, attach the texture to the material texture unit (single layer) and setup properties
		ts->setTextureName(pStrm->mName,Ogre::TEX_TYPE_2D);
		ts->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
		ts->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		// scale tex coords to fit the 0-1 uv range
		Ogre::Matrix4 mat=Ogre::Matrix4::IDENTITY;
		float sx = (float)pStrm->getWidth()/w;
		float sy = (float)pStrm->getHeight()/h;
		mat.setScale(Ogre::Vector3(sx, (float)sy,1));
		ts->setTextureTransform(mat);
		
		pStrm->setColorKeyValue(mColorKeyRatio, mColorKeyThresh);
		pStrm->setUpsideDown(mUpsideDown);
		pStrm->play();
		pStrm->mTextureInit = true;
	} else {
		// Creation failed
	}
}

#endif
void CPlugin::destroyAdvancedTexture(const Ogre::String& material_name,const Ogre::String& groupName)
{
	logMessage("Destroying ogg_video texture on material: "+material_name);
}

bool CPlugin::frameStarted(const Ogre::FrameEvent& evt)
{
	if (evt.timeSinceLastFrame > 0.3f)
		update(0.3f);
	else
	    update(evt.timeSinceLastFrame);

	// update playing videos
	std::list<CAvStream*>::iterator it;
	CVideoFrame* f;
	for (it=mStreams.begin();it!=mStreams.end();it++) {
		f=(*it)->getNextFrame();
		if (f) 	{
			if(!(*it)->mTextureInit){
				createTextureDeferred(*it);
			}
			Ogre::TexturePtr t = mTextures[(*it)->getName()];
			AdjustTextureIfNeeded(*it, t);
			int stride = PluginUtil::nextPow2(f->getWidth());
			
			unsigned char *texData=(unsigned char*) t->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
			f->CopyToHwTextureARGB(texData, stride);

			t->getBuffer()->unlock();
			(*it)->freeFrame(f);
		}
	}
	return true;
}


OGREVIDEO_EXPORT void dllStartPlugin()
{
	CPlugin *pPlugin = CPlugin::Instance();

	int res = -1;


	if(!pPlugin->mSkypeUi){
		pPlugin->mSkypeUi  = CSkypeUi::Instance();
	}
	res = pPlugin->mSkypeUi->Start();

	if(res == 0) {
		Ogre::ExternalTextureSourceManager::getSingleton().setExternalTextureSource(PLUGIN_TYPE,pPlugin);
		Ogre::Root::getSingleton().addFrameListener(pPlugin);
	}

}

OGREVIDEO_EXPORT void dllStopPlugin()
{
	CPlugin *pPlugin = CPlugin::Instance();
	Ogre::Root::getSingleton().removeFrameListener(pPlugin);
}
