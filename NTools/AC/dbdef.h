#ifndef __DBDEF_H__
#define __DBDEF_H__
#pragma once

namespace gtc{

///////////////////////////////////////////////////////////////////////////////////////////////////
// 数据库参数的输入方向
enum BEParameterDirection{
    BEParameterDirectionUnknown			= 0,//adParamUnknown,// = 0,
    BEParameterDirectionInput			= 1,//adParamInput,// = 1,
    BEParameterDirectionOutput			= 2,//adParamOutput,// = 2,
    BEParameterDirectionInputOUtput		= 3,//adParamInputOutput,// = 3,
    BEParameterDirectionReturnValue		= 4//adParamReturnValue// = 4
};
/*****************************************************************************************
	<< --- toStringBEParameterDirection		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	dir = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEParameterDirection dir)
{
	switch (dir)
	{
	case BEParameterDirectionUnknown:			//= 0,		// 无 系统中有CM_NONE = 0
		return L"BEParameterDirectionUnknown";
		break;
	case BEParameterDirectionInput:			//= 1,		// 记次
		return L"BEParameterDirectionInput";
		break;
	case BEParameterDirectionOutput:			//= 2,		// 求和
		return L"BEParameterDirectionOutput";
		break;
	case BEParameterDirectionInputOUtput:			//= 3,		// 求平均
		return L"BEParameterDirectionInputOUtput";
		break;
	case BEParameterDirectionReturnValue:			//= 4,		// 最大值
		return L"BEParameterDirectionReturnValue";
		break;
	default:
		break;
	}
	return L"";
}

////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// 系统支持的数据库类型
enum BEDBType{
	BEDBTypeMSSql = 1,		// MSSQL
	BEDBTypeOracle,			// 
	BEDBTypeAccess,			// 
	BEDBTypeSqlite3			// 
};
/*****************************************************************************************
	<< --- toStringBEDBType		2013-11-18 --- >>
	说明：返回BEDBType类型字符描述形式
	参数：
	dbtype	= 数据库类型
	返回值： BEDBType类型字符描述形式
*****************************************************************************************/
static std::wstring toString(BEDBType dbtype)
{
	switch(dbtype)
	{
	case BEDBTypeMSSql: //= 1,		// MSSQL
		return L"BEDBTypeMSSql";
		break;
	case BEDBTypeOracle: //,			// 
		return L"BEDBTypeOracle";
		break;
	case BEDBTypeAccess: //,			// 
		return L"BEDBTypeAccess";
		break;
	case BEDBTypeSqlite3: //			// 
		return L"BEDBTypeSqlite3";
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 命令模式
enum BEDBCmdType{
	BEDBCmdTypeText = 1,//adCmdText,	// 
	BEDBCmdTypeProc = 4	//adCmdStoredProc
};
/*****************************************************************************************
	<< --- toStringBEDBCmdType		2013-11-18 --- >>
	说明：返回BEDBType类型字符描述形式
	参数：
	cmdtype	= 数据库类型
	返回值： BEDBCmdType类型字符描述形式
*****************************************************************************************/
static std::wstring toString(BEDBCmdType cmdtype)
{
	switch(cmdtype)
	{
	case BEDBCmdTypeText: //= 1,		// MSSQL
		return L"BEDBCmdTypeText";
		break;
	case BEDBCmdTypeProc: //,			// 
		return L"BEDBCmdTypeProc";
		break;
	}
	return L"";
}

}
#endif