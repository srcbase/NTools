
// sqliteTestDlg.h : header file
//

#pragma once


// CsqliteTestDlg dialog
class CsqliteTestDlg : public CDialogEx
{
// Construction
public:
	CsqliteTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SQLITETEST_DIALOG };

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
	afx_msg void OnBnClickedButtonOpen();

protected:
	db::sqlite::sqliteDB m_db;
public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonInsertinto();
};
