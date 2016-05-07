#ifndef __BASEENUM_H__
#define __BASEENUM_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// 自定义通用数据类型
typedef enum BEDataType{
	//							VT_TYPE			STL						ADO.DataType			sqlite3
	BEDataTypeNone = 0,		// VT_EMPTY			无						adEmpty					SQLITE_NULL
	//BEDataTypeNULL,			// VT_NULL
	BEDataTypeByte,			// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
	BEDataTypeBoolean,		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
	BEDataTypeString,		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
	BEDataTypeInt16,		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
	BEDataTypeInt32,		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
	BEDataTypeInt64,		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
	BEDataTypeFloat,		// VT_R4			float					adSingle				SQLITE_FLOAT
	BEDataTypeDouble,		// VT_R8			double					adDouble				SQLITE_FLOAT
	BEDataTypeDate,			// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeTime,			// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeDatetime,		// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeImage,		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
	BEDataTypeText			// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
}BEDataType;


//////////////////////////////////////////////////////////////////////////////////////////
//	2015-10-12 方向标记
typedef enum BEOrientationFlag {
	BEOrientationFlagNone			= 0,		// 
	BEOrientationFlagCenter			,			// 居中
	BEOrientationFlagTop			,			// 
	BEOrientationFlagBottom			,			// 
	BEOrientationFlagLeft			,			// 
	BEOrientationFlagRight			,			// 
	BEOrientationFlagLeftTop		,			// 
	BEOrientationFlagLeftBottom		,			// 
	BEOrientationFlagRightTop		,			// 
	BEOrientationFlagRightBottom				// 

};


///////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象动作类别
typedef enum BEItemAction {
	BEItemActionAdd		= 1,	// 新增
	BEItemActionModify	,		// 修改
	BEItemActionDelete	,		// 删除
	BEItemActionSelect			// 选中
}BEItemAction;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 动作的发送方
typedef enum BESenderFlag {
	BESenderFlagApp			= 1,	// 系统App
	BESenderFlagDoc			,		// 文档对象CDocument
	BESenderFlagMainFrame	,		// 主框架CMainFrame
	BESenderFlagView		,		// 视图CView
	BESenderFlagTreeDock	,		// 树状浮动窗口
	BESenderFlagPropDock			// 属性浮动窗口
}BESenderFlag;


//////////////////////////////////////////////////////////////////////////
// 操作系统版本
typedef enum BEOSVersion {
	BEOSVersionNone				= 0,	// 未知系统
	BEOSVersionXPHome			,		// Windows XP Home Edition
	BEOSVersionXPProfessional	,		// Windows XP Professional
	BEOSVersionXP64				,		// XP x64
	BEOSVersionServer2003		,		// Server 2003
	BEOSVersionServer2003R2		,		// Server 2003R2
	BEOSVersionVista			,		// Vista
	BEOSVersionWin7HomeBasic	,		// Windows 7 Home Basic
	BEOSVersionServer2008		,		// Server 2008
	BEOSVersionServer2008R2		,		// Server 2008 R2
	BEOSVersionWin8				,		// Windows 8
	BEOSVersionWin81			,		// Windows 8.1
	BEOSVersionServer2012		,		// Server 2012
	BEOSVersionServer2012R2		,		// Server 2012 R2
};

typedef enum BEAlignMode{
	BEAlignModeAllLeft			= DT_LEFT|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeAllRight			= DT_RIGHT|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeAllCenter		= DT_CENTER|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeLeftTop			= DT_LEFT|DT_TOP,			// 图片顶部左对齐
	BEAlignModeLeftCenter		= DT_LEFT|DT_VCENTER,		// 图片中间左对齐
	BEAlignModeLeftBottom		= DT_LEFT|DT_BOTTOM,		// 图片底部左对齐
	BEAlignModeCenterTop		= DT_CENTER|DT_TOP,			// 图片顶部居中
	BEAlignModeCenterCenter		= DT_CENTER|DT_VCENTER,		// 图片中间居中
	BEAlignModeCenterBottom		= DT_CENTER|DT_BOTTOM,		// 图片底部居中
	BEAlignModeRightTop			= DT_RIGHT|DT_TOP,			// 图片顶部右对齐
	BEAlignModeRightCenter		= DT_RIGHT|DT_VCENTER,		// 图片中间右对齐
	BEAlignModeRightBottom		= DT_RIGHT|DT_BOTTOM		// 图片底部右对齐
}BEAlignMode;



///////////////////////////////////////////////////////////////////////////////////////////////////
// 单元格的边框样式
typedef enum BEBorderStyle {
	BEBorderStyleNone			= 0x0,			// 无边界框
	BEBorderStyleTop			= 0x1,			// 仅上边框
	BEBorderStyleBottom			= 0x2,			// 仅下边框
	BEBorderStyleLeft			= 0x4,			// 仅左边框
	BEBorderStyleRight			= 0x8,			// 仅右边框
	BEBorderStyleTopBottom		= 0x3,			// 上下边框
	BEBorderStyleLeftRight		= 0xC,			// 左右边框
	BEBorderStyleRectangle		= 0xF			// 矩形边框

}BEBorderStyle;


///////////////////////////////////////////////////////////////////////////////////////////////////
// 我的计算方式
enum BEComputeMethod{
	BEComputeMethodNone			= 0,		// 无 系统中有CM_NONE = 0
	BEComputeMethodCount		= 1,		// 记次
	BEComputeMethodSum			= 2,		// 求和
	BEComputeMethodAvg			= 3,		// 求平均
	BEComputeMethodMax			= 4,		// 最大值
	BEComputeMethodMin			= 5			// 最小值
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 关系运算符
enum BERelationalOptor{
	BERelationalOptorNone				= 0,		// 未定义的
	BERelationalOptorEquals				= 1,		// 等于
	BERelationalOptorNotEqual			= 2,		// 不等于
	BERelationalOptorLessthan			= 3,		// 小于
	BERelationalOptorLessthanOrEqual	= 4,		// 小于等于
	BERelationalOptorGreaterthan		= 5,		// 大于
	BERelationalOptorGreaterthanOrEqual = 6			// 大于等于
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// 逻辑运算符
enum BELogicOptor{
	BELogicOptorNone			= 0,		// 未定义的
	BELogicOptorAnd				= 1,		// 逻辑与
	BELogicOptorOr				= 2,		// 或
	BELogicOptorNot				= 3			// 非
};

#endif	// __BASEENUM_H__