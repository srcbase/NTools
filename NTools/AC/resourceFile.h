#ifndef __RESOURCEFILE_H__
#define __RESOURCEFILE_H__
#include "AC/sysupport.h"
#include "AC/image.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	resourceFile  类		2015-5-27
//	从资源文件中读取数据
//	参考资料
//	1、icon文件说明 https://msdn.microsoft.com/en-us/library/ms997538.aspx
//	2、一个icon操作类 http://code.it168.com/v/vcqingsongzhuaquexewenjiantubiao/Icons.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////
class resourceFile {
public:
	resourceFile(PCWSTR presFile = NULL);
	~resourceFile();

	// 查找ICON
	HICON findIcon(UINT iconID);
	HICON findIcon(UINT iconID, int cxIcon, int cyIcon);
	// 查找png图标
	bool findPNG(UINT pngID, gtc::image	&imgPng);
	// 查找bitmap图片
	HBITMAP findBitmap(UINT bmpID);
public:
	void setResourceFile(PCWSTR presFile);
	std::wstring getResourceFile() const;

private:
	std::wstring	m_wsResfile;	// 资源文件
	HMODULE			m_hmdlRes;		// 记载的资源文件句柄
};


}	// end gtc
#endif	// end __RESOURCEFILE_H__