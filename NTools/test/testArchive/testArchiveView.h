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

// testArchiveView.h : interface of the CtestArchiveView class
//

#pragma once

#include "resource.h"


class CtestArchiveView : public CFormView
{
protected: // create from serialization only
	CtestArchiveView();
	DECLARE_DYNCREATE(CtestArchiveView)

public:
	enum{ IDD = IDD_TESTARCHIVE_FORM };

// Attributes
public:
	CtestArchiveDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CtestArchiveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in testArchiveView.cpp
inline CtestArchiveDoc* CtestArchiveView::GetDocument() const
   { return reinterpret_cast<CtestArchiveDoc*>(m_pDocument); }
#endif

