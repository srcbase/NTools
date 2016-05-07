 /******************************************************************************
 -- TX-1C试验板数码管显示函数
 -- 
 --	
 --	
 -- 
 *******************************************************************************/
#include <reg52.h>
#include "TX1CDisplay.h"
#include "TX1CDefine.h"
//#include "D:\VSTOOLS\MCU\MCUDefine.h"
// 数码管的编码表
BYTE code g_btDigitalTubeTable[] = {
	0x3F, 0x06, 0x5B, 0x4F, // 0,1,2,3
	0x66, 0x6D, 0x7D, 0x07, // 4,5,6,7
	0x7F, 0x6F, 0x77, 0x7C, // 8,9,A,B
	0x39, 0x5E, 0x79, 0x71  // C,D,E,F
	};

/***********************************
-- TX-1C试验板数码管显示函数
-- 显示范围0~999999。负数支持以后考虑。
-- 采用动态显示，T0作为计时器提供显示频率支持
-- xu
-- 2013-06-01
***********************************/  
void TX1CDTDisplay(long val)
{
	BYTE delay = 5;
	BYTE flag0 = 7;
	BYTE wei6,wei5,wei4,wei3,wei2,wei1;
	wei6 = val/100000;//val%1000000/100000;
	if(wei6 == 0)
		flag0 = 6;
	wei5 = val%100000/10000;
	if(flag0 == 6 && wei5 == 0)
		flag0 = 5;
	wei4 = val%10000/1000;
	if(flag0 == 5 && wei4 == 0)
		flag0 = 4;
	wei3 = val%1000/100;
	if(flag0 == 4 && wei3 == 0)
		flag0 = 3;
	wei2 = val%100/10;
	if(flag0 == 3 && wei2 == 0)
		flag0 = 2;
	wei1 = val%10;

	
	// 个位
	//if(wei1 != cwei1)
	{
		SW_WELA = 1;
		P0 = 0xDF;
		SW_WELA = 0;	
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei1];
		SW_DULA = 0;
		P0 = 0xFF;	
		MX_Delayms(delay);
	}
	// 十位
	if(flag0 > 2)
	{
		SW_WELA = 1;
		P0 = 0xEF;
		SW_WELA = 0;	
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei2];
		SW_DULA = 0;
		P0 = 0xFF;	
		MX_Delayms(delay);
	}
	// 百位
	if(flag0 > 3)
	{
		SW_WELA = 1;
		P0 = 0xF7;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei3];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(delay);
	}
	// 千位
	if(flag0 > 4)
	{
		SW_WELA = 1;
		P0 = 0xFB;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei4];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5);
	}
	// 万位
	if(flag0 > 5)
	{
		SW_WELA = 1;
		P0 = 0xFD;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei5];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5);
	}
	// 十万位
	if(flag0 > 6)
	{
		SW_WELA = 1;
		P0 = 0xFE;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[wei6];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5); 
	}

}

/*********************************************************************
-- 功能：结束串口通讯。关闭中断等
-- 参数：pvalue=被显示数据
--		 len=数据长度(板子限制,最多显示3byte)
-- 返回：
-- 描述：xu,2013-06-16
**********************************************************************/
void TX1CDTDisplayHex(BYTE *pvalue, BYTE len)
{
	BYTE delay = 5;
	BYTE val = 0;
	// 最多显示3个字节
	if(len > 3)
		len = 3;

	if(pvalue == NULL || len == 0)
	{
		SW_WELA = 1;
		P0 = 0xDF;
		SW_WELA = 0;	
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[0];
		SW_DULA = 0;
		P0 = 0xFF;	
		MX_Delayms(delay);
		return;
	}
	if(len >= 1)
	{
		val = pvalue[0];
		// 1位,低4bit
		SW_WELA = 1;
		P0 = 0xDF;
		SW_WELA = 0;	
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val & 0x0F];
		SW_DULA = 0;
		P0 = 0xFF;	
		MX_Delayms(delay);
		
		// 2位,高4bit
		SW_WELA = 1;
		P0 = 0xEF;
		SW_WELA = 0;	
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val >> 4];
		SW_DULA = 0;
		P0 = 0xFF;	
		MX_Delayms(delay);
	
	}
	if(len >= 2)
	{
		val = pvalue[1];
		// 3位,低4bit
		SW_WELA = 1;
		P0 = 0xF7;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val & 0x0F];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(delay);
		// 4位,高4bit
		SW_WELA = 1;
		P0 = 0xFB;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val >> 4];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5);
	}
	if(len >= 3)
	{
		val = pvalue[2];
		// 5位,低4bit
		SW_WELA = 1;
		P0 = 0xFD;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val & 0x0F];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5);
		// 6位,高4bit
		SW_WELA = 1;
		P0 = 0xFE;
		SW_WELA = 0;
		SW_DULA = 1;
		P0 = g_btDigitalTubeTable[val >> 4];
		SW_DULA = 0;
		P0 = 0xFF;
		MX_Delayms(5); 
	}
}