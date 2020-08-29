//****************************************************************
//
// Copyright (C) 2010, Intel Corporation. All rights reserved
// Any software source code reprinted in this document is furnished under a 
// software license and may only be used or copied in accordance with the 
// license in the Intel-EULA.rtf file installed as part of the product
// installation.
//****************************************************************



#include "StdAfx.h"
#include "AppFeatureConfig.h"
#ifdef EN_APPUP
#include "OvsControlCrashReport.h"

COvsControlCrashReport::COvsControlCrashReport(void)
{
	wcscpy_s(m_crashModule, L"");
	wcscpy_s(m_crashMessage, L"");
	wcscpy_s(m_crashCategory, L"");
	wcscpy_s(m_crashErrorData, L"");
	m_crashLineNumber = 0;
}

COvsControlCrashReport::~COvsControlCrashReport(void)
{

}


void COvsControlCrashReport::PopulateModuleName()
{
	wcscpy_s(module, m_crashModule);
}

//
// Provide custom crash message
// 
void COvsControlCrashReport::PopulateMessage()
{
	wcscpy_s(message, m_crashMessage);
}

//
// Provide upto 4000 bytes of additional crash data, such as call-stack
// 
void COvsControlCrashReport::PopulateErrorData()
{
	wcscpy_s(errorData, m_crashErrorData);
}

//
// Provide custom crash category to bin / filter crash reports
// 
void COvsControlCrashReport::PopulateCategory()
{
	wcscpy_s(category, m_crashCategory);
}

//
// Provide line number of the module where the crash occured
// 
void COvsControlCrashReport::PopulateLineNumber()
{
	lineNumber = m_crashLineNumber;
}

//
// Provide additional crash report information into user-defined crash report fields.
// Developer can provide upto 10 user specific name value pairs
//
void COvsControlCrashReport::PopulateCrashReportFields() 
{
	pCrashReportFields = new ADP_CrashReportField[2];
	pCrashReportFields[0].name = L"Sub Category";
	pCrashReportFields[0].value = L"Trapped Error";
	pCrashReportFields[1].name = L"Application Mode";
	pCrashReportFields[1].value = L"Initialization";
	crashReportFieldCount = 2;
}

#endif