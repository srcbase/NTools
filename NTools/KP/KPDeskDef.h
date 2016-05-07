#pragma once
#include <unordered_set>
#include "AC/sysupport.h"
#include <ostream>
//#include "AC/MFC/menu.h"
// 往用户发送调试信息
#if defined(_DEBUG)
#define SEND_LOG		__sendtoUser
#define WRITE_LOG		gtc::app::writeVSLogs
#else
#define SEND_LOG		//gtc::app::writeVSLogs//__sendtoUser
#define WRITE_LOG		//gtc::app::writeVSLogs
#endif
/****************************************************************************************
	与"快屏桌面工具栏"共享的信息配置文件
	段[deskset]：
		HideIcon：int 是否支持双击隐藏桌面功能，=1支持，否则不支持
	段[SHARE]：
		ShowDesk：int 桌面是否处于显示状态，=1显示
		KPDESK：int 桌面整理软件是否处于运行中，=1运行中
*****************************************************************************************/
#define KPDESK_SHARE_INI	L"kpzm.ini"

//////////////////////////////////////////////////////////////////////////
// 桌面右键菜单自定义项发送的消息
// WPARAM = 菜单ID；LPARAM = 消息标志
static UINT gc_uiKPDeskRegMsg			= RegisterWindowMessageW(L"KPDeskInvokeCmd");	// 注册消息
// 
static UINT gc_uiKPDeskHookRun			= RegisterWindowMessageW(L"KPDeskHookRun");	// 注册消息
static UINT gc_uiKPDeskHookStop			= RegisterWindowMessageW(L"KPDeskHookStop");	// 注册消息
// 快屏桌面窗口类
#define KPDESK_DESKTOP_CLASS			L"KpDeskDui"	// 快屏桌面窗口类
#define KPDESK_DESKTOP_CAPTION			L"快屏桌面工具栏"	// 快屏桌面窗口标题
// 快屏桌面服务窗口
#define KPDESK_DESKHOST_CLASS			L"KPHostClass"	// hostWnd窗口类名
#define KPDESK_DESKHOST_CAPTION			L"KPWork"		// hostWnd窗口标题
// 桌面重绘方式标记
#define KPDESK_REDRAW_HIDEDESK			0x001			// 是否隐藏桌面窗口
#define KPDESK_REDRAW_MOVEGROUP			0x002			// 是否重新移动组窗口
#define KPDESK_REDRAW_ALL				(KPDESK_REDRAW_HIDEDESK|KPDESK_REDRAW_MOVEGROUP)

#define KPDESK_WMME_RUNORSTOP			WM_USER+201		// 桌面右键菜单“开启快屏桌面整理”发送给KPDESK_DESKTOP_CLASS的消息
#define KPDESK_WMME_ADDNODE				WM_USER+202		// 尝试添加节点 WPARAM=节点key名称，wstring*
#define KPDESK_WMME_DELETENODE			WM_USER+203		// 尝试删除节点 WPARAM=节点key名称，wstring*；LPARAM=0删除并移除节点 1仅移除节点
#define KPDESK_WMME_UPDATENODE			WM_USER+204		// 尝试更新节点 WPARAM=节点key名称，wstring*

// 同步对象名称
#define KPDESK_MUTEX_NAME				L"KPDeskRunning"
#define KPDESK_EVENT_WAITFORSTART		L"KPDeskWaitforStart"
#define KPDESK_EVENT_WAITFOREXIT		L"KPDeskWaitforExit"
// 
#define KPDESK_GROUP_DEFAULT_TITLE		L"新建框"		// 新建框的默认名称
#define KPDESK_GROUP_TITLE_MAXLEN		20			// 组窗口名称最大字符数
#define KPDESK_GROUP_NEXTROW_SPACE		20			// 当光标距离节点容器上下边框这个距离时-->滚行
#define KPDESK_GROUP_NEXTROW_TIMERID	101			// 针对“滚行”的时钟ID
#define KPDESK_GROUP_MOVESIZE_TIMERID	102			// 针对组窗口移动或改变大小时移动节点的时钟ID

#define KPDESK_MENU_START				L"开启图标整理"
#define KPDESK_MENU_EXIT				L"关闭图标整理"
//
// 扩展菜单 消息标志，要与右键菜单扩展的顺序一致哦
#define KPDESK_MENUID_MAIN				0		// 开启或关闭快屏桌面软件
#define KPDESK_MENUID_ICONLARGE			1		// 大图标
#define KPDESK_MENUID_ICONNORMAL		2		// 中图标
#define KPDESK_MENUID_ICONSMALL			3		// 小图标
#define KPDESK_MENUID_ALGINAUTO			4		// 自动排列图标
#define KPDESK_MENUID_ALGINGRID			5		// 将图标与网格对齐
#define KPDESK_MENUID_SHOWDESK			6		// 显示桌面图标
#define KPDESK_MENUID_SORTNAME			7		// 排序，名称
#define KPDESK_MENUID_SORTFILESIZE		8		// 排序，文件大小
#define KPDESK_MENUID_SORTTYPE			9		// 排序，类型
#define KPDESK_MENUID_SORTDTMODIFY		10		// 排序，修改日期
#define KPDESK_MENUID_REFRESH			11		// 刷新


//// 菜单“排序方式”子项
//static acm::menuItem gc_menuItemSort[] = {
//	acm::menuItem(L"名称"), 
//	acm::menuItem(L"大小"), 
//	acm::menuItem(L"项目类型"), 
//	acm::menuItem(L"修改日期")
//};
// 桌面右键菜单命令ID；注意：不同的操作系统可能不一样
// 全选（Ctrl+A）
#define KPDESK_CMDID_SELECTALL_W732			28705		// 全选（Ctrl+A）
// 刷新菜单
#define KPDESK_CMDID_REFRESH_W732			28931		
#define KPDESK_ITEMID_REFRESH_W732			30981		// 刷新菜单的ID		
// 排序-名称
#define KPDESK_CMDID_SORT_NAME_W732			31492		
#define KPDESK_ITEMID_SORT_NAME_W732		30985		// 按名称排序菜单的ID
// 排序-大小
#define KPDESK_CMDID_SORT_FILESIZE_W732		31493		
#define KPDESK_ITEMID_SORT_FILESIZE_W732	30984		// 按大小排序菜单的ID
// 排序-项目类型
#define KPDESK_CMDID_SORT_TYPE_W732			31494		
#define KPDESK_ITEMID_SORT_TYPE_W732		30983		// 按项目类型排序菜单的ID
// 排序-按修改日期
#define KPDESK_CMDID_SORT_DTMODIFY_W732		31495		
#define KPDESK_ITEMID_SORT_DTMODIFY_W732	30982		// 按修改日期排序菜单的ID

// 查看-大图标256*256
#define KPDESK_CMDID_VIEW_ICONLARGE_W732	28751		
#define KPDESK_ITEMID_VIEW_ICONLARGE_W732	30993		// 查看-大图标菜单的ID	
// 查看-中等图标48*48
#define KPDESK_CMDID_VIEW_ICONNORMAL_W732	28750		
#define KPDESK_ITEMID_VIEW_ICONNORMAL_W732	30992		// 查看-中图标菜单的ID			
// 查看-小图标32*32
#define KPDESK_CMDID_VIEW_ICONSMALL_W732	28752		
#define KPDESK_ITEMID_VIEW_ICONSMALL_W732	30991		// 查看-小图标菜单的ID	
// 查看-自动排列图标
#define KPDESK_CMDID_VIEW_ALIGNAUTO_W732	28785		
#define KPDESK_ITEMID_VIEW_ALIGNAUTO_W732	30990		// 查看-自动排列图标菜单的ID	
// 查看-将图标与网格对齐
#define KPDESK_CMDID_VIEW_ALIGNGRID_W732	28788		
#define KPDESK_ITEMID_VIEW_ALIGNGRID_W732	30989		// 查看-将图标与网格对齐菜单的ID	
// 查看-显示桌面
#define KPDESK_CMDID_VIEW_SHOWDESK_W732		29698		
#define KPDESK_ITEMID_VIEW_SHOWDESK_W732	30987		// 查看-显示桌面菜单的ID	


//////////////////////////////////////////////////////////////////////////
// 调试信息输出
static void __sendtoUser(PCWSTR pstrFormat, ...)
{
//#ifdef _DEBUG
	wchar_t szBuffer[1024] = { 0 };
	va_list args;
	va_start(args, pstrFormat);
	std::vswprintf(szBuffer, _countof(szBuffer) - 2, pstrFormat, args);
	va_end(args);
	gtc::app::sendtoUser(szBuffer, L"Untitled - testDeskHost" /*L"无标题 - testDeskHost"*/);
	//gtc::app::writeLogs(szBuffer, L"SEND_LOG");
	std::wcscat(szBuffer, L"\n");
	::OutputDebugStringW(szBuffer);
//#endif
}


//////////////////////////////////////////////////////////////////////////
// 系统颜色
#define KPDESK_COLOR_GROUP_TITLE			0x20000000	// 组窗口标题区背景色
#define KPDESK_COLOR_GROUP_TITLE_HOT		0x40000000	// 组窗口选中时标题区背景色
#define KPDESK_COLOR_GROUP_TITLE_TEXT		0xFFFFFFFF	// 组窗口标题区文本颜色
#define KPDESK_COLOR_GROUP_TITLE_TEXT_HOT	0xFFFFFFFF	// 组窗口选中时标题区文本颜色
#define KPDESK_COLOR_GROUP_CLIENT			0x20000000	// 组窗口客户区背景色
#define KPDESK_COLOR_GROUP_CLIENT_HOT		0x40000000	// 组窗口选中时客户区背景色
#define KPDESK_COLOR_GROUP_BORDER			0x60FFFFFF	// 组窗口边框颜色
#define KPDESK_COLOR_GROUP_BORDER_HOT		0x60FFFFFF	// 组窗口选中时边框颜色
#define KPDESK_COLOR_GROUP_CUTLINE			0x40FFFFFF	// 组窗口标题区与客户区间分隔线颜色
#define KPDESK_COLOR_GROUP_CUTLINE_HOT		0x40FFFFFF	// 组窗口选中时标题区与客户区间分隔线颜色

#define KPDESK_COLOR_SCROLL_BK				0x40FFFFFF	// 滚动条背景色
#define KPDESK_COLOR_SCROLL_BK_HOT			0x80FFFFFF	// 滚动条选中时背景色

#define KPDESK_COLOR_DRAPBAR				0x602DDBC6	// 节点插入时滑块背景色
//////////////////////////////////////////////////////////////////////////
// 桌面系统图标是否隐藏标记
#define KPDESK_HIDE_COMPUTE		0x00000001		// 我的电脑
#define KPDESK_HIDE_NETWORK		0x00000002		// 网络
#define KPDESK_HIDE_CONTROL		0x00000004		// 控制面板
#define KPDESK_HIDE_USER		0x00000008		// 用户
#define KPDESK_HIDE_RECYCLE		0x00000010		// 回收站


//////////////////////////////////////////////////////////////////////////
// 组窗口更新标记
#define KPDESK_SCROLL_FIRST		0	// 
#define KPDESK_SCROLL_UPDATE	1	// 
#define KPDESK_SCROLL_LAST		2	// 
#define KPDESK_SCROLL_TOPKEY	3	// 
#define KPDESK_SCROLL_MOVENODE	4	// 仅仅移动show或cut节点

///////////////////////////////////////////////////////////////////////////////////////////////////
//	不可见节点移动的位置	2015-1-7
#define  KPDESK_HIDENODE_POSX	-1000	// x轴坐标
#define  KPDESK_HIDENODE_POSY	-1000	// x轴坐标

//////////////////////////////////////////////////////////////////////////
// 线程信息
typedef struct BSThreadInfo {
	HANDLE			hThread;		// 线程句柄
	std::wstring	wsDesc;			// 描述信息
	void			*pParameter;	// 
	BSThreadInfo(HANDLE hthread, PCWSTR pdesc)
		: hThread(hthread), wsDesc(pdesc?pdesc:L""), pParameter(NULL)
	{}
	~BSThreadInfo()
	{
		WRITE_LOG(L"~BSThreadInfo free ...");
	}
}BSThreadInfo;

//////////////////////////////////////////////////////////////////////////
// 节点插入点位置标记
typedef enum BENodeDrapPos
{
	BENodeDrapPosNone	= 0,	// 
	BENodeDrapPosFirst	,		// 首位置
	BENodeDrapPosLeft	,		// 光标在节点左侧
	BENodeDrapPosRight	,		// 光标在节点右侧
	//BENodeDrapPosEnd	,		// 在尾部追加
	BENodeDrapPosOn		,		// 在节点上
}BENodeDrapPos;

//////////////////////////////////////////////////////////////////////////
// 图标排序标记
typedef enum BEDeskSortFlag {
	BEDeskSortFlagNone			= 0,	// 没有排序
	BEDeskSortFlagName			,		// 按名称排序
	BEDeskSortFlagFileSize		,		// 文件大小
	BEDeskSortFlagType			,		// 类型
	BEDeskSortFlagDTModify				// 修改日期
};
// 图标资源的key标记
typedef enum BEDeskImageFlag {
	BEDeskImageFlagLock			= 1,	// IDB_PNG_LOCK
	BEDeskImageFlagUnlock		,		// IDB_PNG_UNLOCK
	BEDeskImageFlagMenu					// IDB_PNG_MENU
}BEDeskImageFlag;
// 菜单资源的key标记
typedef enum BEDeskMenuFlag {
	BEDeskMenuFlagMain			= 1		// IDR_MENU_MAIN
};
// 桌面图标的大小
typedef enum BEDeskNodeiconSize{
	BEDeskNodeiconSizeSmall		= 0,	// 32*32
	BEDeskNodeiconSizeNormal	,		// 48*48（默认）
	BEDeskNodeiconSizeLarge				// 256*256
}BEDeskNodeiconSize;
//namespace gtc{
// 默认窗口分组
typedef enum BEDeskGroupFlag {
	BEDeskGroupFlagNone			= 0,	// 未知
	BEDeskGroupFlagSystem		,		// 系统
	BEDeskGroupFlagSoft			,		// 应用程序
	BEDeskGroupFlagFolder		,		// 文件夹
	BEDeskGroupFlagDocument		,		// 文件
	//BEDeskGroupFlagCompress		,		// 文档包
	BEDeskGroupFlagPicture		,		// 图片
	BEDeskGroupFlagOther		,		// 其它
	BEDeskGroupFlag_Last		= 100	// 自定义组窗口在此以后
}BEDeskGroupFlag;

static bool inDeskGroupFlag(int groupFlag) {
	return groupFlag > BEDeskGroupFlagNone && groupFlag < BEDeskGroupFlag_Last;
}

static std::wstring toString(BEDeskGroupFlag flag) {
	switch(flag)
	{
	case BEDeskGroupFlagNone:		//	= 0,// 未知
		return L"未知";
		break;
	case BEDeskGroupFlagSystem:		//,		// 系统
		return L"系统";
		break;
	case BEDeskGroupFlagSoft:		//,		// 应用程序
		return L"软件";
		break;
	case BEDeskGroupFlagFolder:		//,		// 文件夹
		return L"文件夹";
		break;
	case BEDeskGroupFlagDocument:	//,		// 文档文件
		return L"文档";
		break;
	//case BEDeskGroupFlagCompress:	//,		// 文档包
	//	return L"文档包";
	//	break;
	case BEDeskGroupFlagPicture:	//,		// 图片
		return L"图片";
		break;
	case BEDeskGroupFlagOther:		//		// 其它
		return L"其它";
		break;
	}
	return L"";
}
// 图像文件的扩展名；要小写哦
static const wchar_t gc_pswExtnameImage[][20] = {
	L".png",	L".bmp",	L".jpg",	L".jpeg",	L".gif",
	L".tiff",	L".pcx",	L".tga",	L".exif",	L".fpx",
	L".svg",	L".psd",	L".cdr",	L".pcd",	L".dxf",
	L".ufo",	L".eps",	L".ai",		L".raw",	L".ico"
};
static const std::unordered_set<std::wstring> gc_mapExtnameImage(std::begin(gc_pswExtnameImage), std::end(gc_pswExtnameImage));

// 压缩包文件；要小写哦
static const wchar_t gc_pswExtnamePackeg[][20] = {
	L".exe",	L".msi",	L".zip",	L".7z",		L".lhaozip",
	L".rar",	L".iso"
};
static const std::unordered_set<std::wstring> gc_mapExtnamePackeg(std::begin(gc_pswExtnamePackeg), std::end(gc_pswExtnamePackeg));
// 文档文件；要小写哦
static const wchar_t gc_pswExtnameDocument[][20] = {
	L".rtf",	L".doc",	L".docx",	L".wps",	L".xls",
	L".xlsx",	L".et",		L".txt",	L".ppt",	L".pptx",
	L".dps",	L".pdf",	L".pdg",	L".chm",	L".chw",
	L".ini",	L".xml",	L".vsd"
};
static const std::unordered_set<std::wstring> gc_mapExtnameDocument(std::begin(gc_pswExtnameDocument), std::end(gc_pswExtnameDocument));


static std::wstring lvItemState(UINT state)
{
	std::wstring ws;
	if(state & LVIS_ACTIVATING)
		ws += L"LVIS_ACTIVATING ";
	else if(state & LVIS_CUT)
		ws += L"LVIS_CUT";
	else if(state & LVIS_DROPHILITED)
		ws += L"LVIS_DROPHILITED";
	else if(state & LVIS_FOCUSED)
		ws += L"LVIS_FOCUSED";
	else if(state & LVIS_OVERLAYMASK)
		ws += L"LVIS_OVERLAYMASK";
	else if(state & LVIS_SELECTED)
		ws += L"LVIS_SELECTED";
	else if(state & LVIS_STATEIMAGEMASK)
		ws += L"LVIS_STATEIMAGEMASK";

	return ws;
}
static std::wstring toString(const NMLISTVIEW *pnmlv)
{
	std::wostringstream out;
	if (pnmlv)
	{
		out << L"NMHDR:<wndFrom:" << std::setiosflags(std::ios::uppercase|std::ios::hex) << std::setw(8) << pnmlv->hdr.hwndFrom
			<< L" idFrom:" << pnmlv->hdr.idFrom << L" code:" << (long)pnmlv->hdr.code 
			<< L" item:" << pnmlv->iItem << L" subItem:" << pnmlv->iSubItem
			<< L" changed:" << pnmlv->uChanged << L" action:<" << pnmlv->ptAction.x << L"," << pnmlv->ptAction.y << L">"
			<< L" oldState:<" << lvItemState(pnmlv->uOldState) << L">"
			<< L" newState:<" << lvItemState(pnmlv->uNewState) << L">"
			<< L" param:" << std::setiosflags(std::wios::uppercase|std::wios::hex|std::wios::showbase) << pnmlv->lParam
			<< std::endl;
	}

	return out.str();
}
//}