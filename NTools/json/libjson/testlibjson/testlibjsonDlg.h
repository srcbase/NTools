
// testlibjsonDlg.h : header file
//

#pragma once
#include "explorer2.h"


// CtestlibjsonDlg dialog
class CtestlibjsonDlg : public CDialogEx
{
// Construction
public:
	CtestlibjsonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTLIBJSON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonParser();
	CExplorer2 m_web;
	afx_msg void OnBnClickedOk();
};
