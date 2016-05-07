#ifndef __CLIPBOARD_H__
#define __CLIPBOARD_H__
#pragma once
#include "dragDrop.h"

namespace acm
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
//	clipBoard 类		2015-2-26
//	剪切板操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class clipBoard
{
private:
	clipBoard() {}
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-26 CFSTR_SHELLIDLIST（即：“Shell IDList Array”）格式对应的文件信息
	struct _BSItemIDList {
		wchar_t		pFullPath[MAX_PATH];	// 全路径 或 ::{CLSID}
		ITEMIDLIST	*pIIDL;					// 调用者需要释放
	};
	typedef std::vector<_BSItemIDList>	ItemIDArrayType;

public:
	/****************************************************************************************
		<< --- enumFormat		2015-2-26 --- >>         
		说明：枚举剪切板格式
		参数：
			vecFormatETC	= 保存枚举到的剪切板格式类型集合
		返回值：剪切板包含的格式数
	*****************************************************************************************/
	static UINT enumFormat(FormatETCArrayType &vecFormatETC);

	/****************************************************************************************
		<< --- 		2015-2-26 --- >>         
		说明：查验指定类型的数据格式是否存在
		参数：
			pvecItemID	= 文件节点信息
			hWnd		= 关联到打开的剪切板的窗口句柄
		返回值：包含的文件节点数
	*****************************************************************************************/
	static UINT enumFile(StringArrayType *pvecFiles = NULL, HWND hWnd = NULL);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-26 释放
	static void free(ItemIDArrayType &vecItemID);
protected:
};

}	// end acm



#endif	// end __CLIPBOARD_H__