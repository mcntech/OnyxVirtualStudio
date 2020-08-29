#include "stdafx.h"
#include "ConfigComn.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void GetConfigFoderPath(CString &cfgFolderPath)
{
	PWSTR romingFolder = NULL; 
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &romingFolder);
	cfgFolderPath = romingFolder;
	cfgFolderPath += APP_INSTALL_FOLDER;
	CoTaskMemFree(romingFolder);
}


int GetOnyxOutStreamInf(
			const char *ConfigFile,
			const char *SectionName,
			CAVSrcInf  &VidSrcInf
			)
{

	char szValue[MAX_PATH] = {0};

	GetPrivateProfileStringA(SectionName,KEY_NAME_WIDTH,"",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mWidth = atoi(szValue);

	GetPrivateProfileStringA(SectionName,KEY_NAME_HEIGHT,"",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mHeight = atoi(szValue);

	GetPrivateProfileStringA(SectionName,KEY_NAME_STREAM,"",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mSharedMemName = szValue;

	GetPrivateProfileStringA(SectionName,KEY_NAME_MIN_BITRATE,"500000",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mVMinBitrate = atoi(szValue);

	GetPrivateProfileStringA(SectionName,KEY_NAME_MAX_BITRATE,"1500000",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mVMaxBitrate = atoi(szValue);

	GetPrivateProfileStringA(SectionName,KEY_NAME_FAIR_BITRATE,"6000000",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mVFairBitrate = atoi(szValue);

	GetPrivateProfileStringA(SectionName,KEY_NAME_GOP_LENGTH,"60",szValue,MAX_PATH - 1,ConfigFile);
	VidSrcInf.mGopLength = atoi(szValue);

	VidSrcInf.mSectionName = SectionName;
	if(VidSrcInf.mWidth > 0 && VidSrcInf.mWidth <= 1920 && VidSrcInf.mHeight > 0 && VidSrcInf.mHeight < 1200)
		return 0;
	else
		return -1;
}
