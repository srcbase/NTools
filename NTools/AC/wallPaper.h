#ifndef __WALLPAPER_H__
#define __WALLPAPER_H__
#pragma once
#include "AC/sysupport.h"
#include "AC/baseReg.h"
namespace acm {
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-21	桌面墙纸的绘制样式
enum BEWallpaperStyle
{
	BEWallpaperStyleCenter		,	//居中
	BEWallpaperStyleTile		,	//平铺
	BEWallpaperStyleStretch		,	//拉伸
	BEWallpaperStyleFit			,	//适应
	BEWallpaperStyleFill			//填充
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wallPaper类	2015-1-21
//	墙纸信息收集，以及监控墙纸的改变
//	注意：
//		1、WinXP无法接收到SPI_SETDESKWALLPAPER消息；
//		2、在主题下变化壁纸时注册表中壁纸文件全路径不会发生改变，但sogo壁纸可以改变其设定值；
//	解决方法：同时监控以下3个状态的改变
//		1、同时监控注册表“HKEY_CURRENT_USER\Control Panel\Desktop\Wallpaper”值改变。
//		2、系统墙纸目录文件（Win7_later：“C:\Users\xxx\AppData\Roaming\Microsoft\Windows\Themes\TranscodedWallpaper.jpg”；
//			WinXp：“C:\Documents and Settings\xxx\Local Settings\Application Data\Microsoft\Wallpaper1.bmp”）
//			文件属性（大小，最后修改日期）的变化。
//		3、主窗口收到SPI_SETDESKWALLPAPER消息
////////////////////////////////////////////////////////////////////////////////////////////////////////
class wallPaper
{
public:
	wallPaper();
	~wallPaper();

public:
	/****************************************************************************************
		<< --- refresh		2015-1-21 --- >>         
		说明：刷新墙纸属性信息
		参数：
			bcopyWallfile	= 是否另存为壁纸文件
		返回值：
	*****************************************************************************************/
	void refresh(bool bcopyWallfile = true);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-22	绘制墙纸
	void draw(HDC hdc);

	/****************************************************************************************
		<< --- startMonitor		2015-1-21 --- >>         
		说明：启动墙纸监控线程
		参数：
			hReceiver	= 接收墙纸改变消息的窗口
			uMsg		= 消息标记
		返回值：是否启动成功
	*****************************************************************************************/
	bool startMonitor(HWND hReceiver, UINT uMsg);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-21	停止监控
	void stopMonitor();
public:
	// 根据屏幕与图标尺寸比例计算要绘制的墙纸源区域，返回屏幕源区域和输出屏幕目标区域
	static void estimateWallparperArea(BEWallpaperStyle style, SIZE szImage, SIZE szScreen, RECT &rcImage, RECT &rcScreen);

	/****************************************************************************************
		<< --- threadMonitor		2015-1-21 --- >>         
		说明：解决方案1、2的监控线程
		参数：
			pparam	= wallPaper*
		返回值：
	*****************************************************************************************/
	static unsigned int WINAPI threadMonitor(void *pparam);
	/****************************************************************************************
		<< --- threadCopyWallfile		2015-6-16 --- >>         
		说明：尝试复制墙纸文件
		参数：
			pparam	= _BSCopyWallfileData*
		返回值：
	*****************************************************************************************/
	static unsigned int WINAPI threadCopyWallfile(gtc::thread::_BSThreadInfo *pthr);
protected:
	struct _BSCopyWallfileData {
		std::wstring	wsSourceFile;	// 墙纸源文件全路径
		wallPaper		*pWallPaper;	// 目标文件全路径
	};
protected:
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-21	设定系统墙纸路径
	void init();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-22	查看注册表墙纸路径是否有变化，或其属性（大小或最后修改日期）有无变化
	bool checkChanged();
public:
	BEWallpaperStyle getStyle() const {			return m_beStyle;						}
	PCWSTR getFullPath() const {				return m_wsFullPath.c_str();			}
protected:
	BEWallpaperStyle	m_beStyle;				// 墙纸的绘制样式
	std::wstring		m_wsFullPath;			// 绘制用墙纸图片全路径
	std::wstring		m_wsSysWallPath;		// 系统墙纸路径
	std::wstring		m_wsSysWallFileFullPath;// 系统墙纸文件全路径

	acm::regKey			m_regWall;
	HANDLE				m_eventReg;		// 对注册表监控 HKEY_CURRENT_USER\Control Panel\Desktop

	HANDLE				m_eventExit;	// 退出监控事件
	HANDLE				m_hThread;		// 监控线程
	HWND				m_hReceiver;	// 接收墙纸改变消息的窗口
	UINT				m_uiMsgFlag;	// 消息标记
};
}	// end acm
#endif