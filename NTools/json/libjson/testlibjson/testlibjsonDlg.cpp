
// testlibjsonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testlibjson.h"
#include "testlibjsonDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtestlibjsonDlg dialog




CtestlibjsonDlg::CtestlibjsonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestlibjsonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestlibjsonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER2, m_web);
}

BEGIN_MESSAGE_MAP(CtestlibjsonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CtestlibjsonDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_PARSER, &CtestlibjsonDlg::OnBnClickedButtonParser)
	ON_BN_CLICKED(IDOK, &CtestlibjsonDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CtestlibjsonDlg message handlers

BOOL CtestlibjsonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestlibjsonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestlibjsonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestlibjsonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestlibjsonDlg::OnBnClickedButtonWrite()
{
	JSONNode n(JSON_NODE);
	n.push_back(JSONNode(L"String Node", L"String Value"));
	n.push_back(JSONNode(L"Integer Node", 42));
	n.push_back(JSONNode(L"Floating Point Node", 3.14));
	n.push_back(JSONNode(L"Boolean Node", true));
	std::wstring jc = n.write_formatted();
	gtc::app::message(jc.c_str());
}


void CtestlibjsonDlg::OnBnClickedButtonParser()
{
	JSONNode n = libjson::parse(L"{\"RootA\":\"Value in parent node\",\"ChildNode\":{\"ChildA\":\"String Value\",\"ChildB\":42}}");
	std::wstring wsName;
	std::wstring wsValue;
	for (JSONNode::const_iterator iter = n.begin(); iter != n.end(); ++iter)
	{
		if (iter->type()== JSON_ARRAY || iter->type() == JSON_NODE)
		{
			for (JSONNode::const_iterator it = (*iter).begin(); it != (*iter).end(); ++it)
			{
				wsName = it->name();
				wsValue = it->as_string();
			}
		}
		wsName = iter->name();
		wsValue = iter->write_formatted();

	}
}


void CtestlibjsonDlg::OnBnClickedOk()
{
	std::wstring wstr;
	UpdateData();
	CString str;
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	CDialogEx::OnOK();
}
