//****************************************************************
//
// Copyright (C) 2010, Intel Corporation. All rights reserved
// Any software source code reprinted in this document is furnished under a 
// software license and may only be used or copied in accordance with the 
// license in the Intel-EULA.rtf file installed as part of the product
// installation.
//****************************************************************

/*
This a sample class that inherits from Intel::Adp::DefaultCrashReport.
This sample illustrates the implementation of a custom Crash Report class 
that can override virtual methods in the DefaultCrashReport class. The developer
does not have to override all virtual methods. The developer can choose which 
method to override. The following custom Crash Report class facilitates 
customization of crash data.
*/

#pragma once
#include "AppFeatureConfig.h"

#ifdef EN_APPUP
#include "adpcppf.h"
class COvsControlCrashReport :
	public  Intel::Adp::DefaultCrashReport
{
public:
	COvsControlCrashReport(void);
	~COvsControlCrashReport(void);
	// Sets the name of the module that caused the crash
	void setCrashModule(wchar_t crashModule[80])
	{ wcsncpy_s(m_crashModule, crashModule, _TRUNCATE); }
	// Sets the Crash Message
	void setCrashMessage(wchar_t crashMessage[80])
	{ wcsncpy_s(m_crashMessage,crashMessage, _TRUNCATE); }
	// Sets the category of the crash. For example, "System Error"
	void setCrashCategory(wchar_t crashCategory[80])
	{ wcsncpy_s(m_crashCategory, crashCategory, _TRUNCATE); }
	// Sets Error Data 
	void setCrashErrorData(wchar_t crashErrorData[4000])
	{ wcsncpy_s(m_crashErrorData, crashErrorData, _TRUNCATE); }
	// Sets the line number in the code to indicate the source of the crash
	void setCrashLineNumber(long lineNumber)
	{ m_crashLineNumber = lineNumber; }

private:
	// 
	wchar_t m_crashModule[80];
	wchar_t m_crashMessage[80];
	wchar_t m_crashCategory[80];
	wchar_t m_crashErrorData[4000];
	long    m_crashLineNumber;

	// override virtual PopulateModuleName method in Intel::Adp::DefaultCrashReport class
	void PopulateModuleName();
	// override virtual PopulateMessage method in Intel::Adp::DefaultCrashReport class
	// Provides an ability to include a error message in the crash report data
	void PopulateMessage();
	// override virtual PopulateErrorData method in Intel::Adp::DefaultCrashReport class
	// Provides an ability to add error data to the crash report
	void PopulateErrorData();
	// override virtual PopulateCategory method in Intel::Adp::DefaultCrashReport class
	void PopulateCategory();
	// override virtual PopulateLineNumber method in Intel::Adp::DefaultCrashReport class
	void PopulateLineNumber();
	// override virtual PopulateCrashReportFields method in Intel::Adp::DefaultCrashReport class
	void PopulateCrashReportFields();
};
#endif