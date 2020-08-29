#ifndef __CONFIG_COMN_H__
#define __CONFIG_COMN_H__

#include <string>

#define APP_INSTALL_FOLDER        "\\MCN\\Onyx Virtual Studio Elite"
#define ONYX_CONFIG_FILE          "\\ovs.cfg"

#define KEY_NAME_WIDTH          "Width"
#define KEY_NAME_HEIGHT         "Height"
#define KEY_NAME_STREAM         "Video"
#define KEY_NAME_MIN_BITRATE    "MinBitrate"
#define KEY_NAME_MAX_BITRATE    "MaxBitrate"
#define KEY_NAME_FAIR_BITRATE   "FairBitrate"
#define KEY_NAME_GOP_LENGTH     "GopLength"
// CPageInputVideos message handlers

class CVidDestFmtInf
{
public:
	CVidDestFmtInf(const char *szLabel, int nDestFmt):
	  mLabel(szLabel)
	  ,mFmt(nDestFmt)
	{

	}

	std::string mLabel;
	int         mFmt;
};

class CAVSrcInf
{
public:
	/* Items retrieved from ovs.cfg */
	std::string mSectionName;
	std::string mSharedMemName;

	int         mWidth;
	int         mHeight;
	int         mVMaxBitrate;
	int         mVMinBitrate;
	int         mVFairBitrate;
	int         mGopLength;
	
	/* Items retrieved/saved from orc.cfg or olc.cfg*/
	int         mVUserBitrate;
	int         mAUserBitrate;	
	int         mUserGopLen;
	/* Runtime generated */
	std::string mLabel;
};

void GetConfigFoderPath(CString &cfgFolderPath);

int GetOnyxOutStreamInf(
			const char *ConfigFile,
			const char *SectionName,
			CAVSrcInf &AVSrcInf
			);

#endif