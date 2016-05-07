#ifndef __DEV_DEVDEF_H__
#define __DEV_DEVDEF_H__
#pragma once
#include "AC/sysupport.h"

namespace dev {

//////////////////////////////////////////////////////////////////////////
//	2016-1-11	设备类型
enum BEDeviceModel {
	BEDeviceModelNone		= 0,
	BEDeviceModelPort		,
};

//////////////////////////////////////////////////////////////////////////////////////////
// 2016-1-28		xutong	秤重包信息
struct BSPackInfo {
	long	lMaterialID;		// 对应的物料
	float	flgWeightPack;		// 目标包重。逻辑单位，与坐标系单位一致
	float	flgMarginError;		// 目标包重的误差范围。逻辑单位，与坐标系单位一致
	int		iPackCount;			// 打包的数量
	int		iRealCount;			// 实际合格的打包数量
	int		iFailedCount;		// 不合格的数量
};

}	// dev

#endif	// __DEV_DEVDEF_H__
