#include "wallPaper.h"
#include <process.h>
#include <GdiPlus.h>
#include "sysupport.h"
#include "baseReg.h"
#include "AC/monitor.h"
#include "AC/file.h"
#include "AC/draw/bmp.h"
#include "AC/draw/drawer.h"
namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wallPaper类	2015-1-21
//	墙纸信息收集，以及监控墙纸的改变
////////////////////////////////////////////////////////////////////////////////////////////////////////
wallPaper::wallPaper()
	: m_beStyle(BEWallpaperStyleFill)
	, m_hReceiver(NULL), m_uiMsgFlag(0)
	, m_eventExit(NULL), m_hThread(NULL)
	, m_eventReg(NULL)
{
	// 墙纸另存的临时文件
	m_wsFullPath = gtc::app::getConfigPath();
	m_wsFullPath += L"\\wallpaper.tmp";
	//wchar_t pPath[MAX_PATH] = {0};
	//DWORD dwHigh = 0, dwLow = 0;
	//HGLOBAL hGlobal = NULL;
	//SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, pPath);
	//PathAppendW(PathAppendW, gtc::app::getConfigPath())
	//PathAppendW(pbuf, L"jzzm\\wall0001.tmp");

	refresh();
}

wallPaper::~wallPaper()
{
	stopMonitor();
}

void wallPaper::refresh(bool bcopyWallfile /*= true*/)
{
	//std::wstring wsWallPath;
	acm::regKey reg;

	m_wsSysWallFileFullPath.clear();
	if (!reg.open(HKEY_CURRENT_USER, L"Control Panel\\Desktop"))
	{
		WRITE_LOG(L"wallPaper::refresh 打开注册表失败<Control Panel\\Desktop>！！！");
	}
	else
	{
		WRITE_LOG(L"wallPaper::refresh readWallpaper 墙纸 0");
		// 样式
		DWORD dwStyle = 0;
		std::wstring wsStyle;
		if(reg.readString(L"WallpaperStyle", wsStyle))
			dwStyle = std::wcstoul(wsStyle.c_str(), NULL, 10);
		else
			reg.readUI4(L"WallpaperStyle", dwStyle);	// 狗日360壁纸
		if(dwStyle == 10)
			m_beStyle = BEWallpaperStyleFill;
		else if(dwStyle == 6)
			m_beStyle = BEWallpaperStyleFit;
		else if(dwStyle == 2)
			m_beStyle = BEWallpaperStyleStretch;
		else if(dwStyle == 0)
		{
			if(reg.readString(L"TileWallpaper", wsStyle))
				dwStyle = std::wcstoul(wsStyle.c_str(), NULL, 10);
			else
				reg.readUI4(L"TileWallpaper", dwStyle);	// 狗日360壁纸
			if(dwStyle == 1)
				m_beStyle = BEWallpaperStyleTile;
			else if(dwStyle == 0)
				m_beStyle = BEWallpaperStyleCenter;
		}
		// 墙纸0
		//if (wsWallPath.empty())
		//{
			reg.readString(L"Wallpaper", m_wsSysWallFileFullPath);

		//}

	}
	wchar_t pbuf[MAX_PATH] = {0};
	// 壁纸1
	if (m_wsSysWallFileFullPath.empty())
	{
		SystemParametersInfo(SPI_GETDESKWALLPAPER, _countof(pbuf), pbuf, 0);
		if(::PathFileExistsW(pbuf)) m_wsSysWallFileFullPath = pbuf;
	}
	// 壁纸2
	if (m_wsSysWallFileFullPath.empty())
	{
		DWORD dwLen = 0;
		DWORD dwType = REG_SZ;
		if(gtc::app::isWindows7Later())
			SHGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Desktop\\General", L"WallpaperSource", &dwType, pbuf, &dwLen );
		else
			SHGetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", &dwType, pbuf,&dwLen );
		pbuf[dwLen] = L'\0';
		if(::PathFileExistsW(pbuf)) m_wsSysWallFileFullPath = pbuf;
	}

	//SEND_LOG(L"wallPaper::refresh wall:<%s> temp:<%s>", m_wsSysWallFileFullPath.c_str(), m_wsFullPath.c_str());
	// 壁纸另存为
	if (bcopyWallfile)
	{
		DWORD dwHigh = 0, dwLow = 0;
		HGLOBAL hGlobal = NULL;
		//SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, pbuf);
		//PathAppendW(pbuf, L"jzzm\\wall0001.tmp");
		if (m_wsSysWallFileFullPath.empty())
		{
			// 系统颜色0x00BGR，而GDI+表示为0xFFRGB
			DWORD clrdesk = GetSysColor(COLOR_DESKTOP);
			clrdesk = (GetRValue(clrdesk) << 16)|GetGValue(clrdesk) << 8|GetBValue(clrdesk)|0xFF000000;
			gtc::bmpFile fileBmp;

			m_wsSysWallFileFullPath = gtc::app::getConfigPath();
			m_wsSysWallFileFullPath += L"\\wallpaper_tmp.bmp";
			if (!fileBmp.create(m_wsSysWallFileFullPath, 100, 100, clrdesk))
			{
				m_wsSysWallFileFullPath = gtc::app::getConfigPath();
				m_wsSysWallFileFullPath += L"\\wallpaper.bmp";
			}

		}
		HANDLE hFile = CreateFile(m_wsSysWallFileFullPath.c_str(),GENERIC_READ ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
		if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
		{
			dwLow = GetFileSize(hFile, &dwHigh);
			hGlobal = ::GlobalAlloc(GMEM_FIXED, dwLow);
			void* pBuffer = ::GlobalLock(hGlobal);
			if (pBuffer)
			{
				ReadFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
				::GlobalUnlock(hGlobal);
			}
			CloseHandle(hFile);
			hFile = NULL;
		}
		else
		{
			SEND_LOG(L"WARNING wallPaper::refresh “read”无法打开墙纸文件，开启线程尝试读取文件！");
			//::SendMessage(m_hReceiver, m_uiMsgFlag, 0, 0);	// 再次重绘
			gtc::thread::start(threadCopyWallfile, this, true);
		}
		if (hGlobal)
		{
			hFile = CreateFile(m_wsFullPath.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
			{
				void* pBuffer = ::GlobalLock(hGlobal);
				if (pBuffer)
				{
					WriteFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
					::GlobalUnlock(hGlobal);
				}
				CloseHandle(hFile);
				hFile = NULL;
				::SendMessage(m_hReceiver, m_uiMsgFlag, 0, true);	// 再次重绘
			}
			else
			{
				SEND_LOG(L"WARNING wallPaper::refresh “write”无法打开墙纸文件，开启线程尝试读取文件！");
				//::SendMessage(m_hReceiver, m_uiMsgFlag, 0, 0);	// 再次重绘
				gtc::thread::start(threadCopyWallfile, this, true);

			}
			::GlobalFree(hGlobal);
			hGlobal = NULL;
		}
	}
	
	//m_wsFullPath = pbuf;
	//pImage = Gdiplus::Image::FromFile(pPath);
}
void wallPaper::draw(HDC hdc)
{
	//SEND_LOG(L"ERROR wallPaper::draw 准备绘制墙纸 begin ...");
	assert(hdc);
	if(!hdc) return;
	std::wstring wsWall = m_wsFullPath;
	BEWallpaperStyle beStyle = m_beStyle;
	// Win8下墙纸另选
	if (gtc::app::isWindows8Later())
	{
		std::wstring pathWall;
		wchar_t path[MAX_PATH] = {0};
		if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path)))
		{
			pathWall = path;
			pathWall += L"\\Microsoft\\Windows\\Themes";
			std::wstring wsPath = pathWall;
			wsPath += L"\\CachedFiles\\*.jpg";
			WIN32_FIND_DATA wfd = {0};
			HANDLE hFind = ::FindFirstFile(wsPath.c_str(), &wfd);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				wsWall = pathWall;
				wsWall += L"\\CachedFiles\\";
				wsWall += wfd.cFileName;
				//WRITE_LOG(L"wallPaper::draw A WIN8 name:<%s>", wsWall.c_str());
				FindClose(hFind);
			}
		}
	}
	// 桌面区域
	SIZE szWall = {GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)};
	RECT rcWall = {0,0,szWall.cx,szWall.cy};

	// 系统颜色0x00BGR，而GDI+表示为0xFFRGB
	DWORD clrdesk = GetSysColor(COLOR_DESKTOP);
	clrdesk = (GetRValue(clrdesk) << 16)|GetGValue(clrdesk) << 8|GetBValue(clrdesk)|0xFF000000;
	if (wsWall.empty() && gtc::app::isWindowsXP())
	{
		acm::regKey reg;
		if (reg.open(HKEY_CURRENT_USER, L"Control Panel\\Desktop"))
		{
			std::wstring wsIniPath = gtc::app::getConfigPath();
			//wchar_t path[MAX_PATH] = {0};
			//if(FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, nullptr, 0, path)))
			//{
			//	WRITE_LOG(L"wallPaper::draw XP下读取路径CSIDL_APPDATA失败！");
			//}
			gtc::bmpFile file;
			wsWall = wsIniPath;
			wsWall += L"\\wallpaper_tmp.bmp";
			if (!file.create(wsWall.c_str(), 100, 100, clrdesk))
			{
				WRITE_LOG(L"wallPaper::draw 创建壁纸图片wallpaper_tmp.bmp失败！");
				wsWall = wsIniPath;
				wsWall += L"\\wallpaper.bmp";
			}

			beStyle = BEWallpaperStyleStretch;
			reg.writeString(L"Wallpaper", wsWall);
			::SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);

		}
	}
	// 绘制壁纸
	Gdiplus::Graphics grap(hdc);
	//// 背景色
	//gtc::drawer::fillRectangle(grap, clrdesk, rcWall);

	// 图片
	Gdiplus::Image *pImage = NULL;
	//if(!pImage)
	//{
	//	/*DWORD dwHigh = 0, dwLow = 0;
	//	HGLOBAL hGlobal = NULL;
	//	wchar_t pPath[MAX_PATH] = {0};
	//	SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, pPath);
	//	PathAppendW(pPath, L"jzzm\\wall0001.tmp");

	//	HANDLE hFile = CreateFile(m_wsFullPath.c_str(),GENERIC_READ ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
	//	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	//	{
	//		dwLow = GetFileSize(hFile, &dwHigh);
	//		hGlobal = ::GlobalAlloc(GMEM_FIXED, dwLow);
	//		void* pBuffer = ::GlobalLock(hGlobal);
	//		if (pBuffer)
	//		{
	//			ReadFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
	//			::GlobalUnlock(hGlobal);
	//		}
	//		CloseHandle(hFile);
	//		hFile = NULL;
	//	}
	//	else
	//	{
	//		SEND_LOG(L"WARNING 无法打开墙纸文件，可能系统正在换壁纸！！！");
	//	}
	//	if (hGlobal)
	//	{
	//		hFile = CreateFile(pPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//		if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	//		{
	//			void* pBuffer = ::GlobalLock(hGlobal);
	//			if (pBuffer)
	//			{
	//				WriteFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
	//				::GlobalUnlock(hGlobal);
	//			}
	//			CloseHandle(hFile);
	//			hFile = NULL;
	//		}
	//		::GlobalFree(hGlobal);
	//		hGlobal = NULL;
	//	}
	//	pImage = Gdiplus::Image::FromFile(pPath);*/
	//	//pImage = Gdiplus::Image::FromFile(m_wsFullPath.c_str());
	//	//if (pImage)
	//	//{

	//	//	hr = StgCreateDocfile(
	//	//		pbuf, 
	//	//		STGM_READWRITE|STGM_CREATE|STGM_SHARE_EXCLUSIVE, 
	//	//		0, 
	//	//		&pIStorage);
	//	//	hr = pIStorage->CreateStream(
	//	//		L"StreamImage1",
	//	//		STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
	//	//		0,
	//	//		0,
	//	//		&pIStream1);
	//	//	CLSID jpgClsid;
	//	//	gtc::image::getImageCLSID(L"jpg", &jpgClsid);

	//	//	pImage->Save(pIStream1, &jpgClsid);
	//	//
	//	//	delete pImage;

	//	//	pImage = Gdiplus::Image::FromStream(pIStream1);
	//	//}
	//	//else
	//	//{
	//	//	SEND_LOG(L"ERROR -------------------xxxxxxxxxx 为空1");
	//	//}
	//}

	
	pImage = Gdiplus::Image::FromFile(m_wsFullPath.c_str());
	if (pImage)
	{	
		const SIZE szImage = {pImage->GetWidth(), pImage->GetHeight()};
		RECT rcImage;
		// 
		acm::monitor moni;
		for (int idx = 0; idx < acm::monitor::monitorCount(); ++idx)
		{
			moni.setNumber(idx);
			//WRITE_LOG(L"wallPaper::draw 屏幕分辨率发生改变:%s", moni.toString().c_str());

			estimateWallparperArea(beStyle, szImage, moni.getWorkAreaSize(), rcImage, rcWall);
			::OffsetRect(&rcWall, moni.getWorkArea().left, moni.getWorkArea().top);
			//estimateWallparperArea(beStyle, szImage, szWall, rcImage, rcWall);

			//WRITE_LOG(L"wallPaper::draw deskColor:%08X style:%d  imageSrc:<%s> screenDest:<%s>  wall:<%d,%d> image:<%d,%d>", 
				//clrdesk, beStyle, gtc::format::rect(rcImage, true).c_str(), 
				//gtc::format::rect(rcWall, true).c_str(), 
				//szWall.cx, szWall.cy, szImage.cx, szImage.cy);

			if(isEmpty(rcImage))
			{
				//WRITE_LOG(L"wallPaper::draw BBB_1");
				RECT rcSrc = {0, 0, pImage->GetWidth(), pImage->GetHeight()};
				gtc::drawer::drawImage(grap, pImage, rcWall, rcSrc);
			}
			else
			{
				if (beStyle == BEWallpaperStyleTile)
				{
					//WRITE_LOG(L"wallPaper::draw BBB_2");
					SIZE szImage = {rectWidth(rcImage), rectHeight(rcImage)};
					int cols = szWall.cx/szImage.cx;
					if(szWall.cx%szImage.cx) ++cols;
					int rows = szWall.cy/szImage.cy;
					if(szWall.cy%szImage.cy) ++rows;
					RECT rc, rc2;
					for (int row = 0; row < rows; ++row)
					{
						for (int col = 0; col < cols; ++col)
						{
							rc2 = rcImage;
							rc.left = szImage.cx * col;
							rc.right = rc.left + szImage.cx;
							rc.top = szImage.cy * row;
							rc.bottom = rc.top + szImage.cy;
							if(rc.right > szWall.cx)
							{
								rc.right = szWall.cx;
								rc2.right = szWall.cx-rc.left;
							}
							if(rc.bottom > szWall.cy)
							{
								rc.bottom = szWall.cy;
								rc2.bottom = szWall.cy-rc.top;
							}
							gtc::drawer::drawImage(grap, pImage, rc, rc2);
						}
					}
				}
				else
				{
					//WRITE_LOG(L"wallPaper::draw BBB_3");
					gtc::drawer::drawImage(grap, pImage, rcWall, rcImage);
				}

			}			
			//WRITE_LOG(L"wallPaper::draw CCC");
		}

		delete pImage;
		pImage = NULL;
	}
	else
	{
		SEND_LOG(L"ERROR -------------------xxxxxxxxxx 为空2");
	}
	//if(pIStream1) pIStream1->Release();
	//if(pIStorage) pIStorage->Release();

}

void wallPaper::estimateWallparperArea(BEWallpaperStyle style, SIZE szImage, SIZE szScreen, RECT &rcImage, RECT &rcScreen)
{
	// 或取屏幕大小
	//SIZE szScreen = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	float slopeScreen = float(szScreen.cy)/szScreen.cx;
	float slopeImage = float(szImage.cy)/szImage.cx;
	rcImage.left = rcImage.top = 0;
	rcImage.right = szImage.cx;
	rcImage.bottom = szImage.cy;
	rcScreen.left = rcScreen.top = 0;
	rcScreen.right = szScreen.cx;
	rcScreen.bottom = szScreen.cy;
	switch(style)
	{
	case BEWallpaperStyleCenter:		//,	//居中
	case BEWallpaperStyleFit:			//,	//适应
		{	
			if (slopeImage > slopeScreen)
			{	// 图像尺寸窄，要截取上下各一部分
				rcImage.left = 0;
				rcImage.right = szImage.cx;
				rcImage.top = 0;
				rcImage.bottom = szImage.cy;
				if (szImage.cy > szScreen.cy)
				{
					rcImage.top = (szImage.cy-szScreen.cy)/2;
					rcImage.bottom = szImage.cy-rcImage.top;
					rcScreen.left = (szScreen.cx-szImage.cx)/2;// (float(szScreen.cx)-float(szScreen.cy)*szImage.cx/szImage.cy)/2;
					rcScreen.right = szScreen.cx-rcScreen.left;
				}
				else
				{
					rcScreen.left = (szScreen.cx-szImage.cx)/2;
					rcScreen.right = szScreen.cx-rcScreen.left;
					rcScreen.top = (szScreen.cy-szImage.cy)/2;
					rcScreen.bottom = szScreen.cy-rcScreen.top;
				}
			}
			else
			{	// 图像比较长，要截去左右各一部分
				rcImage.left = 0;
				rcImage.right = szImage.cx;
				rcImage.top = 0;
				rcImage.bottom = szImage.cy;
				if (szImage.cx > szScreen.cx)
				{
					rcImage.left = (szImage.cx - szScreen.cx)/2;
					rcImage.right = szImage.cx - rcImage.left;
					rcScreen.top = (szScreen.cy-szImage.cy)/2;// (float(szScreen.cy)-float(szScreen.cx)*szImage.cy/szImage.cx)/2;
					rcScreen.bottom = szScreen.cy-rcScreen.top;
				}
				else
				{
					rcScreen.left = (szScreen.cx-szImage.cx)/2;
					rcScreen.right = szScreen.cx-rcScreen.left;
					rcScreen.top = (szScreen.cy-szImage.cy)/2;
					rcScreen.bottom = szScreen.cy-rcScreen.top;

				}
			}
		}
		break;
		//{
		//	if (slopeImage > slopeScreen)
		//	{
		//		if (szImage.cx > szScreen.cx)
		//		{	// 绘制图像中间一部分
		//			rcImage.left = (szImage.cx-szScreen.cx)/2;
		//			rcImage.right = szImage.cx-rcImage.left;
		//			rcImage.top = (szImage.cy-szScreen.cy)/2;
		//			rcImage.bottom = szImage.cy-rcImage.top;
		//		}
		//		else
		//		{
		//			if (szImage.cy > szScreen.cy)
		//			{
		//				rcImage.top = (szImage.cy-szScreen.cy)/2;
		//				rcImage.bottom = szImage.cy-rcImage.top;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (szImage.cy > szScreen.cy)
		//		{
		//			rcImage.left = (szImage.cx-szScreen.cx)/2;
		//			rcImage.right = szImage.cx-rcImage.left;
		//			rcImage.top = (szImage.cy-szScreen.cy)/2;
		//			rcImage.bottom = szImage.cy-rcImage.top;
		//		}
		//		else
		//		{
		//			if (szImage.cx > szScreen.cx)
		//			{
		//				rcImage.left = (szImage.cx-szScreen.cx)/2;
		//				rcImage.right = szImage.cx-rcImage.left;
		//			}
		//		}
		//	}
		//}
		//break;
	case BEWallpaperStyleTile:			//,	//平铺
		{	
			if (slopeImage > slopeScreen)
			{
				if (szImage.cx > szScreen.cx)
				{	// 绘制图像左上一部分
					rcImage.right = szScreen.cx;
					rcImage.bottom = szScreen.cy;
				}
				else
				{
					if (szImage.cy > szScreen.cy)
					{
						rcImage.bottom = szScreen.cy;
					}
				}
			}
			else
			{
				if (szImage.cy > szScreen.cy)
				{
					rcImage.right = szScreen.cx;
					rcImage.bottom = szScreen.cy;
				}
				else
				{
					if (szImage.cx > szScreen.cx)
					{
						rcImage.right = szScreen.cx;
					}
				}
			}
		}
		break;
	case BEWallpaperStyleStretch:		//,	//拉伸
		{
			memset(&rcImage, 0, sizeof(rcImage));
		}
		break;

	case BEWallpaperStyleFill:			//填充
		if (slopeImage > slopeScreen)
		{	// 图像尺寸窄，要截取上下各一部分
			rcImage.left = 0;
			rcImage.right = szImage.cx;
			rcImage.top = (LONG)(float(szImage.cy) - float(szImage.cx)*szScreen.cy/szScreen.cx)/2;
			rcImage.bottom = szImage.cy - rcImage.top;
		}
		else
		{	// 图像比较长，要截去左右各一部分
			rcImage.top = 0;
			rcImage.bottom = szImage.cy;
			rcImage.left = (LONG)(float(szImage.cx) - float(szScreen.cx)*szImage.cy/szScreen.cy)/2;
			rcImage.right = szImage.cx - rcImage.left;
		}
		break;
	}
}

void wallPaper::init()
{
	wchar_t path[MAX_PATH] = {0};
	if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		m_wsSysWallPath = path;
		if (gtc::app::isWindowsXP())
		{
			m_wsSysWallPath += L"\\Microsoft";//\\Wallpaper1.bmp
		}
		else
		{
			m_wsSysWallPath += L"\\Microsoft\\Windows\\Themes";//\\TranscodedWallpaper.jpg
		}

	}
}

bool wallPaper::checkChanged()
{
	static std::wstring c_oldFullPath;
	static std::time_t	c_oldLastWrite;
	static int64_t		c_oldFileSize;
	// 先读取注册表值
	refresh(false);
	// 图片是否改变
	if (c_oldFullPath != m_wsSysWallFileFullPath)
	{
		c_oldFullPath = m_wsSysWallFileFullPath;
		WRITE_LOG(L"wallPaper::checkChanged 壁纸文件发生改变 name:<%s>", m_wsSysWallFileFullPath.c_str());
		return true;
	}
	// 文件属性变化
	BSFileAttribute fattr = {0};
	if(gtc::file::fileAttributeInfo(m_wsSysWallFileFullPath.c_str(), fattr))
	{
		if (fattr.dtLastWrite != c_oldLastWrite || fattr.llSize != c_oldFileSize)
		{
			WRITE_LOG(L"wallPaper::checkChanged 壁纸文件属性发生改变 size:<%ul,%ul> date:<%lld,%lld>", c_oldFileSize, fattr.llSize, c_oldLastWrite, fattr.dtLastWrite);
			c_oldLastWrite = fattr.dtLastWrite;
			c_oldFileSize = fattr.llSize;

			//if(ptm = std::localtime(&c_dtWallpaperWrite))
			//	SEND_LOG(L"ERROR dtLastWrite 3: %04d-%02d-%02d %02d:%02d:%02d", ptm->tm_year+1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			return true;
		}
	}


	return false;
}
unsigned int WINAPI wallPaper::threadMonitor(void *pparam)
{
	wallPaper *pWall = static_cast<wallPaper *>(pparam);
	HANDLE hWallFile = FindFirstChangeNotification(pWall->m_wsSysWallPath.c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE);
	SEND_LOG(L"wallPaper::threadMonitor 壁纸监控线程 开启");
	assert(hWallFile && hWallFile != INVALID_HANDLE_VALUE);
	//if(hWallFile == NULL || hWallFile == INVALID_HANDLE_VALUE) 
	HANDLE hHandles[] = {pWall->m_eventExit, hWallFile, pWall->m_eventReg};
	DWORD wret = 0;
	while(true)
	{
		wret = WaitForMultipleObjects(3, hHandles, FALSE, INFINITE);
		//SEND_LOG(L"NOTE wallPaper::threadMonitor 壁纸监控线程 --〉发生改变 ret:%u", wret);
		if (wret == WAIT_OBJECT_0)
		{	// 退出监控
			break;
		}
		else if (wret == WAIT_OBJECT_0 + 1)
		{
			// 壁纸文件或其属性是否发生改变
			//SEND_LOG(L"NOTE wallPaper::threadMonitor 被监控目录发生改变 dir:<%s>", pWall->m_wsSysWallPath.c_str());
			if (pWall->checkChanged())
			{
				// 通知监控窗口
				SEND_LOG(L"NOTE wallPaper::threadMonitor 被监控“目录”内容发生改变-->改变墙纸");
				pWall->refresh();
				//::SendMessage(pWall->m_hReceiver, pWall->m_uiMsgFlag, 0, 0);
			}
			// 继续监控
			if (!FindNextChangeNotification(hWallFile))
			{
				SEND_LOG(L"ERROR wallPaper::threadMonitor 继续目录监控失败！！！");
				break;
			}
		}
		else if (wret == WAIT_OBJECT_0 + 2)
		{	// 注册表值发生改变
			SEND_LOG(L"NOTE wallPaper::threadMonitor 被监控墙纸“注册表”发生改变-->改变墙纸");
			pWall->refresh();
			//::SendMessage(pWall->m_hReceiver, pWall->m_uiMsgFlag, 0, 0);
			pWall->m_regWall.notifyChangeKeyValue(false, REG_NOTIFY_CHANGE_ATTRIBUTES|REG_NOTIFY_CHANGE_LAST_SET, pWall->m_eventReg);
		}
		else
		{
			/* WAIT_TIMEOUT WAIT_FAILED */
			break;
		}

	}
	// 
	if (hWallFile)
	{
		FindCloseChangeNotification(hWallFile);
	}

	SEND_LOG(L"wallPaper::threadMonitor 壁纸监控线程 关闭");
	return 0;
}

unsigned int WINAPI wallPaper::threadCopyWallfile(gtc::thread::_BSThreadInfo *pthr)
{
	wallPaper *pWall = static_cast<wallPaper *>(pthr->pParameter);
	assert(pWall);
	HANDLE hFile = NULL;
	DWORD dwHigh = 0, dwLow = 0;
	const DWORD tryCount = 5;
	DWORD idx = 0;
	HGLOBAL hGlobal = NULL;
	SEND_LOG(L"NOTE wallPaper::threadCopyWallfile begin ...");
	__try
	{
		OleInitialize(NULL);
		if(!PathFileExistsW(pWall->m_wsSysWallFileFullPath.c_str())) __leave;

		hFile = CreateFile(pWall->m_wsSysWallFileFullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		while((hFile == NULL || hFile == INVALID_HANDLE_VALUE) && idx < tryCount*2)
		{
			++idx;
			SEND_LOG(L"NOTE wallPaper::threadCopyWallfile 准备再次“read”（%u）打开墙纸文件，以备复制！！！err:%u wallfile:<%s>", idx, GetLastError(), pWall->m_wsSysWallFileFullPath.c_str());
			Sleep(100);
			hFile = CreateFile(pWall->m_wsSysWallFileFullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
		{
			dwLow = GetFileSize(hFile, &dwHigh);
			hGlobal = ::GlobalAlloc(GMEM_FIXED, dwLow);
			void* pBuffer = ::GlobalLock(hGlobal);
			if (pBuffer)
			{
				ReadFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
				::GlobalUnlock(hGlobal);
			}
			CloseHandle(hFile);
			hFile = NULL;
		}
		else
		{
			SEND_LOG(L"ERROR wallPaper::threadCopyWallfile 无法打开墙纸文件“read”，可能系统正在换壁纸！！！");
			//::SendMessage(m_hReceiver, m_uiMsgFlag, 0, 0);	// 再次重绘
		}
		if (hGlobal)
		{
			idx = 0;
			hFile = CreateFile(pWall->m_wsFullPath.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			while((hFile == NULL || hFile == INVALID_HANDLE_VALUE) && idx < tryCount)
			{
				++idx;
				SEND_LOG(L"NOTE wallPaper::threadCopyWallfile 准备再次“write”（%u）打开墙纸文件，以备复制！！！err:%u targetfile:<%s>", idx, GetLastError(), pWall->m_wsFullPath.c_str());
				Sleep(100);
				hFile = CreateFile(pWall->m_wsFullPath.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
			{
				void* pBuffer = ::GlobalLock(hGlobal);
				if (pBuffer)
				{
					WriteFile(hFile, pBuffer, dwLow, &dwHigh, NULL);
					::GlobalUnlock(hGlobal);
				}
				CloseHandle(hFile);
				hFile = NULL;
				::SendMessage(pWall->m_hReceiver, pWall->m_uiMsgFlag, 0, true);	// 再次重绘
			}
			else
			{
				SEND_LOG(L"ERROR wallPaper::threadCopyWallfile 无法打开墙纸文件“write”，可能系统正在绘制壁纸！！！");
				::SendMessage(pWall->m_hReceiver, pWall->m_uiMsgFlag, 0, false);	// 再次重绘

			}
			::GlobalFree(hGlobal);
			hGlobal = NULL;

		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"wallPaper::threadCopyWallfile"))
	{
		WRITE_LOG(L"wallPaper::threadCopyWallfile SEH异常 ");
	}

	// 
	if(hGlobal) GlobalFree(hGlobal);
	CoUninitialize();
	SEND_LOG(L"NOTE wallPaper::threadCopyWallfile end ...");

	if(pthr->bAutofree) gtc::thread::free(pthr);
	return 1;
}
bool wallPaper::startMonitor(HWND hReceiver, UINT uMsg)
{
	assert(!m_hThread);
	if(m_hThread) return true;
	assert(hReceiver && uMsg > 0);
	if(!hReceiver || uMsg == 0) return false;
	init();
	m_hReceiver = hReceiver;
	m_uiMsgFlag = uMsg;
	// 注册表监控
	m_eventReg = CreateEvent(NULL, FALSE, TRUE, NULL);
	assert(m_eventReg);
	if(!m_eventReg) return false;
	if (gtc::app::isWindows7Later()/*gtc::app::isWindows8Later()*/)
	{
		if (m_regWall.open(HKEY_CURRENT_USER, L"Control Panel\\Desktop", KEY_NOTIFY|KEY_READ))
		{
			// 查验Wallpaper和TranscodedImageCache值的变化
			m_regWall.notifyChangeKeyValue(false, REG_NOTIFY_CHANGE_ATTRIBUTES|REG_NOTIFY_CHANGE_LAST_SET, m_eventReg);
		}
	}
	//else if (gtc::app::isWindows7Later())
	//{
	//	if (m_regWall.open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Desktop\\General", KEY_NOTIFY|KEY_READ))
	//	{
	//		// 查验WallpaperSource值的变化
	//		m_regWall.notifyChangeKeyValue(false, REG_NOTIFY_CHANGE_ATTRIBUTES|REG_NOTIFY_CHANGE_LAST_SET, m_eventReg);
	//	}
	//}
	else
	{
		if (m_regWall.open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Desktop\\General", KEY_NOTIFY|KEY_READ))
		{
			// 查验WallpaperSource、WallpaperFileTime、WallpaperLocalFileTime值的变化
			m_regWall.notifyChangeKeyValue(false, REG_NOTIFY_CHANGE_ATTRIBUTES|REG_NOTIFY_CHANGE_LAST_SET, m_eventReg);
		}

	}
	// 当退出监控时触发此消息
	m_eventExit = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, threadMonitor, this, 0, NULL);
	//if (m_hThread)
	//{
	//	InterlockedCompareExchange(&m_lThreadSwitch, 1, 0);
	//	ResumeThread(m_hThread);
	//}

	return m_hThread != NULL;
}

void wallPaper::stopMonitor()
{
	//if(m_eventReg) SetEvent(m_eventReg);
	if(m_eventExit) SetEvent(m_eventExit);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	CloseHandle(m_eventExit);
	CloseHandle(m_eventReg);
	m_eventExit = NULL;
	m_eventReg = NULL;
	m_hThread = NULL;
	m_regWall.close();
}



}	// end acm