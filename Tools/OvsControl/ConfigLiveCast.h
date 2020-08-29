#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include "ConfigComn.h"

class CConfigLiveCast
{
public:
	CConfigLiveCast(const char *StreamName, CAVSrcInf &mAVSrcInf);
	void Save(CAVSrcInf &mAVSrcInf);
	~CConfigLiveCast();

	void GetEth0LocaIpAddr(std::string &strIpAddr);

	int mDestType;
	int mVidOutSize;

	std::string mCrntAudioDevice;
	std::string mCrntVideoDevice;
	std::string mCrntFileName;
	
	std::string mHlsUrl;
	std::string mHlsSrvAddr;
	std::string mHlsSrvPort;
	std::string mHlsSrvRoot;
	std::string mHlsSrvFolder;
	std::string mHlsSrvStream;
	std::string mHlsSrvSegmentDuration;
	std::string mHlsSrvLiveOnly;

	std::string mHlsS3Url;
	std::string mHlsPublishStream;
	std::string mHlsPublishFolder;
	std::string mHlsPublishS3Bucket;
	std::string mHlsPublishS3Host;
	std::string mHlsPublishS3AccessId;
	std::string mHlsPublishS3SecKey;
	std::string mHlsPublishSegmentDuration;
	std::string mHlsPublishLiveOnly;

	std::string mHRtspUrl;
	std::string mRtspIpAddr;
	std::string mRtspIpPort;
	std::string mRtspResource;
	std::string mRtmpUrl;
	std::string mRtmpUrlBackup;
	int         mRecordServer1;
	int         mRecordServer2;
	std::string mStreamName;

	int         mFrameRate;


	std::string mRtpRecHost;
	std::string mRtpRecUserId;
	std::string mRtpRecPasswd;
	std::string mRtpRecStream;
	int         mRtpRecRemotePort;
};
