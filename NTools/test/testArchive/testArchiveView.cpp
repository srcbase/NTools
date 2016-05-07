// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// testArchiveView.cpp : implementation of the CtestArchiveView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "testArchive.h"
#endif

#include "testArchiveDoc.h"
#include "testArchiveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestArchiveView

IMPLEMENT_DYNCREATE(CtestArchiveView, CFormView)

BEGIN_MESSAGE_MAP(CtestArchiveView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CtestArchiveView construction/destruction

CtestArchiveView::CtestArchiveView()
	: CFormView(CtestArchiveView::IDD)
{
	// TODO: add construction code here

}

CtestArchiveView::~CtestArchiveView()
{
}

void CtestArchiveView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CtestArchiveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CtestArchiveView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

void CtestArchiveView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CtestArchiveView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CtestArchiveView diagnostics

#ifdef _DEBUG
void CtestArchiveView::AssertValid() const
{
	CFormView::AssertValid();
}

void CtestArchiveView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CtestArchiveDoc* CtestArchiveView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CtestArchiveDoc)));
	return (CtestArchiveDoc*)m_pDocument;
}
#endif //_DEBUG


// CtestArchiveView message handlers
