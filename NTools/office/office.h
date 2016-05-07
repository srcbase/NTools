#ifndef __OFFICE_H__
#define __OFFICE_H__
#pragma once
#include "AC/sysupport.h"



// office 2000
#ifdef _USED_OFFICE_2000_
#import "D:\NTools\office\2000\mso9.dll" rename_namespace("Office"), rename("RGB", "RGBOffice"), rename("IAccessible", "IAccessibleOffice"), rename("DocumentProperties", "DocumentPropertiesOffice")
using namespace Office;
#import "D:\NTools\office\2000\VBE6EXT.olb" rename("Application", "ApplicationVBE6"), rename("Properties","PropertiesVBE6"), \
	rename("Property","PropertyVBE6"),rename_namespace("VBE6")
using namespace VBE6;
// Excel导入库
#import "D:\NTools\office\2000\EXCEL9.olb" rename("RGB","RGBEx"), rename("DialogBox","DialogBoxEx"), \
	rename("CopyFile", "CopyFileMSExcel"), rename("ReplaceText","ReplaceTextMSExcel"), named_guids,rename_namespace("Excel")
using namespace Excel;
#endif

// office 2003
#ifdef _USED_OFFICE_2003_
#import "D:\NTools\office\2003\mso.dll" rename("RGB", "MSRGB")
#import "D:\NTools\office\2003\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "testVBE") 
#import "D:\NTools\office\2003\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "testVBE"), rename("ReplaceText", "EReplaceText"), rename("CopyFile","ECopyFile"), rename("FindText", "EFindText"), rename("NoPrompt", "ENoPrompt") 
using namespace Office;
using namespace Excel ;
#endif

// office 2007
#ifdef _USED_OFFICE_2007_
#import "D:\NTools\office\2007\mso.dll"  rename("DocumentProperties", "DocumentPropertiesXL")  rename("RGB", "RBGXL")
#import "D:\NTools\office\2007\vbe6ext.olb"
#import "D:\NTools\office\2007\excel.exe" rename("DialogBox", "DialogBoxXL") rename("RGB", "RBGXL") \
	rename("DocumentProperties", "DocumentPropertiesXL") \
	rename("ReplaceText", "ReplaceTextXL") \
	rename("CopyFile", "CopyFileXL") \
	exclude("IFont", "IPicture") no_dual_interfaces
using namespace Office;
using namespace Excel;
#endif

namespace office {
	
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	excel类			2014-10-19
//	excel处理类描述
////////////////////////////////////////////////////////////////////////////////////////////////////////

class excel
{
public:
	excel();
	~excel();

public:
	/*****************************************************************************************
		<< --- static cellIndex 2014-10-19 --- >>
		说明：根据行列得到Excel的索引，row、col基于0
		参数：
		row	= 行索引，基于0
		col	= 列索引，基于0
		返回值：excel单元格索引
	*****************************************************************************************/
	static std::wstring cellIndex(int32_t row, int32_t col);
};


}
#endif