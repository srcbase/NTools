
// sqliteTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sqliteTest.h"
#include "sqliteTestDlg.h"
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


// CsqliteTestDlg dialog




CsqliteTestDlg::CsqliteTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CsqliteTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsqliteTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsqliteTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CsqliteTestDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CsqliteTestDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_INSERTINTO, &CsqliteTestDlg::OnBnClickedButtonInsertinto)
END_MESSAGE_MAP()


// CsqliteTestDlg message handlers

BOOL CsqliteTestDlg::OnInitDialog()
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

void CsqliteTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CsqliteTestDlg::OnPaint()
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
HCURSOR CsqliteTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsqliteTestDlg::OnBnClickedButtonOpen()
{
	if (!m_db.open(L"eriger.s3db"))
	{
		gtc::app::message(L"´ò¿ªÊý¾Ý¿âÊ§°Ü£¡");
	}
}

void CsqliteTestDlg::OnBnClickedButtonClose()
{
	if (!m_db.close())
	{
		gtc::app::messageEx(L"¹Ø±ÕÊý¾Ý¿âÊ§°Ü£¡%s", m_db.getLastErrorInfo().c_str());
	}
}

void CsqliteTestDlg::OnBnClickedButtonInsertinto()
{
	int flag = 8;
	gtc::variant var = m_db.executeScalar(L"SELECT MAX(rowid)+1 FROM tbZSysParams");
	if(var.dataType == BEDataTypeInt32)
		flag = var.lVal;
	SBDataType param4;
	param4.push_back(1);
	param4.push_back(2);
	param4.push_back(0);
	param4.push_back(3);
	param4.push_back(4);
	SWDataType param5;
	param5.push_back(L'a');
	param5.push_back(L'b');
	param5.push_back(L'\0');
	param5.push_back(L'c');
	param5.push_back(L'd');
	m_db.addParameter(L"FFlag", flag);
	m_db.addParameter(L"FParam1", flag);
	m_db.addParameter(L"FParam4", param4);
	m_db.addParameter(L"FParam5", param4);
	wchar_t pbuf[MAX_PATH] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"param %d", flag++);
	m_db.addParameter(L"FDesc", pbuf);
	int ret = m_db.executeNonQuery(L"INSERT INTO tbZSysParams(FFlag,FParam,FParam1,FParam4,FParam5) VALUES(@FFlag,@FDesc,@FParam1,@FParam4,@FParam5)");
	if (ret == 0)
	{
		gtc::app::messageEx(L"Ê§°Ü£¡%s", m_db.getLastErrorInfo().c_str());
	}	
	//bool bl = 
	//COleVariant var;
	//VARIANT v;
	//COleDateTime dt;
	//wchar_t pbuf[260] = {0};
	//std::swprintf(pbuf, _countof(pbuf), L"%d", 123);
	//SWDataType vectxt;
	//vectxt.assign(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
	//int year= 0x01020304;//dt.GetYear();
	//SBDataType data((uint8_t*)&year, (uint8_t*)&year+sizeof(int));
	//int dd = 0;
	//memcpy(&dd, data.data(), sizeof(int));
	//std::time_t tt = std::time(NULL);
	//dt = tt;
	//var = _T("123");
	//var.ChangeType(VT_I1|VT_ARRAY);
	//int ret = m_db.executeNonQuery(L"INSERT INTO tbZParameter(FFlag,FDesc,FParam1) VALUES(4, 'hguuu', 2);");
	//if (ret == 0)
	//{
	//	gtc::app::message(L"Ê§°Ü£¡");
	//}
}
