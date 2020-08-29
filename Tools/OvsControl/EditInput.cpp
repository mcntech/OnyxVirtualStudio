// EditInput.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "EditInput.h"


// CEditInput dialog

IMPLEMENT_DYNAMIC(CEditInput, CDialog)

CEditInput::CEditInput(CWnd* pParent /*=NULL*/)
	: CDialog(CEditInput::IDD, pParent)
{

}

CEditInput::~CEditInput()
{
}

void CEditInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditInput, CDialog)
END_MESSAGE_MAP()


// CEditInput message handlers
