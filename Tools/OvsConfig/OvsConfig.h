// OvsConfig.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COvsConfigApp:
// See OvsConfig.cpp for the implementation of this class
//

class COvsConfigApp : public CWinApp
{
public:
	COvsConfigApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	BOOL InitStreamInf(
			CString &ConfigFile,
			const char *SectionName,
			CString &StreamPath, 
			CString &StreamEnity, 
			CString &StreamMaterial, 
			CString &Type, 
			BOOL &Enable, 
			BOOL &UpsideDown,
			BOOL &ColorKey);


	BOOL SaveStreamInf(
			CString    &ConfigFile,
			const char *SectionName,
			CString    &StreamPath, 
			CString    &StreamEnity, 
			CString    &StreamMaterial, 
			CString    &Type, 
			BOOL       Enable, 
			BOOL       UpsideDown,
			BOOL       ColorKey = FALSE);

	BOOL InitOutputInf(
			CString     &ConfigFile,
			const char  *SectionName,
			CString     &StreamPath,
			CString     &AudioPath, 
			BOOL        &Enable,
			CString     &Bitrate,
			CString     &GopLength

			);

	BOOL SaveOutputInf(
			CString    &ConfigFile,
			const char *SectionName,
			CString    &StreamPath, 
			CString    &AudioPath, 
			BOOL       Enable,
			CString    &Bitrate,
			CString    &GopLength

			);

	DECLARE_MESSAGE_MAP()
};

extern COvsConfigApp theApp;