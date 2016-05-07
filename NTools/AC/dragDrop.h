#ifndef __DRAGDROP_H__
#define __DRAGDROP_H__
#pragma once
/****************************************************************************************
	<< --- 		2015-3-10 --- >>         
	说明：
	参考：
		a) Dropping Files on the Recycle Bin
		https://msdn.microsoft.com/en-us/library/bb776904(v=vs.85).aspx
		b) drag drop介绍
		http://blog.csdn.net/leehong2005/article/details/8609478
	1、从资源管理器拖文件到目标时，数据对象IDataObject支持的格式有：
		CFSTR_SHELLIDLIST("Shell IDList Array")		"UsingDefaultDragImage"
		"DragImageBits"								"DragContext"
		"DragSourceHelperFlags"						CFSTR_INDRAGLOOP("InShellDragLoop")
		CF_HDROP									CFSTR_FILENAMEA("FileName")
		CFSTR_FILENAMEW("FileNameW")				
*****************************************************************************************/
#include <ObjIdl.h>

#include "AC/basedef.h"
#include "AC/sysupport.h"

#define HIDA_GetPIDLFolder(pida) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define HIDA_GetPIDLItem(pida, i) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])

namespace acm {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-9 得到DROPEFFECT_COPY...字符描述
static std::wstring toStringEffect(DWORD dwEffect){
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%08X", dwEffect);
	switch(dwEffect)
	{
	case DROPEFFECT_COPY:
		return L"COPY";
	case DROPEFFECT_MOVE:
		return L"MOVE";
	case DROPEFFECT_LINK:
		return L"LINK";
		break;
	default:
		return pbuf;
		break;
	}
	return pbuf;
}
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-6-25 标准化格式

static std::wstring toStringCF(CLIPFORMAT cf)
{
	switch(cf)
	{
	case CF_TEXT:		// 1
		return L"CF_TEXT";
		break;
	case CF_BITMAP:           // 2
		return L"CF_BITMAP";
		break;
	case CF_METAFILEPICT:     // 3
		return L"CF_METAFILEPICT";
		break;
	case CF_SYLK:             // 4
		return L"CF_SYLK";
		break;
	case CF_DIF:              // 5
		return L"CF_DIF";
		break;
	case CF_TIFF:             // 6
		return L"CF_TIFF";
		break;
	case CF_OEMTEXT:          // 7
		return L"CF_OEMTEXT";
		break;
	case CF_DIB:              // 8
		return L"CF_DIB";
		break;
	case CF_PALETTE:          // 9
		return L"CF_PALETTE";
		break;
	case CF_PENDATA:          // 10
		return L"CF_PENDATA";
		break;
	case CF_RIFF:             // 11
		return L"CF_RIFF";
		break;
	case CF_WAVE:             // 12
		return L"CF_WAVE";
		break;
	case CF_UNICODETEXT:      // 13
		return L"CF_UNICODETEXT";
		break;
	case CF_ENHMETAFILE:      // 14
		return L"CF_ENHMETAFILE";
		break;
	case CF_HDROP:            // 15
		return L"CF_HDROP";
		break;
	case CF_LOCALE:           // 16
		return L"CF_LOCALE";
		break;
	case CF_DIBV5:            // 17
		return L"CF_DIBV5";
		break;
	case CF_MAX:              // 18
		return L"CF_MAX";
		break;
	case CF_OWNERDISPLAY:     // 0x0080
		return L"CF_OWNERDISPLAY";
		break;
	case CF_DSPTEXT:          // 0x0081
		return L"CF_DSPTEXT";
		break;
	case CF_DSPBITMAP:        // 0x0082
		return L"CF_DSPBITMAP";
		break;
	case CF_DSPMETAFILEPICT:  // 0x0083
		return L"CF_DSPMETAFILEPICT";
		break;
	case CF_DSPENHMETAFILE:   // 0x008E
		return L"CF_DSPENHMETAFILE";
		break;
	case CF_PRIVATEFIRST:     // 0x0200
		return L"CF_PRIVATEFIRST";
		break;
	case CF_PRIVATELAST:      // 0x02FF
		return L"CF_PRIVATELAST";
		break;
	case CF_GDIOBJFIRST:      // 0x0300
		return L"CF_GDIOBJFIRST";
		break;
	case CF_GDIOBJLAST:       // 0x03FF
		return L"CF_GDIOBJLAST";
		break;
	}
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d", cf);
	return pbuf;

}
static void toStringCF(CLIPFORMAT cf, PWSTR pbuf, DWORD len)
{
	std::wstring wstr = toStringCF(cf);
	std::swprintf(pbuf, len, L"%s", wstr.c_str());
}
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-6 托拽的数据格式及存储介质
struct BSDataStorage {
	FORMATETC	*pformatETC;	// 存储数据格式
	STGMEDIUM	*pstgMedium;	// 存储对象
};
typedef std::vector<BSDataStorage> DataStorageArrayType;	// 数据存储对象的集合
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-26 剪切板格式定义
struct BSFormatETC {
	FORMATETC	fetc;
	wchar_t		pFormatName[MAX_PATH];	// 注册的剪切板格式名
};
typedef std::vector<BSFormatETC>	FormatETCArrayType;

//////////////////////////////////////////////////////////////////////////
// 自定义托拽接口
class IDropTargetCallback
{
public:
	/*****************************************************************************************
		<< --- virtual IDropTargetCallback::dragEnter		2014-07-05 --- >>
		说明：准备开始托拽，当鼠标进入目标窗口时触发
		参数：
		pDataObject	= 被托拽的数据
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		pdwEffect	= 当前操作影响的动作DROPEFFECT_MOVE DROPEFFECT_COPY...
		返回值：
	*****************************************************************************************/
	virtual bool onDragEnter(IDataObject *pDataObject, DWORD keyState, POINT pt, DWORD* pdwEffect) = 0;
	/*****************************************************************************************
		<< --- virtual IDropTargetCallback::dragOver		2014-07-05 --- >>
		说明：发生在dragEnter之后，即：鼠标在托拽中移动，可以寻找目标点
		参数：
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		pdwEffect	= 当前操作影响的动作DROPEFFECT_MOVE DROPEFFECT_COPY...
		返回值：	
	*****************************************************************************************/
	virtual bool onDragOver(DWORD keyState, POINT pt, DWORD* pdwEffect) = 0;
	/*****************************************************************************************
		<< --- virtual IDropTargetCallback::dragLeave		2014-07-05 --- >>
		说明：鼠标离开托拽窗口时触发，可以处理恢复窗口状态等操作
		参数：
		返回值：
	*****************************************************************************************/
	virtual bool onDragLeave() = 0;
	/*****************************************************************************************
		<< --- virtual IDropTargetCallback::drop		2014-07-05 --- >>
		说明：托拽操作的收尾工作，如将托拽数据放入目标窗口
		参数：
		pDataObject	= 被托拽的数据
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		pdwEffect	= 当前操作影响的动作DROPEFFECT_MOVE DROPEFFECT_COPY...
		返回值：
	*****************************************************************************************/
	virtual bool onDrop(IDataObject *pDataObject, DWORD keyState, POINT pt, DWORD* pdwEffect) = 0;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dataObject  类		2015-3-6
//	托拽动作的数据封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class dataObject : public IDataObject
{
public:
	dataObject();
	~dataObject();

	// IUnknown接口实现
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	// IDataObject接口实现
	STDMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	STDMETHODIMP SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
	STDMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
	STDMETHODIMP QueryGetData(FORMATETC *pformatetc);
	STDMETHODIMP GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
	STDMETHODIMP DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSnk , DWORD *pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-9 清空数据
	void clear();
	/****************************************************************************************
		<< --- 	isDataAvailable	2015-3-6 --- >>         
		说明：查验指定格式的数据是否存在
		参数：
			cfFormat	= 被查验的数据格式，如：CF_TEXT 或RegisterClipboardFormat注册的格式
			lpFormatEtc	= 不为空时返回找到的格式
		返回值：被查验的格式是否存在
	*****************************************************************************************/
	//bool isDataAvailable(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);
	/****************************************************************************************
		<< --- 		2015-3-6 --- >>         
		说明：将数据存入剪切板
	*****************************************************************************************/
	////bool setClipboard();
	//////////////////////////////////////////////////////////////////////////////////////////////
	//////	2015-3-6 将数据存入数据对象，数据格式：CF_TEXT
	////bool setGlobalData(PCSTR pstr);
	//////////////////////////////////////////////////////////////////////////////////////////////
	//////	2015-3-6 将数据存入数据对象，数据格式：CF_UNICODETEXT
	////bool setGlobalData(PCWSTR pwstr);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-6 将数据存入数据对象，数据格式：cfFormat
	//bool setGlobalData(CLIPFORMAT cfFormat, HGLOBAL globalData);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-9 将文件集合存入数据对象，数据格式：CF_HDROP
	bool setFileData(const std::vector<std::wstring> &vecFile);
	////////////////////////////////////////////////////////////////////////////////////////////
	////	2015-3-10 	，数据格式：CFSTR_TARGETCLSID
	//bool setTargetData(const ITEMIDLIST *pidlTarget);
	//	2015-3-16 设定拖放描述信息，格式：CFSTR_DROPDESCRIPTION
	bool setDropDescription(DROPIMAGETYPE dimgType, PCWSTR pMessage, PCWSTR pInsert = NULL);
	bool setDropDescription(DWORD dwEffect, const std::wstring &wsTarget);
	////////////////////////////////////////////////////////////////////////////////////////////
	////	2015-3-12 更新指定格式数据
	//bool updateData(CLIPFORMAT cfFormat, HGLOBAL globalData);

public:
	// static 
	// 复制数据
	static bool copyMedium(STGMEDIUM* lpDest, STGMEDIUM* lpSource, CLIPFORMAT cfFormat);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-12 枚举数据对象支持的数据格式
	static std::size_t enumFormat(IDataObject *pDataObject, FormatETCArrayType &vecFormatETC);
	//// 将数据存入系统剪切板
	//static bool setClipboard(IDataObject *pdataObject);
	//// 设定CFSTR_TARGETCLSID格式数据
	//static bool setTargetData(IDataObject *pDataObject);
	//////////////////////////////////////////////////////////////////////////////////////////
	// 从CF_HDROP数据中解析文件集合
	static bool parseDropFile(IDataObject *pdataObject, std::vector<std::wstring> &vecFile);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-12 从数据对象中解析字符串
	static bool getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, std::wstring &wsData);
	static bool getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, std::string &csData);
	static bool getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, std::wstring &wsData);
	static bool getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, std::string &csData);
	static bool setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, const std::wstring &wsData, bool bForce = true);
	static bool setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, const std::string &csData, bool bForce = true);
	static bool setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, const std::wstring &wsData, bool bForce = true);
	static bool setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, const std::string &csData, bool bForce = true);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-22 从数据对象中解析字符串DWORD
	static bool getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, DWORD &dwData);
	static bool getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, DWORD &dwData);
	static bool setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, DWORD dwData, bool bForce = true);
	static bool setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, DWORD dwData, bool bForce = true);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-22 从数据对象中解析DWORD
	static bool getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, FORMATETC& formatEtc, STGMEDIUM& stgMedium);
	static bool getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, FORMATETC& formatEtc, STGMEDIUM& stgMedium);
	static bool setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, HGLOBAL hgData, bool bForce = true);
	static bool setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, HGLOBAL hgData, bool bForce = true);
private:
	dataObject(const dataObject &);
	dataObject& operator=(const dataObject &);


protected:
	volatile LONG           m_lRefCount;		// 引用计数器
	int						m_iCurDataIndex;	// QueryGetData中设定为当前查到格式的数据索引
	DataStorageArrayType	m_vecData;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dropSource  类		2015-3-6
//	拖放源的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class dropSource : public IDropSource
{
public:
	dropSource();
	~dropSource();

	// IUnknown接口实现
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	// IDropSource接口实现
	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHODIMP GiveFeedback(DWORD dwEffect);

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-6 内部调用DoDragDrop开始托拽	
	bool startDragDrop(dataObject *pDataObject, DWORD dwOKEffects, LPDWORD pdwEffect);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-9 初始化IDragSourceHelper
	bool initFromWindow(HWND hWnd, POINT pt,IDataObject* pDataObject);
	bool initFromBitmap(HBITMAP hBitmap, POINT ptMouse, RECT rcDrawScreen, IDataObject* pDataObject, 
		COLORREF crColorKey = 0xFFFFFFFF);

	dataObject* getDataObject();
protected:
	bool setDragImageCursor(DWORD dwEffect);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-23 清空DROPDESCRIPTION结果，清空前结构有数据时返回true
	static bool clearDescription(DROPDESCRIPTION *pDropDescription);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-23 
	static DROPIMAGETYPE dropEffectToDropImage(DWORD dwEffect);
public:
	std::wstring getDropDescInsert() const {				return m_wsInsertTarget;			}
	void setDropDescInsert(const std::wstring &wsInsert) {	m_wsInsertTarget = wsInsert;		}
protected:
	volatile LONG		m_lRefCount;			// 引用计数器

	bool				m_bSetCursor;			// internal flag set when Windows cursor must be set
	std::wstring		m_wsInsertTarget;		// 
	dataObject			*m_pDataObject;
	IDragSourceHelper	*m_pDragSourceHelper;	// 
	IDragSourceHelper2	*m_pDragSourceHelper2;	// Drag image helper 2 (SetFlags function)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dropTarget  类		2015-3-9
//	拖放目标封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class dropTarget : public IDropTarget
{
public:
	dropTarget();
	~dropTarget();

	bool registerDropTarget(HWND hWnd);
	void revokeDropTarget();
	void addSupportedFormat(CLIPFORMAT fmt);
	void addSupportedFormat(FORMATETC &fmtETC);
	void allowDragDrop(bool bAllowDrop = true);
	bool queryDrop(DWORD grfKeyState, LPDWORD pdwEffect);
	//DROPEFFECT FilterDropEffect(DWORD grfKeyState, DROPEFFECT dwEffectSrc);
	// IUnknown接口实现
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);
	// IDropTarget接口实现
	STDMETHODIMP DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	STDMETHODIMP DragLeave(void);
	STDMETHODIMP Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

public:
	IDropTargetCallback *getOwner() const;
	void setOwner(IDropTargetCallback *pOwner);
protected:
	// 所支持的拖放格式
	std::vector<FORMATETC>	m_vecFormatETC;
	FORMATETC			*m_pFormatETC;
	//!< The flag indicates whether allow drag or not  
	bool                m_bAllowDrop;       
	//!< The flag indicates the data available or not       
	bool                m_isDataAvailable;        
	//!< The target window handle  
	HWND                m_hTargetWnd;       
	//!< The reference count        
	volatile LONG       m_lRefCount;              
	//!< The pointer of IDropTargetHelper interface  
	IDropTargetHelper	*m_pDropTargetHelper;
	IDropTargetCallback	*m_pDTargetCallback;

};
}	// end gtc
#endif	// end __DRAGDROP_H__