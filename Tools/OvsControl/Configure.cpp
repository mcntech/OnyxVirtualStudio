#include "stdafx.h"
#include <string>
#include "Configure.h"
#include "DshowCapGraph.h"

CConfigure::CConfigure(const char *StreamName)
	{
	   char szPath[128] = "";
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		mStreamName = StreamName;
		std::string configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + "\\olc.cfg";
		char szValue[MAX_PATH];
		// TODO
		mDestType = DEST_TYPE_RTPMP;
		
		GetPrivateProfileStringA(mStreamName.c_str(), "AudioInput",CSTR_NO_AUDIO,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntAudioDevice = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "VideoInput",CSTR_MCN_VCAP,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntVideoDevice = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsPort","8080",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsIpPort = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "HlsResourceName","stream.m3u8",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsResource = szValue;


		GetPrivateProfileStringA(mStreamName.c_str(), "RtspPort","5540",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspIpPort = szValue;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtspResourceName",StreamName,szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspResource = szValue;


		std::string strIpAddr;
		GetEth0LocaIpAddr(strIpAddr);
		mRtspIpAddr = strIpAddr;
		mHlsIpAddr = strIpAddr;
		
		mHlsUrl = "http://" + mHlsIpAddr + ":" + mHlsIpPort + "/" + mHlsResource;
		
		mHRtspUrl = "rtsp://" + mRtspIpAddr + ":" + mRtspIpPort + "/" + mRtspResource;

		GetPrivateProfileStringA(mStreamName.c_str(), "RtmpServer","",szValue,MAX_PATH - 1,configMsc.c_str());
		if(strlen(szValue)) {
			mRtmpUrl = szValue;
		} else {
			mRtmpUrl = "rtmp://localhost/flvplayback/mp4:";
			mRtmpUrl += StreamName;
		}

		GetPrivateProfileStringA(mStreamName.c_str(), "RtmpServerBackup","",szValue,MAX_PATH - 1,configMsc.c_str());
		if(strlen(szValue)) {
			mRtmpUrlBackup = szValue;
		} else {
			mRtmpUrlBackup = "";
		}


		GetPrivateProfileStringA(mStreamName.c_str(), "Bitrate","4000000",szValue,MAX_PATH - 1,configMsc.c_str());
		m_VidBitrate = atoi(szValue);

		GetPrivateProfileStringA(mStreamName.c_str(), "AudBitrate","64000",szValue,MAX_PATH - 1,configMsc.c_str());
		m_AudBitrate = atoi(szValue);

		GetPrivateProfileStringA(mStreamName.c_str(), "GopLength","15",szValue,MAX_PATH - 1,configMsc.c_str());
		mGopLength = atoi(szValue);

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
	
	void CConfigure::Save()
	{
		std::string configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + "\\olc.cfg";
		WritePrivateProfileStringA(mStreamName.c_str(), "AudioInput",mCrntAudioDevice.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "VideoInput",mCrntVideoDevice.c_str(),configMsc.c_str());

		WritePrivateProfileStringA(mStreamName.c_str(), "HlsPort",mHlsIpPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "HlsResourceName",mHlsResource.c_str(),configMsc.c_str());

		WritePrivateProfileStringA(mStreamName.c_str(), "RtspPort",mRtspIpPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(mStreamName.c_str(), "RtspResourceName",mRtspResource.c_str(),configMsc.c_str());

		char szValue[32];
		itoa(mDestType,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "DestinationType",szValue,configMsc.c_str());

		itoa(mVidOutSize,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "VideoOutputSize",szValue,configMsc.c_str());

		itoa(m_VidBitrate,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "Bitrate",szValue,configMsc.c_str());

		itoa(m_AudBitrate,szValue, 10);
		WritePrivateProfileStringA(mStreamName.c_str(), "AudBitrate",szValue,configMsc.c_str());


		itoa(mGopLength,szValue, 10);
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
	}
	CConfigure::~CConfigure()
	{
		WSACleanup();
	}
	void CConfigure::GetConfigFoderPath(std::string &cfgFolderPath)
	{
		PWSTR romingFolder = NULL; 
		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &romingFolder);
		std::string path = convertWStringToCString(romingFolder);
		CoTaskMemFree(romingFolder);
		cfgFolderPath = path + "\\MCN\\OnyxVirtualStudio";
	}

	void CConfigure::GetEth0LocaIpAddr(std::string &strIpAddr)
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
