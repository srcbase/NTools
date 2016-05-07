#ifndef __GTDCSDATA_H__
#define __GTDCSDATA_H__
#pragma once


// 表示器内容
#pragma pack(push)
#pragma pack(1)
struct pn_infor{
	unsigned short addr;	// 表示器地址
	unsigned char modem_no;	// 模板号
	char msg[400];			// 显示内容
	unsigned int len;		// 有效数据长度
};
#pragma pack(pop)

// 指令的应答事件（发送给用户）标记
#define WM_G                        WM_USER+114
#define WM_T                        WM_USER+115
#define WM_Ac                       WM_USER+116
#define WM_Z1                       WM_USER+117
#define WM_Pn                       WM_USER+118
#define WM_H						WM_USER+119
#define WM_Log						WM_USER+200		// 通知用户的日志信息，比如重连socket等信息
// G命令应答标志
#define _GTCMD_RECVTYPE_G		't'	// 74H
// Pn命令应答标志
#define _GTCMD_RECVTYPE_Pn		'd'	// 64H
// H命令应答标志
#define _GTCMD_RECVTYPE_H		'h'	// 68H
// Ac命令应答标志
#define _GTCMD_RECVTYPE_Ac		'a'	// 
// 应答命令截止符
#define _GTCMD_RECVTYPE_EOF		0x0D	// 0DH
// Pn指令中模板分割符
#define _GTCMD_MODEL_SPLIT		0x1D
// Pn指令中模板内容区域分割符
#define _GTCMD_MODELAREA_SPLIT	0x1F// ','

#endif