// SkypeConfig.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "SkypeConfig.h"
#include <string>

// CSkypeConfig dialog

IMPLEMENT_DYNAMIC(CSkypeConfig, CDialog)

CSkypeConfig::CSkypeConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CSkypeConfig::IDD, pParent)
	, m_AudioInput(_T(""))
	, m_AudioOutput(_T(""))
{

}

CSkypeConfig::~CSkypeConfig()
{
}

void CSkypeConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_SKYPE_AUDIO_INPUT, m_AudioInput);
	DDX_CBString(pDX, IDC_COMBO_SKYPE_AUDIO_OUTPUT, m_AudioOutput);
	DDX_Control(pDX, IDC_COMBO_SKYPE_AUDIO_INPUT, m_ComboAudInput);
	DDX_Control(pDX, IDC_COMBO_SKYPE_AUDIO_OUTPUT, m_ComboAudOutput);
}


BEGIN_MESSAGE_MAP(CSkypeConfig, CDialog)
END_MESSAGE_MAP()


// CSkypeConfig message handlers

BOOL CSkypeConfig::OnInitDialog()
{
	CDialog::OnInitDialog();
	char szBuff[1024] = {0};

	m_pSkypeIf->GetAudioInputDeviceList(szBuff, 1024);
	char *pNameList = szBuff;
	char *pPosNewLine = NULL;
	do{
		pPosNewLine = strchr(pNameList,'\n');
		if(pPosNewLine) {
			*pPosNewLine  = 0;	// replace new line with null termination
			m_ComboAudInput.AddString(pNameList);
			pNameList = pPosNewLine + 1;
		}
	} while(pPosNewLine);
	
	if(m_AudioInput.GetLength() == 0 && m_ComboAudInput.GetCount() > 0){
		m_ComboAudInput.SetCurSel(0);
	}
	memset(szBuff, 0x00, 1024);

	m_pSkypeIf->GetAudioOutputDeviceList(szBuff, 1024);
	pNameList = szBuff;
	pPosNewLine = NULL;
	do{
		pPosNewLine = strchr(pNameList,'\n');
		if(pPosNewLine) {
			*pPosNewLine  = 0;	// replace new line with null termination
			m_ComboAudOutput.AddString(pNameList);
			pNameList = pPosNewLine + 1;
		}
	} while(pPosNewLine);
	if(m_AudioOutput.GetLength() == 0 && m_ComboAudOutput.GetCount() > 0){
		m_ComboAudOutput.SetCurSel(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
