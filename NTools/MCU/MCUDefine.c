/******************************************************************************
-- 单片机相关的定义 
-- 
--	
--	xu
-- 2013-06-01
*******************************************************************************/
#include <reg52.h>
#include "MCUDefine.h"

/*********************************************************************
-- 简单的延时函数
-- 晶振频率为11.0592MHz，单位毫秒级。
-- xu
-- 2013-06-01
*********************************************************************/ 
void MX_Delayms(BYTE ms)
{
	BYTE i, j;
	for(i = ms; i > 0; i--)
		for(j = 110; j > 0; j--);
}

/*****************************************************************************************
	<< ---  mx_toHexChar	2014-02-13 --- >>
	说明：转换成十六进制字符“0,1,2,...E,F”
	参数：
		num = 被转换的数（0-15）
	返回值：转换后的十六进制字符'0''1'--'E''F'
*****************************************************************************************/
char mx_toHexChar(BYTE num)
{
	char temp;
	switch(num)
	{
	case 0:	temp = '0';break;
	case 1:	temp = '1';break;
	case 2:	temp = '2';break;
	case 3:	temp = '3';break;
	case 4:	temp = '4';break;
	case 5:	temp = '5';break;
	case 6:	temp = '6';break;
	case 7:	temp = '7';break;
	case 8:	temp = '8';break;
	case 9:	temp = '9';break;
	case 10:	temp = 'A';break;
	case 11:	temp = 'B';break;
	case 12:	temp = 'C';break;
	case 13:	temp = 'D';break;
	case 14:	temp = 'E';break;
	case 15:	temp = 'F';break;
	default:temp = ' ';break;
	}
	return temp;
}