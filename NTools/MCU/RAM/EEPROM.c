/******************************************************************************
-- 单片机相关的定义 
-- 
--	
--	xu
-- 2013-06-01
*******************************************************************************/
#include <reg52.h>
#include "EEPROM.h"
#include <intrins.h>
/*********************************************************************
-- 功能：开启STC89C5x系列单片机内部EEPROM扇区擦除功能
-- 参数：
-- 返回：先前的中断开关标记
-- xu；2013-06-09
**********************************************************************/
static bit ISP_Enable(void)
{
	bit oldea = EA;
	EA = 0;
	ISP_CONTR &= 0x18;
	ISP_CONTR |= ISP_WAIT_TIME | 0x80;

	return oldea;
}
/*********************************************************************
-- 功能：关闭STC89C5x系列单片机内部EEPROM扇区擦除功能
-- 参数：oldea= ISP_Enable()函数范围的中断开关标记
-- 返回：
-- 描述：xu,2013-06-09
**********************************************************************/
static void ISP_Disable(bit oldea)
{
	ISP_CONTR &= 0x7F;
	ISP_TRIG = 0;
	EA = oldea;
}
/*********************************************************************
-- 功能：STC89C5x系列单片机ISP,IAP命令触发开关
-- 参数：
-- 返回：
-- 描述：xu,2013-06-09
**********************************************************************/
static void ISP_Trig(BYTE cmd)
{
	ISP_CMD &= 0xF8;
	ISP_CMD |= cmd;

	ISP_TRIG = 0x46;
	ISP_TRIG = 0xB9;
	_nop_();
	_nop_();
	//_nop_();
}
/*********************************************************************
-- 功能：STC89C5x系列单片机内部EEPROM扇区擦除
-- 参数：
-- 		Address：EEPROM擦除扇区的地址
-- xu；2013-06-08
**********************************************************************/
void ISP_Erase(UINT address)
{
	bit oldea = 0;
	UINT secaddr = address & 0xFE00;
	ISP_ADDRL = 0x0;
	ISP_ADDRH = (BYTE)(secaddr >> 8);
	// 
	//ISP_CMD &= 0xF8;
	//ISP_CMD |= ISP_SECTOR_ERASE;
	// 
	oldea = ISP_Enable();
	ISP_Trig(ISP_SECTOR_ERASE);
	ISP_Disable(oldea);

}		
/*********************************************************************
-- 功能：读取STC89C5x系列单片机内部EEPROM的字节数据
-- 参数：
-- 		Address：被EEPROM的地址
-- 		返回值：读取的数据
-- xu；2013-06-08
**********************************************************************/
BYTE ISP_Read(UINT address)
{	
	BYTE rtdata = 0;
	bit oldea = 0;
	ISP_ADDRL = (BYTE)(address&0x0FF);
	ISP_ADDRH = (BYTE)(address >> 8);

	//ISP_CMD &= 0xF8;
	//ISP_CMD |= ISP_BYTE_READ;
	oldea = ISP_Enable();
	ISP_Trig(ISP_BYTE_READ);
	ISP_Disable(oldea);

	rtdata = ISP_DATA;
	return rtdata;
}  
/*********************************************************************
-- 功能：对STC89C5x系列单片机内部EEPROM进行编程
-- 参数：
-- 		Address：编程地址
--		data：编程的值
-- xu；2013-06-08
**********************************************************************/
void ISP_Write(UINT address, BYTE edata)
{
	bit oldea = 0;
	ISP_ADDRL = (BYTE)(address&0x0FF);
	ISP_ADDRH = (BYTE)(address >> 8);

	//ISP_CMD &= 0xF8;
	//ISP_CMD |= ISP_BYTE_PROGRAM;
	ISP_DATA = edata;
	
	oldea = ISP_Enable();
	ISP_Trig(ISP_BYTE_PROGRAM);
	ISP_Disable(oldea);
}	
/*********************************************************************
-- 功能：对STC89C5x系列单片机内部EEPROM进行编程并验证
-- 参数：address = 写入的地址
--		 edata = 写入的值
-- 返回：成功=MCU_OK,失败=MCU_ERROR
-- 描述：xu,2013-06-10
**********************************************************************/
BYTE ISP_WriteVerify(UINT address, BYTE edata)
{
	bit oldea = 0;
	ISP_ADDRL = (BYTE)(address & 0x0FF);
	ISP_ADDRH = (BYTE)(address >> 8);

	//ISP_CMD &= 0xF8;
	//ISP_CMD |= ISP_BYTE_PROGRAM;
	ISP_DATA = edata;
	
	oldea = ISP_Enable();
	ISP_Trig(ISP_BYTE_PROGRAM);
	// 进行读取验证
	ISP_DATA = 0x0;
	//ISP_CMD &= 0xF8;
	//ISP_CMD |= ISP_BYTE_READ;
	ISP_Trig(ISP_BYTE_READ);
	ISP_Disable(oldea);

	return 	ISP_DATA == edata ? MCU_OK : MCU_ERROR;
}

/*********************************************************************
-- 功能：向指定地址开始处连续写字节数组并验证，不允许跨扇区操作
-- 参数：address = 写入数据的起始地址
--		 pvalue = 写入的值数组
--		 len = 值数组长度
-- 返回：成功=MCU_OK,失败=MCU_ERROR
-- 描述：xu,2013-06-10
**********************************************************************/
BOOL ISP_Writes(UINT address, BYTE *pvalue, UINT len)
{
	UINT inaddr = 0, idx = 0;
	bit oldea = 0;
	// 不允许跨扇区操作
	if(len > SIZE_OF_SECTOR || pvalue == NULL)
		return MCU_ERROR;
	inaddr = address & 0x1FF;	// 计算扇区内的偏移起点
	if((inaddr + len) > SIZE_OF_SECTOR)
		return MCU_ERROR;
	
	inaddr = address;
	// 逐个写入并校验
	oldea = ISP_Enable();
	for(idx = 0; idx < len; idx++)
	{
		// 写入
		ISP_ADDRL = (BYTE)(inaddr & 0x0FF);
		ISP_ADDRH = (BYTE)(inaddr >> 8);
		ISP_DATA = pvalue[idx];

		//ISP_CMD &= 0xF8;
		//ISP_CMD |= ISP_BYTE_PROGRAM;
		ISP_Trig(ISP_BYTE_PROGRAM);
		// 读取验证
		ISP_DATA = 0x0;
	//	ISP_CMD &= 0xF8;
		//ISP_CMD |= ISP_BYTE_READ;
		ISP_Trig(ISP_BYTE_READ);
		if(ISP_DATA != pvalue[idx])
		{
			ISP_Disable(oldea);
			return MCU_ERROR;
		}
		// 
		inaddr++;
	}
	ISP_Disable(oldea);
	return MCU_OK;
}

/*********************************************************************
-- 功能：从指定地址开始处连续读取数据，不允许跨扇区操作
-- 参数：address = 读取数据的起始地址
--		 pbuf = 保存数据缓存区
--		 buflen = 数据缓存区长度
-- 返回：成功=MCU_OK,失败=MCU_ERROR
-- 描述：xu,2013-06-10
**********************************************************************/
BOOL ISP_Reads(UINT address, BYTE *pbuf, UINT buflen)
{
	UINT inaddr = 0, idx = 0;
	bit oldea = 0;
	// 不允许跨扇区操作
	if(buflen > SIZE_OF_SECTOR || pbuf == NULL)
		return MCU_ERROR;
	inaddr = address & 0x1FF;	// 计算扇区内的偏移起点
	if((inaddr + buflen) > SIZE_OF_SECTOR)
		return MCU_ERROR;
	
	inaddr = address;
	// 逐个写入并校验
	oldea = ISP_Enable();
	for(idx = 0; idx < buflen; idx++)
	{
		// 读取
		ISP_ADDRL = (BYTE)(inaddr & 0x0FF);
		ISP_ADDRH = (BYTE)(inaddr >> 8);
		ISP_DATA = 0;

	//	ISP_CMD &= 0xF8;
	//	ISP_CMD |= ISP_BYTE_READ;
		ISP_Trig(ISP_BYTE_READ);
		pbuf[idx] = ISP_DATA;
		inaddr++;
	}
	ISP_Disable(oldea);
	return MCU_OK;
}