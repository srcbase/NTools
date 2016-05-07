#pragma once
#include "AC/sysupport.h"
namespace gtc{
//////////////////////////////////////////////////////////////////////////
// bmp位图文件头
#pragma pack(push) //保存对齐状态 
#pragma pack(2)//设定为4字节对齐 
typedef struct BSBitmapFileHeader { /* bmfh */

	WORD	wType;			//文件类型，必须是0x424D，即字符“BM”  
	DWORD	dwSize;			//文件大小  
	WORD	wReserved1;		//保留字，必须设置为0
	WORD	wReserved2;		//保留字，必须设置为0
	DWORD	dwOffBits;		//从文件头到实际位图数据的偏移字节数  
}BSBitmapFileHeader;
#pragma pack(pop)//恢复对齐状态
//////////////////////////////////////////////////////////////////////////
// 位图信息头
typedef struct BSBitmapInfoHeader{  
	DWORD	dwSize;			//信息头大小  
	LONG	lWidth;			//图像宽度，以象素为单位
	LONG	lHeight;		//图像高度，以象素为单位
	WORD	wPlanes;		//位平面数，必须为1  
	WORD	wBitCount;		//每像素位数  
	DWORD	dwCompression;	//压缩类型  
	DWORD	dwSizeImage;	//压缩图像大小字节数  
	LONG	lXPelsPerMeter;	//水平分辨率  
	LONG	lYPelsPerMeter;	//垂直分辨率  
	DWORD	dwClrUsed;		//位图实际用到的色彩数  
	DWORD	dwClrImportant;	//本位图中重要的色彩数  
}BSBitmapInfoHeader;

//////////////////////////////////////////////////////////////////////////
// 调色板定义
typedef struct BSRGBQuad{  
	BYTE rgbBlue;			//该颜色的蓝色分量  
	BYTE rgbGreen;			//该颜色的绿色分量  
	BYTE rgbRed;			//该颜色的红色分量  
	BYTE rgbReserved;		//保留值  
}BSRGBQuad;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	bmpFile类	2014-09-29
//	bmp文件类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class bmpFile
{
public:
	bmpFile(void);
	~bmpFile(void);
	// 读取bmp文件数据
	bool read(std::wstring wsFullPath);
	// 创建单色（clrFill）bmp文件
	bool create(std::wstring wsFullPath, LONG width, LONG height, COLORREF clrFill);


public:
	const BSBitmapInfoHeader& getBitmapInfo() const;
protected:
	BSBitmapFileHeader	m_bsFileHeader;
	BSBitmapInfoHeader	m_bsInfoHeader;
	BSRGBQuad			*m_pbsQuad;
	BYTE				*m_pbtData;
};

}