#pragma once
namespace wui {

#ifdef _DEBUG
#ifndef TRACE
#define TRACE __trace
#endif
#define TRACEMSG __traceMsg
#define TRACE_CLASS_BEGIN(cls)	__traceA("--->　new \t\t%s", typeid((cls)).name());
#define TRACE_CLASS_END(cls)	__traceA("--->　delete \t%s", typeid((cls)).name());
#else
#ifndef TRACE
#define TRACE
#endif
#define TRACEMSG L""
#define TRACE_CLASS_BEGIN(cls)
#define TRACE_CLASS_END(cls)
#endif

void WINUI_API __trace(PCWSTR pstrFormat, ...);
void WINUI_API __traceA(PCSTR pstrFormat, ...);

PCWSTR WINUI_API __traceMsg(UINT uMsg);
/////////////////////////////////////////////////////////////////////////////////////
//

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
/////////////////////////////////////////////////////////////////////////////////////
//

class stringOrID
{
public:
	stringOrID(PCWSTR lpString) : m_lpstr(lpString)
	{ }
	stringOrID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
	{ }
	PCWSTR m_lpstr;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- point	 2014-05-21 --- >>
//	自定义点
////////////////////////////////////////////////////////////////////////////////////////////////////////
class WINUI_API point : public tagPOINT
{
public:
	point() {					x = y = 0;															}
	point(const POINT& src) {	x = src.x; y = src.y;												}
	point(int _x, int _y) {		x = _x; y = _y;														}
	point(LPARAM lParam) {		x = GET_X_LPARAM(lParam); y = GET_Y_LPARAM(lParam);				}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- point	 2014-05-21 --- >>
//	自定义点
////////////////////////////////////////////////////////////////////////////////////////////////////////

class WINUI_API size : public tagSIZE
{
public:
	size() {					cx = cy = 0;												}
	size(const SIZE& src) {		cx = src.cx; cy = src.cy;									}
	size(const RECT rc) {		cx = rc.right - rc.left; 	cy = rc.bottom - rc.top;			}
	size(int _cx, int _cy) {	cx = _cx; cy = _cy;											}
};

///////////////////////////////////////////////////////////////////////////////////


class WINUI_API rect : public tagRECT
{
public:
	rect() {				left = top = right = bottom = 0;								}
	rect(const RECT& src) {	left = src.left; top = src.top;
							right = src.right; bottom = src.bottom;							}
	rect(int iLeft, int iTop, int iRight, int iBottom) {
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}
	rect& operator=(const RECT &rt);
	int width() const;
	int height() const;
	void clear();
	bool empty() const;
	void join(const RECT& rc);
	void resetOffset();
	void normalize();
	void offset(int cx, int cy);
	void inflate(int cx, int cy);
	void deflate(int cx, int cy);
	void Union(rect& rc);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- stringex	2014-05-21 --- >>
//	扩展std::wstring
////////////////////////////////////////////////////////////////////////////////////////////////////////
class WINUI_API stringex : public std::wstring
{
public:
	stringex() : std::wstring() {}
	stringex(const char *pstr);
	stringex(PCWSTR pstr);
	stringex(const std::wstring &ws);
	stringex(const stringex &sx);
	stringex &operator=(const stringex & sx);
	stringex &operator=(PCWSTR pstr);
	~stringex(){}

	//operator std::wstring() const {
	//	return *this;
	//}
public:
	int compareNoCase(const wchar_t *pstr) const;

	void makeUpper();
	void makeLower();
	stringex left(int nLength) const;
	stringex mid(int iPos, int nLength = -1) const;
	stringex right(int nLength) const;
	int replace(const wchar_t *pstrFrom, const wchar_t *pstrTo);
	void processResourceTokens();

	int __cdecl format(PCWSTR pstrFormat, ...);
	int __cdecl smallFormat(PCWSTR pstrFormat, ...);
public:
	//bool length() const {
	//	return __super::length();
	//}
	//wchar_t& at(size_type pos) {
	//	return __super::at(pos);
	//}
	//wchar_t& operator[](size_type pos) {
	//	return __super::at(pos);
	//}
	//bool empty() const {
	//	return __super::empty();
	//}
	//void clear() {
	//	__super::clear();
	//}
	//PCWSTR c_str() const {
	//	return __super::c_str();
	//}
	//PCWSTR data() const {
	//	return __super::data();
	//}
	//size_type find(wchar_t wchr, size_type pos = 0) const {
	//	return __super::find(wchr, pos);
	//}
	//size_type find(PCWSTR pstr, size_type pos = 0) const {
	//	return __super::find(pstr, pos);
	//}
	//size_type find_last_of(PCWSTR pstr) const {
	//	return __super::find_last_of(pstr);
	//}
	//bool operator==(PCWSTR pstr) {
	//	return __super::compare(pstr) == 0;
	//}
	//bool operator==(const stringex &sx) {
	//	return operator==(sx.c_str());
	//}	
	//stringex& operator +=(wchar_t wchr) {
	//	__super::operator +=(wchr);
	//	return *this;
	//}
	//stringex& operator +=(PCWSTR pstr) {
	//	__super::operator +=(pstr);
	//	return *this;
	//}
	//stringex& operator+=(const stringex &sx) {
	//	__super::operator +=(c_str());
	//	return *this;
	//}

public:
	// static
	static stringex RES(UINT nRes);
	/*****************************************************************************************
		<< --- static convert::toWChar	2014-04-25 --- >>
		说明：将单字符转换成宽字符串(UTF-8未测试)
		参数：
		pdata		= 被转换的字符
		isAnsi	= true:被转换的字符为ANSI码；false:否则为UTF-8
		返回值：转换后的字符串
	*****************************************************************************************/
	static std::wstring toWChar(const char *pdata, bool isAnsi = true);
	/*****************************************************************************************
		<< --- static convert::toWChar	2014-04-25 --- >>
		说明：将单字符转换成单字节字符串(UTF-8未测试)
		参数：
		pdata	= 被转换的字符
		toAnsi	= true:字符串转化为ANSI码；false:否则为UTF-8
		返回值：转换后的字符串
	*****************************************************************************************/
	static std::string toChar(const wchar_t *pdata, bool toAnsi = true);
};

//stringex operator+(const stringex &sx, PCWSTR pstr)
//{
//	stringex s = sx;
//	s += pstr;
//	return s;
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- waitCursor	2014-05-22 --- >>
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class WINUI_API waitCursor
{
public:
	waitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}
	~waitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}

protected:
	HCURSOR m_hOrigCursor;
};


typedef struct tagTImageInfo
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	bool alphaChannel;
	stringex sResType;
	DWORD dwMask;
} TImageInfo;

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API windowWnd
{
public:
	windowWnd();
	~windowWnd() {		//TRACE_CLASS_END(this)									
	}
	HWND getHWND() const;
	operator HWND() const;

	bool registerWindowClass();
	bool registerSuperclass();

	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	HWND subclass(HWND hWnd);
	void unsubclass();
	void showWindow(bool bShow = true, bool bTakeFocus = true);
	bool showModal();
	void close();
	void centerWindow();
	void setIcon(UINT nRes);

	LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	LRESULT postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	void resizeClient(int cx = -1, int cy = -1);

protected:
	virtual PCWSTR getWindowClassName() const = 0;
	virtual PCWSTR getSuperClassName() const;
	virtual UINT getClassStyle() const;

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __controlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	HWND	m_hWnd;
	WNDPROC m_OldWndProc;
	bool	m_bSubclassed;
};


}