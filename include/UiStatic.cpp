﻿#include "stdafx.h"
#include "UiStatic.h"
#include "UiMessage.h"

	
UI_STATIC::UI_STATIC()
{
	pParent    = NULL;
	uId        = 0;
	rect       = CRect(0, 0, 0, 0);
	uBitmap    = 0;
	clrMask    = CLR_NONE;
	sCaption   = _T("");
	dwStyle    = 0;
}
UI_STATIC::UI_STATIC(CWnd* parent, UINT id, CRect rcWin, CString caption, UINT bmp, COLORREF mask, DWORD style)
{
	pParent    = parent;
	uId        = id;
	rect       = rcWin;
	sCaption   = caption;
	uBitmap    = bmp;
	clrMask    = mask;
	dwStyle    = style;
}
UI_STATIC& UI_STATIC::operator = (const UI_STATIC* pStatic)
{
	pParent    = pStatic->pParent;
	uId        = pStatic->uId;
	rect       = pStatic->rect;
	sCaption   = pStatic->sCaption;
	uBitmap    = pStatic->uBitmap;
	clrMask    = pStatic->clrMask;
	dwStyle    = pStatic->dwStyle;

	return *this;
}

CUiStatic::CUiStatic()
{
	m_uNormal    = 0;
	m_dwFlags    = DT_VCENTER | DT_CENTER | DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK;
	m_clrMask    = CLR_NONE;
	m_clrText    = CLR_NONE;
	m_clrBk		 = CLR_NONE;
	m_pFont      = NULL;
	m_sCaption   = _T("");
}

CUiStatic::~CUiStatic()
{
}

BEGIN_MESSAGE_MAP(CUiStatic, CUiBase)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


CToolTipCtrl* CUiStatic::GetToolTipCtrl()
{
	return &m_toolTip;
}
//-------------------------------------------------------------------------------
void CUiStatic::CreateStatic(UI_STATIC uiStatic)
{
	Create(uiStatic.pParent, uiStatic.uId, uiStatic.rect, uiStatic.dwStyle); 
	if(uiStatic.uBitmap != 0)
	{
		SetStaticBitmap(uiStatic.uBitmap, uiStatic.clrMask); 
	}
	SetCaption(uiStatic.sCaption);
}
//-------------------------------------------------------------------------------
void CUiStatic::SetStaticBitmap(UINT nNormal, COLORREF clrMask)
{
	m_uNormal = nNormal;
	m_clrMask = clrMask;
	if(m_uNormal == 0) return;

	BITMAP  bmp;
	CBitmap bitmap;
	bitmap.LoadBitmap(m_uNormal);
	bitmap.GetBitmap(&bmp);

	SetWindowPos(NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	bitmap.DeleteObject();

	Invalidate(FALSE);
}
//-------------------------------------------------------------------------------
void CUiStatic::SetCaption(CString sCaption)
{
	m_sCaption = sCaption;
	Invalidate(FALSE);
}
//-------------------------------------------------------------------------------
CString CUiStatic::GetCaption()
{
	return m_sCaption;
}
//-------------------------------------------------------------------------------
void CUiStatic::SetBkColor(COLORREF clrBk) 
{
	if(clrBk == m_clrMask) clrBk++;
	m_clrBk = clrBk;
	Invalidate(FALSE);
}
//-------------------------------------------------------------------------------
DWORD CUiStatic::GetCtrlStyle()
{
	return CUiBase::GetCtrlStyle();
}
//-------------------------------------------------------------------------------
void CUiStatic::ModifyCtrlStyle(DWORD dwRemove, DWORD dwAdd) 
{
	CUiBase::ModifyCtrlStyle(dwRemove, dwAdd);

	DWORD dwFlags = GetCtrlStyle();

	m_dwFlags = DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK;
	if(dwFlags & ST_TOP)        { m_dwFlags = m_dwFlags | DT_TOP; }
	if(dwFlags & ST_LEFT)       { m_dwFlags = m_dwFlags | DT_LEFT; }
	if(dwFlags & ST_CENTER)     { m_dwFlags = m_dwFlags | DT_CENTER; }
	if(dwFlags & ST_RIGHT)      { m_dwFlags = m_dwFlags | DT_RIGHT; }
	if(dwFlags & ST_VCENTER)    { m_dwFlags = m_dwFlags | DT_VCENTER; }
	if(dwFlags & ST_BOTTOM)     { m_dwFlags = m_dwFlags | DT_BOTTOM; }
	if(dwFlags & ST_SINGLELINE) { m_dwFlags = m_dwFlags | DT_SINGLELINE; }

	Invalidate(FALSE);
}
//-------------------------------------------------------------------------------
void CUiStatic::SetFont(CFont* pFont, COLORREF clrFont)
{
	m_pFont   = pFont;
	m_clrText = clrFont;
}
//-------------------------------------------------------------------------------
void CUiStatic::SetToolTip(CString sTip, int nMaxTipWidth)
{
	if(NULL != GetSafeHwnd() && !sTip.IsEmpty()) 
	{
		if(NULL == m_toolTip.GetSafeHwnd()) 
		{
			m_toolTip.Create(this, TTS_ALWAYSTIP);
			m_toolTip.Activate(TRUE);
			m_toolTip.AddTool(this, sTip);
			if(nMaxTipWidth > 0)
			{
				m_toolTip.SetMaxTipWidth(nMaxTipWidth);
			}
		}
		else
		{
			m_toolTip.UpdateTipText(sTip, this);	
			if(nMaxTipWidth > 0)
			{
				m_toolTip.SetMaxTipWidth(nMaxTipWidth);
			}
		}
	}
}
//-------------------------------------------------------------------------------
BOOL CUiStatic::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE)
	{
		if (m_toolTip.GetSafeHwnd()) { m_toolTip.RelayEvent(pMsg); }
	}

	return CUiBase::PreTranslateMessage(pMsg);
}
//-------------------------------------------------------------------------------
LRESULT CUiStatic::OnNcHitTest(CPoint point)
{
	return HTTRANSPARENT;
}
//-------------------------------------------------------------------------------
void CUiStatic::UpdateBkBitmap()
{
	CPaintDC dc(this);
	CRect rcCli;
	GetClientRect(&rcCli);

	DrawStatic(&dc, rcCli);
}
//-------------------------------------------------------------------------------
void CUiStatic::Draw(CDC* pDC, CRect rcCli)
{
	DrawStatic(pDC, rcCli);
}
//-------------------------------------------------------------------------------
void CUiStatic::DrawStatic(CDC* pDC, CRect rcCli)
{
	if (m_clrBk != CLR_NONE)
	{
		pDC->FillSolidRect(rcCli, m_clrBk);
	}

	if(m_uNormal != 0)
	{
		DrawBitmap(pDC, rcCli, m_uNormal, m_clrMask);
	}

	if(_T("") != m_sCaption)
	{
		DrawText(pDC, rcCli, m_sCaption, m_dwFlags, m_clrText, m_pFont);
	}
}
//-------------------------------------------------------------------------------
