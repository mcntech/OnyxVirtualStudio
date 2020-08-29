#include "stdafx.h"
#include <string>
#include "ConfigLiveCast.h"
#include "DshowCapGraph.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#define LIVECAST_CONFIG_FILE      "\\olc.cfg"

static char* convertWStringToCString(WCHAR* wString)
{
    const int MAX_STRINGZ=500;
    static char ctext[MAX_STRINGZ+2];

    if (WideCharToMultiByte(CP_ACP, 0, wString, -1, ctext, MAX_STRINGZ,0,0) == 0)  {
        throw("convertWStringToCString failed with no extra error info");
    }
    return ctext;
}

CConfigLiveCast::CConfigLiveCast(const char *StreamName, CAVSrcInf &mAVSrcInf)
	{
	   char szPath[128] = "";
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		mStreamName = StreamName;
		CString configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + LIVECAST_CONFIG_FILE;
		std::string configOvs = configFolder + ONYX_CONFIG_FILE;
		char szValue[MAX_PATH];
		// TODO
		mDestType = DEST_TYPE_RTPMP;
		
		GetPrivateProfileStringA(mStreamName.c_str(), "AudioInput",CSTR_NO_AUDIO,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntAudioDevice = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "VideoInput",CSTR_MCN_VCAP,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntVideoDevice = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvPort","8080",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvPort = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvStream","channel1",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvStream = szValue;
		GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvFolder","httplive",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvFolder = szValue;
		GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvRoot","c:/teststreams",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvRoot = szValue;
				GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvSegmentDuration","4",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvSegmentDuration = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsSrvLiveOnly","1",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsSrvLiveOnly = szValue;


		GetPrivateProfileStringA(mStreamName.c_str(), "RtspPort","5540",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspIpPort = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtspResourceName",StreamName,szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspResource = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishS3AccessId","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishS3AccessId = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishS3SecKey","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishS3SecKey = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsHost","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishS3Host = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsBucket","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishS3Bucket = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishStream","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishStream = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishFolder","test",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishFolder = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishSegmentDuration","4",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishSegmentDuration = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPublishLiveOnly","0",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsPublishLiveOnly = szValue;

		std::string strIpAddr;
		GetEth0LocaIpAddr(strIpAddr);
		mRtspIpAddr = strIpAddr;
		mHlsSrvAddr = strIpAddr;
		
		mHlsUrl = "http://" + mHlsSrvAddr + ":" + mHlsSrvPort + "/" +  mHlsSrvFolder + "/" + mHlsSrvStream + ".m3u8";
		
		mHRtspUrl = "rtsp://" + mRtspIpAddr + ":" + mRtspIpPort + "/" + mRtspResource;

		mHlsS3Url = "http://" + mHlsPublishS3Bucket + "."  + mHlsPublishS3Host + "/" + mHlsPublishFolder + "/" +  mHlsPublishStream;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtmpServer","",szValue,MAX_PATH - 1,configMsc.c_str());
		if(strlen(szValue)) {
			mRtmpUrl = szValue;
		} else {
			mRtmpUrl = "rtmp://localhost/live/mp4:";
			mRtmpUrl += StreamName;
		}

		GetPrivateProfileStringA(mStreamName.c_str(), "RtmpServerBackup","",szValue,MAX_PATH - 1,configMsc.c_str());
		if(strlen(szValue)) {
			mRtmpUrlBackup = szValue;
		} else {
			mRtmpUrlBackup = "";
		}

		GetPrivateProfileStringA(mStreamName.c_str(), "EnableRecordServer1","0",szValue,MAX_PATH - 1,configMsc.c_str());
		mRecordServer1 = atoi(szValue);
		GetPrivateProfileStringA(mStreamName.c_str(), "EnableRecordServer2","0",szValue,MAX_PATH - 1,configMsc.c_str());
		mRecordServer2 = atoi(szValue);


		GetPrivateProfileStringA(mStreamName.c_str(), "Bitrate","",szValue,MAX_PATH - 1,configMsc.c_str());
		mAVSrcInf.mVUserBitrate = atoi(szValue);
		if(mAVSrcInf.mVUserBitrate == 0)
			mAVSrcInf.mVUserBitrate = mAVSrcInf.mVFairBitrate;

		GetPrivateProfileStringA(mStreamName.c_str(), "AudBitrate","64000",szValue,MAX_PATH - 1,configMsc.c_str());
		mAVSrcInf.mAUserBitrate = atoi(szValue);

		GetPrivateProfileStringA(mStreamName.c_str(), "GopLength","60",szValue,MAX_PATH - 1,configMsc.c_str());
		mAVSrcInf.mUserGopLen = atoi(szValue);

		GetPrivateProfileStringA(mStreamName.c_str(), "FrameRate","15",szValue,MAX_PATH - 1,configMsc.c_str());
		mFrameRate = atoi(szValue);

		GetPrivateProfileStringA(mStreamName.c_str(), "RtpRecHost","localhost",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtpRecHost = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtpRecUserId","admin",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtpRecUserId = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtpRecPasswd","admin",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtpRecPasswd = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtpRecStream","stream",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtpRecStream = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtpRecRemotePort","5540",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtpRecRemotePort = atoi(szValue);
	}
	
	void CConfigLiveCast::Save(CAVSrcInf &mAVSrcInf)
	{
		CString configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + LIVECAST_CONFIG_FILE;
		WritePrivateProfileStringA(mStreamName.c_str(), "AudioInput",mCrntAudioDevice.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "VideoInput",mCrntVideoDevice.c_str(),configMsc.c_str());

		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvPort",mHlsSrvPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvStream",mHlsSrvStream.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvFolder",mHlsSrvFolder.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvRoot",mHlsSrvRoot.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvSegmentDuration",mHlsSrvSegmentDuration.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsSrvLiveOnly",mHlsSrvLiveOnly.c_str(),configMsc.c_str());


		WritePrivateProfileStringA(mStreamName.c_str(), "RtspPort",mRtspIpPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtspResourceName",mRtspResource.c_str(),configMsc.c_str());

		char szValue[32];
		itoa(mDestType,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "DestinationType",szValue,configMsc.c_str());

		itoa(mVidOutSize,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "VideoOutputSize",szValue,configMsc.c_str());

		itoa(mAVSrcInf.mVUserBitrate, szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "Bitrate",szValue,configMsc.c_str());

		itoa(mAVSrcInf.mAUserBitrate, szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "AudBitrate",szValue,configMsc.c_str());


		itoa(mAVSrcInf.mUserGopLen, szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "GopLength",szValue,configMsc.c_str());

		itoa(mFrameRate,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "FrameRate",szValue,configMsc.c_str());


		WritePrivateProfileStringA(mStreamName.c_str(), "RtpRecHost",mRtpRecHost.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtpRecUserId",mRtpRecUserId.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtpRecPasswd",mRtpRecPasswd.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtpRecStream",mRtpRecStream.c_str(),configMsc.c_str());

		itoa(mRtpRecRemotePort,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "RtpRecRemotePort",szValue,configMsc.c_str());

		WritePrivateProfileStringA(mStreamName.c_str(), "RtmpServer",mRtmpUrl.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtmpServerBackup",mRtmpUrlBackup.c_str(),configMsc.c_str());

		itoa(mRecordServer1,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "EnableRecordServer1",szValue,configMsc.c_str());
		itoa(mRecordServer2,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "EnableRecordServer2",szValue,configMsc.c_str());



		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishS3AccessId",mHlsPublishS3AccessId.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishS3SecKey",mHlsPublishS3SecKey.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsHost",mHlsPublishS3Host.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsBucket",mHlsPublishS3Bucket.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishStream",mHlsPublishStream.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishFolder",mHlsPublishFolder.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishSegmentDuration",mHlsPublishSegmentDuration.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPublishLiveOnly",mHlsPublishLiveOnly.c_str(),configMsc.c_str());
	}
	CConfigLiveCast::~CConfigLiveCast()
	{
		WSACleanup();
	}

	void CConfigLiveCast::GetEth0LocaIpAddr(std::string &strIpAddr)
	{
		char hostname[256];
		struct sockaddr_in	addr;

		addrinfo* results;
		addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		hint.ai_family    = PF_INET; //is_v4 ? PF_INET : PF_INET6;
		hint.ai_socktype  = SOCK_STREAM; //is_dgram ? SOCK_DGRAM : SOCK_STREAM;

		if(gethostname (hostname, sizeof (hostname)) != 0){
			//MessageBox(ghWnd, TEXT("Could not init network interface(1)!"), TEXT("Configure"), IDOK);
			return;
		}

		int res = getaddrinfo(hostname,  0, &hint, &results);
		if(res == 0) {
			memcpy(&addr, results->ai_addr, sizeof(struct sockaddr));
			freeaddrinfo(results);
			strIpAddr = inet_ntoa(addr.sin_addr);
		} else {
			//MessageBox(ghWnd, TEXT("Could not init network interface(2)!"), TEXT("Configure"), IDOK);
		}
	}
