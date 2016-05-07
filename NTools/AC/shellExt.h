#ifndef __SHELLEXT_H__
#define __SHELLEXT_H__
#pragma once
#include "AC/basedef.h"
//#include "dragDrop.h"
//#include <ShlObj.h>

// shell菜单项范围
#define SHELL_MENU_MIN_ID		1
#define SHELL_MENU_MAX_ID		1000

namespace acm {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-12 shell扩展菜单加载项	
struct BSContextMenuHandle {
	HMODULE			hDLL;			// shell扩展文件，动态加载
	IUnknown		*pUnknown;		// 
	std::wstring	wsKeyName;		// 注册表中相关键项名称
	std::wstring	wsClsid;		// 相关CLSID

	BSContextMenuHandle()
		: hDLL(NULL), pUnknown(NULL)
	{}
};
typedef std::vector<BSContextMenuHandle> ContextMenuHandleArrayType;

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-15 STRINGRESOURCEIMAGE
struct BSStringResourceImage {
	WORD	nLength;	
	wchar_t	chString[1];
};
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-12 	
// shell32.dll SHCreateItemFromParsingName Vista及其以后版本才支持
typedef HRESULT (__stdcall * fnSHCreateItemFromParsingName)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);


//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-25 	节点系统操作命令
enum BEFileSyscmdFlag {
	BEFileSyscmdFlagOpen			= 1,	// 打开；区分标记：&o &O
	BEFileSyscmdFlagCopy			,		// 复制；区分标记：&c &C
	BEFileSyscmdFlagCut				,		// 剪切；区分标记：&t &T
	BEFileSyscmdFlagPaste			,		// 粘贴；区分标记：&p &P
	BEFileSyscmdFlagDelete			,		// 删除；区分标记：&d &D
	BEFileSyscmdFlagUndoDelete		,		// 回收站还原；区分标记：&E
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	shell类	2014-12-26
//	windows shell 扩展
////////////////////////////////////////////////////////////////////////////////////////////////////////
class shell {
private:
	shell() {}
public:
	/****************************************************************************************
		<< --- 	findDeskHandle	2015-5-18 --- >>         
		说明：查找桌面窗口句柄
		参数：
			refProgman		= 返回“Program Manager”窗口句柄
			refShelldll		= 返回“ShellDLL_DefView”窗口句柄
			refDeskListView	= 返回“SysListView32”窗口句柄
		返回值：是否执行成功
	*****************************************************************************************/
	static bool findDeskHandle(HWND &refProgman, HWND &refShelldll, HWND &refDeskListView);
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-18 根据文件父目录与文件或文件夹名称创建IShellItem接口
	static bool createShellItem(PCWSTR pRootName, PCWSTR pName, REFIID riid, void **ppv);
	static bool createShellItem(PCWSTR pFullPath, REFIID riid, void **ppv);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-17 根据全路径文件集合创建IShellItemArray接口
	static bool createShellItemArray(const StringArrayType &vecFiles, REFIID riid, void **ppv);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-4-16 释放
	static void free(IIDLArrayType &vecIIDLs);
public:
	static UINT mc_uiSmallIconSize;		// 小图标的大小
	/****************************************************************************************
		<< --- createThumbnail		2014-12-26 --- >>         
		说明：创建文件的缩略图，支持Windows Vista [desktop apps only]
		参考：http://blog.163.com/lvan100@yeah/blog/static/6811721420118375054348/
		参数：
			pfullPath	= 需要得到缩略图文件的全路径
			imgCXCY		= 缩略图大小，宽=高；如：16 32 48 96 256 ...。当然 300x300 100x300也支持
			prioritizeOnlyThumbnail	= 当文件为系统节点、应用软件、文件夹、图片时优先处理缩略图
		返回值：= NULL失败，否则成功
	*****************************************************************************************/
	static HBITMAP createThumbnail(PCWSTR pfullPath, int imgCXCY);
	/****************************************************************************************
		<< --- 	parseLinkTarget	2015-2-3 --- >>         
		说明：通过解析.lnk文件得到其目标对象的全路径
		参数：
			plnkPath		= 被解析的文件
			wsTargetPath	= lnk文件对应的目标对象全路径
			pwsTargetParam	= 链接参数
		返回值：是否解析成功
	*****************************************************************************************/
	static bool parseLinkTarget(PCWSTR plnkPath, std::wstring &wsTargetPath, std::wstring *pwsTargetParam = NULL);

	/****************************************************************************************
		<< --- 		2015-2-3 --- >>         
		说明：通过解析.lnk文件得到图标文件路径，及其在图标文件中的位置索引
		参数：
			plnkPath		= 被解析的文件
			wsIconPath		= 文件图标所在的文件
			iIconIndex		= 图标在文件中的索引
		返回值：是否解析成功
	*****************************************************************************************/
	static bool parseLinkIconFile(PCWSTR plnkPath, std::wstring &wsIconPath, int &iIconIndex);

	/****************************************************************************************
		<< --- 		2015-2-3 --- >>         
		说明：在imageres.dll或或注册表中查找相关图标
		参数：
		pfullPath		= 被解析的文件
		wsIconPath		= 文件图标所在的文件
		iIconIndex		= 图标在文件中的索引
		返回值：是否解析成功
	*****************************************************************************************/
	static bool parseAssociateIconFile(PCWSTR pfullPath, std::wstring &wsIconPath, int &iIconIndex);

	/****************************************************************************************
		<< --- 	parseFileForIconFile	2015-2-3 --- >>         
		说明：通过解析文件得到其对应的图标文件路径，及其在图标文件中的位置索引
		参数：
			pfullPath		= 被解析的文件
			wsIconPath		= 文件图标所在的文件
			iIconIndex		= 图标在文件中的索引
		返回值：是否解析成功
	*****************************************************************************************/
	static bool parseFileForIconFile(PCWSTR pfullPath, std::wstring &wsIconPath, int &iIconIndex);
	static bool parseFileForIconFile(PCWSTR pfullPath, PWSTR pIconPath, DWORD dwIconPathLen, int &iIconIndex);
	/****************************************************************************************
		<< --- 	extractFileIcon	2015-2-3 --- >>         
		说明：提取文件对应的图标；普通文件夹通过注册表得到，图片通过缩略图，其它通过IExtractIcon得到
		参数：
			pfullPath	= 被提取文件
			iconCXCY	= 图标的大小
			hIcon		= 保存iconCXCY尺寸图标
			hSmallIcon	= 保存小图标16x16
		返回值：是否解析成功
	*****************************************************************************************/
	static bool extractFileIcon(PCWSTR pfullPath, int iconCXCY, HICON &hIcon, HICON &hSmallIcon);
	/****************************************************************************************
		<< ---	extractFolderIcon		2015-2-4 --- >>         
		说明：普通非缩略的文件夹的图标都一样，所以直接从注册表中提取一次就可以了
		参数：
			iconCXCY	= 图标的大小
			hIcon		= 保存iconCXCY尺寸图标
			hSmallIcon	= 保存小图标16x16
		返回值：是否解析成功
	*****************************************************************************************/
	static bool extractFolderIcon(int iconCXCY, HICON &hIcon, HICON &hSmallIcon);

	/****************************************************************************************
		<< --- 	parseShellMenu	2015-2-12 --- >>         
		说明：根据目录返回其菜单句柄
		参数：
			vecIIDLs		= 要解析目录或文件夹，数量 >= 1
			poutContextMenu	= 不为空时返回菜单IContextMenu*
		返回值：目录或文件夹对应的菜单，失败返回NULL
	*****************************************************************************************/
	static HMENU parseShellMenu(IShellFolder *pshFolder, IIDLArrayType &vecIIDLs, bool bRename = false, IContextMenu **poutContextMenu = NULL, HWND hwndOwner = NULL);


	/****************************************************************************************
		<< --- 		2015-2-13 --- >>         
		说明：从注册表相关位置提取上下文菜单
		参数：
			pregKey			= 所提取菜单的位置
			vecContextMenu	= 所提取的上下文菜单信息
		返回值：是否执行成功
	*****************************************************************************************/
	static bool extractContextMenuHandle(PCWSTR pregKey, ContextMenuHandleArrayType &vecContextMenu);

	static bool parseShellMenuFromContextMenuHandle(const BSContextMenuHandle &contextMenu, IContextMenu **pcontextMenu = NULL, 
		IContextMenu2 **pcontextMenu2 = NULL, IContextMenu3 **pcontextMenu3 = NULL);
	/****************************************************************************************
		<< --- 	extractStringFromDLL	2015-2-15 --- >>         
		说明：从资源文件中提取字符串
		参数：
			pregValue	= 从注册表中提取的“@”开头的字符串值
			pdllpath	= 存储被提取字符串的资源文件路径
			uiResID		= 资源ID
			wsOutValue	= 提取的字符串
			wLanguageID	= 语言环境ID
		返回值：是否成功提取
	*****************************************************************************************/
	static bool extractStringFromDLL(PCWSTR pdllpath, UINT uiResID, std::wstring &wsOutValue, WORD wLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	static bool extractStringFromDLL(const std::wstring &wsregValue, std::wstring &wsOutValue, WORD wLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));

	///****************************************************************************************
	//	<< --- 	createNewFolder	2015-2-26 --- >>         
	//	说明：在指定目录下创建“新建文件夹”
	//	参数：
	//		wsParentPath	= 新文件夹的父目录
	//	返回值：返回新建文件夹的全路径
	//*****************************************************************************************/
	// 该方法已经移到gtc::file中
	//static std::wstring createNewFolder(const std::wstring &wsParentPath);
	/****************************************************************************************
		<< --- 		2015-2-26 --- >>         
		说明：在指定目录下创建“新快捷方式”
		参数：
			wsParentPath	= 新建快捷方式的父目录
		返回值：
	*****************************************************************************************/
	static void createNewLink(const std::wstring &wsParentPath);
	/****************************************************************************************
		<< --- 	execSystemMenuCommand	2015-3-23 --- >>         
		说明：执行节点右键菜单中的系统命令，如：“打开”、“复制”、“粘贴”
		参数：
			vecIIDLs	= 要操作的文件和文件夹集合
			wsFullPath	= 要操作的文件或文件夹
			cmdFlag		= 命令标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool execSystemMenuCommand(IShellFolder *pshFolder, IIDLArrayType &vecIIDLs, BEFileSyscmdFlag cmdFlag, HWND hWnd = NULL, PCSTR pParameters = NULL);
	static bool execSystemMenuCommand(std::wstring wsFullPath, BEFileSyscmdFlag cmdFlag, HWND hWnd = NULL, PCSTR pParameters = NULL);
	static bool execSystemMenuCommand(const StringArrayType &vecFullPath, BEFileSyscmdFlag cmdFlag, HWND hWnd = NULL, PCSTR pParameters = NULL);
	/****************************************************************************************
		<< --- execSystemCommand		2015-4-8 --- >>         
		说明：通过ShellExecuteEx 对指定文件执行操作
		参数：
			pcmdFile	= 被执行的文件
			pCmdVerb	= 命令标记，如：open、edit、explore、find、print、properties
			pParameters	= 参数
		返回值：
	*****************************************************************************************/
	static bool execSystemCommand(PCWSTR pcmdFile, PCWSTR pCmdVerb = L"open", PCWSTR pParameters = NULL, HWND hWnd = NULL);
	/****************************************************************************************
		<< --- 	extractTipInfo	2015-3-27 --- >>         
		说明：得到文件或文件夹等节点的提示信息
		参数：
			piidl		= 被提取的文件或文件夹
			refwsTip	= 返回的提示信息
		返回值：
	*****************************************************************************************/
	static bool extractTipInfo(LPCITEMIDLIST piidl, std::wstring &refwsTip);
	static bool extractTipInfo(std::wstring wsFullPath, std::wstring &refwsTip);
protected:
	/****************************************************************************************
		<< --- 	loadContextMenuHandleFromCLSID	2015-2-13 --- >>         
		说明：通过CLSID得到shell扩展菜单信息
		参数：
			pCLSID		= 指定扩展项的CLSID
			menuHandle	= 返回的菜单信息
		返回值：是否执行成功
	*****************************************************************************************/
	static bool loadContextMenuHandleFromCLSID(PCWSTR pCLSID, BSContextMenuHandle &menuHandle);

	/****************************************************************************************
		<< --- 		2015-2-15 --- >>         
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	static const BSStringResourceImage* getStringResouceImage(HINSTANCE hInstance, HRSRC hResource, UINT id);

	
};


}	// end acm
#endif	// end __SHELLEXT_H__