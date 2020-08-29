// OvsConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OvsConfig.h"
#include "OvsConfigDlg.h"
#include <Shlobj.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GetConfigFoderPath(CString &cfgFolderPath)
{
	PWSTR romingFolder = NULL; 
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &romingFolder);
	cfgFolderPath = romingFolder;
	cfgFolderPath += "\\MCN\\OnyxVirtualStudio";
	CoTaskMemFree(romingFolder);
}


// COvsConfigApp

BEGIN_MESSAGE_MAP(COvsConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COvsConfigApp construction

COvsConfigApp::COvsConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COvsConfigApp object

COvsConfigApp theApp;

BOOL COvsConfigApp::InitStreamInf(
			CString &ConfigFile,
			const char *SectionName,
			CString &StreamPath, 
			CString &StreamEnity, 
			CString &StreamMaterial, 
			CString &Type, 
			BOOL &Enable, 
			BOOL &UpsideDown,
			BOOL &ColorKey)
{
	char szValue[MAX_PATH] = {0};
	GetPrivateProfileStringA(SectionName, "Video","",szValue,MAX_PATH - 1,ConfigFile);
	StreamPath = szValue;
	GetPrivateProfileStringA(SectionName, "Entity","",szValue,MAX_PATH - 1,ConfigFile);
	StreamEnity = szValue;
	GetPrivateProfileStringA(SectionName, "Material","",szValue,MAX_PATH - 1,ConfigFile);
	StreamMaterial = szValue;

	GetPrivateProfileStringA(SectionName, "Enable","",szValue,MAX_PATH - 1,ConfigFile);
	Enable = atoi(szValue);
	GetPrivateProfileStringA(SectionName, "UpsideDown","",szValue,MAX_PATH - 1,ConfigFile);
	UpsideDown = atoi(szValue);

	GetPrivateProfileStringA(SectionName, "ColorKey","",szValue,MAX_PATH - 1,ConfigFile);
	ColorKey = atoi(szValue);

	return TRUE;
}


BOOL COvsConfigApp::SaveStreamInf(
			CString    &ConfigFile,
			const char *SectionName,
			CString    &StreamPath, 
			CString    &StreamEnity, 
			CString    &StreamMaterial, 
			CString    &Type, 
			BOOL       Enable, 
			BOOL       UpsideDown,
			BOOL       ColorKey)
{
	char szValue[MAX_PATH] = {0};

	if(!StreamPath.IsEmpty()){
		WritePrivateProfileStringA(SectionName, "Video",StreamPath, ConfigFile);
	} else {
		// Override enable
		Enable = 0;
	}
	itoa(Enable, szValue, 10);

	WritePrivateProfileStringA(SectionName, "Enable",szValue, ConfigFile);

	WritePrivateProfileStringA(SectionName, "Entity",StreamEnity, ConfigFile);
	WritePrivateProfileStringA(SectionName, "Material",StreamMaterial, ConfigFile);

	itoa(UpsideDown, szValue, 10);
	WritePrivateProfileStringA(SectionName, "UpsideDown",szValue, ConfigFile);

	itoa(ColorKey, szValue, 10);
	WritePrivateProfileStringA(SectionName, "ColorKey",szValue, ConfigFile);

	return TRUE;
}

BOOL COvsConfigApp::InitInstance()
{
	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	COvsConfigDlg dlg;
	m_pMainWnd = &dlg;
	
	GetConfigFoderPath(dlg.m_ConfigFile);

	dlg.m_ConfigFile += "\\ovs.cfg";
	char szValue[MAX_PATH] = {0};
	char szSection[MAX_PATH] = {0};
	CString streamType;
	BOOL bUpsideDown = 0;
	BOOL bColorKey = 0;
	CString material_none = "None";
	CString entity_none = "None";

	InitStreamInf(dlg.m_ConfigFile, "Stream1",dlg.m_Stream1, dlg.m_EntityStream1, dlg.m_MaterialStream1, streamType, dlg.m_ChkStream1, dlg.m_ChkFlipVStream1, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Stream2",dlg.m_Stream2, dlg.m_EntityStream2, dlg.m_MaterialStream2, streamType, dlg.m_ChkStream2, dlg.m_ChkFlipVStream2, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Stream3",dlg.m_Stream3, dlg.m_EntityStream3, dlg.m_MaterialStream3, streamType, dlg.m_ChkStream3, dlg.m_ChkFlipVStream3, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Stream4",dlg.m_Stream4, dlg.m_EntityStream4, dlg.m_MaterialStream4, streamType, dlg.m_ChkStream4, dlg.m_ChkFlipVStream4, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Stream5",dlg.m_Stream5, dlg.m_EntityStream5, dlg.m_MaterialStream5, streamType, dlg.m_ChkStream5, dlg.m_ChkFlipVStream5, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Stream6",dlg.m_Stream6, dlg.m_EntityStream6, dlg.m_MaterialStream6, streamType, dlg.m_ChkStream6, dlg.m_ChkFlipVStream6, bColorKey);

	InitStreamInf(dlg.m_ConfigFile, "Anchor1",dlg.m_Anchor1, dlg.m_EntityAnchor1, dlg.m_MaterialAnchor1, streamType, dlg.m_ChkAnchor1, dlg.m_ChkFlipVAnchor1, dlg.m_ChkAnchor1ColorKey);

	InitStreamInf(dlg.m_ConfigFile, "Advertisement1",dlg.m_Advt1, dlg.m_EntityAdv1,dlg.m_MaterialAdv1, streamType, dlg.m_ChkAdvt1, dlg.m_ChkFlipVAdvt1, bColorKey);
	InitStreamInf(dlg.m_ConfigFile, "Advertisement2",dlg.m_Advt2, dlg.m_EntityAdv2, dlg.m_MaterialAdv2, streamType, dlg.m_ChkAdvt2, dlg.m_ChkFlipVAdvt2, bColorKey);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)	{
		SaveStreamInf(dlg.m_ConfigFile, "Stream1",dlg.m_Stream1, dlg.m_EntityStream1, dlg.m_MaterialStream1, streamType, dlg.m_ChkStream1, dlg.m_ChkFlipVStream1);
		SaveStreamInf(dlg.m_ConfigFile, "Stream2",dlg.m_Stream2, dlg.m_EntityStream2, dlg.m_MaterialStream2, streamType, dlg.m_ChkStream2, dlg.m_ChkFlipVStream2);
		SaveStreamInf(dlg.m_ConfigFile, "Stream3",dlg.m_Stream3, dlg.m_EntityStream3, dlg.m_MaterialStream3, streamType, dlg.m_ChkStream3, dlg.m_ChkFlipVStream3);
		SaveStreamInf(dlg.m_ConfigFile, "Stream4",dlg.m_Stream4, dlg.m_EntityStream4, dlg.m_MaterialStream4, streamType, dlg.m_ChkStream4, dlg.m_ChkFlipVStream4);
		SaveStreamInf(dlg.m_ConfigFile, "Stream5",dlg.m_Stream5, dlg.m_EntityStream5, dlg.m_MaterialStream5, streamType, dlg.m_ChkStream5, dlg.m_ChkFlipVStream5);
		SaveStreamInf(dlg.m_ConfigFile, "Stream6",dlg.m_Stream6, dlg.m_EntityStream6, dlg.m_MaterialStream6, streamType, dlg.m_ChkStream6, dlg.m_ChkFlipVStream6);

		SaveStreamInf(dlg.m_ConfigFile, "Anchor1",dlg.m_Anchor1, dlg.m_EntityAnchor1, dlg.m_MaterialAnchor1, streamType, dlg.m_ChkAnchor1, dlg.m_ChkFlipVAnchor1, dlg.m_ChkAnchor1ColorKey);

		SaveStreamInf(dlg.m_ConfigFile, "Advertisement1",dlg.m_Advt1,  dlg.m_EntityAdv1,dlg.m_MaterialAdv1, streamType, dlg.m_ChkAdvt1, dlg.m_ChkFlipVAdvt1);
		SaveStreamInf(dlg.m_ConfigFile, "Advertisement2",dlg.m_Advt2,  dlg.m_EntityAdv2, dlg.m_MaterialAdv2, streamType, dlg.m_ChkAdvt2, dlg.m_ChkFlipVAdvt2);
	} else if (nResponse == IDCANCEL) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
