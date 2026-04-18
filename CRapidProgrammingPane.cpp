﻿// CRapidProgrammingPane.cpp : implementation file
//

#include "stdafx.h"
#include "CVImageEditor.h"
#include "Part.h"
#include "CRapidProgrammingPane.h"
#include "PartPropertiesPane.h"
#include "Inspector.h"
#include "RapidProgram/PartTypePaneDlg.h"

IMPLEMENT_DYNAMIC(CRapidProgrammingPane, CWnd)

CRapidProgrammingPane::CRapidProgrammingPane()
{
	m_partTypePane = NULL;
}

CRapidProgrammingPane::~CRapidProgrammingPane()
{

}

void CRapidProgrammingPane::SetPartData(CPart * pPart)
{
	m_pSelectPart = pPart;
	m_pSelectPart->m_vRapidInspects.clear();
}

void CRapidProgrammingPane::SetInspectorList()
{
}

BEGIN_MESSAGE_MAP(CRapidProgrammingPane, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



int CRapidProgrammingPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	BOOL b = m_wndOutlookBar.Create(_T("Shortcuts"),
		this,
		CRect(0,
			0,
			100,
			100),
		IDS_RAPID_PROGRAMMING_BAR,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT);

	m_pOutlookBarTabCtrl = (CMFCOutlookBarTabCtrl*)m_wndOutlookBar.GetUnderlyingWindow();

	m_partTypePane = new RapidPaneTemplate<CPartTypePaneDlg>(IDD_PARTTYPEPANEDLG, this,0);
	b = m_partTypePane->Create(m_pOutlookBarTabCtrl,
		AFX_DEFAULT_TOOLBAR_STYLE/*| WS_CAPTION*/,
		IDR_RAPID_PARTTYPE_PANE,
		AFX_CBRS_FLOAT | AFX_CBRS_RESIZE);
	m_partTypePane->EnableDocking(CBRS_ALIGN_TOP);	
	m_pOutlookBarTabCtrl->AddTab(m_partTypePane, L"PartType", (UINT)-1, FALSE);
	return 0;
}

void CRapidProgrammingPane::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_wndOutlookBar.GetSafeHwnd())
	{
		m_wndOutlookBar.MoveWindow(CRect(0, 0, cx, cy));
		m_wndOutlookBar.Invalidate(FALSE);
	}
}

void CRapidProgrammingPane::DeleteRapidProgrammingControls()
{
	if (m_pOutlookBarTabCtrl)
		m_pOutlookBarTabCtrl->RemoveAllTabs();


	if (m_partTypePane != NULL)
	{
		m_partTypePane->ReleaseWindow();
		delete m_partTypePane;
		m_partTypePane = NULL;
	}
	m_pPanes.clear();

	if (m_pSelectPart)
	{
		m_pSelectPart->m_bShowRapidInspectRects = false;
		for (int i = 0; i < m_pSelectPart->m_vRapidInspects.size();i++)
		{
			delete m_pSelectPart->m_vRapidInspects[i];
			m_pSelectPart->m_vRapidInspects[i] = NULL;
		}
		m_pSelectPart->m_vRapidInspects.clear();
		if (m_pSelectPart->m_cvobjRapidBodySizeWindows != NULL)
		{
			m_pSelectPart->m_cvobjRapidBodySizeWindows = NULL;
		}
		if (m_pSelectPart->m_cvobjRapidPadSizeWindows != NULL)
		{
			m_pSelectPart->m_cvobjRapidPadSizeWindows = NULL;
		}
		if (m_pSelectPart->m_cvobjRapidPinSizeWindows != NULL)
		{
			m_pSelectPart->m_cvobjRapidPinSizeWindows = NULL;
		}
	}

	m_pSelectPart = NULL;
	m_pWndProperties = NULL;

	if (m_wndOutlookBar.GetSafeHwnd())
	{
		m_wndOutlookBar.DestroyWindow();
	}

	//中国人名
}

void CRapidProgrammingPane::UpdataPartProperties()
{
	if(m_pWndProperties)
	{
		m_pWndProperties->SetPartData(m_pSelectPart);
		m_pWndProperties->UpdateInspectorList();
	}
}