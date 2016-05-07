/******************************************************************************
-- 在LCD设备上显示文字 
-- 
-- 1. 设备可显示字符的行数_LCD_ROWS_，每行能显示多少个字符_LCD_COLS_
-- 2. 记第一个字符为左上角，坐标为(0,0)
--	xu
-- 2013-08-15
*******************************************************************************/
#include "lcd.h"
#include <intrins.H>
#include <stdio.h>
#include "D:\VSTOOLS\MCU\MCUDefine.h"


/*********************************************************************
-- 功能：往LCD设备发送指令
-- 参数：cmd=指令
-- 返回：
-- 描述：xu,2013-08-14
**********************************************************************/
void LCD_WriteCMD(BYTE cmd)
{
	LCDRS = 0;
	LCDRW = 0;
	LCDEN = 0;
	_nop_();
	_nop_();
	P0 = cmd;
	__NOP4__
	LCDEN = 1;
	__NOP4__
	LCDEN = 0;
	//Delay(10);
	__NOP4__
   /*
	//   while(lcd_busy());
    LCD_RS = 0;
//    LCD_RW = 0;
    LCD_EN = 0;
    _nop_();
    _nop_(); 
    P0 = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
    Delay(10);*/
}

/*********************************************************************
-- 功能：在当前位置显示一个字符
-- 参数：chr=要显示的字符
-- 返回：
-- 描述：xu,2013-08-16
**********************************************************************/
void LCD_DisplayChar(char chr)
{
	LCDRS = 1;
	LCDRW = 0;
	LCDEN = 0;
	P0 = chr;
	__NOP4__
	LCDEN = 1;
    __NOP4__
    LCDEN = 0;
	__NOP4__
    //Delay(10);
	  /*
//   while(lcd_busy());
    LCD_RS = 1;
//    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
    Delay(10); */
}

/*********************************************************************
-- 功能：初始化LCD设备
-- 参数：
-- 返回：
-- 描述：xu,2013-08-14
**********************************************************************/
void LCD_Init()
{
	MX_Delayms(15);
	LCD_WriteCMD(0x01);
	LCD_WriteCMD(0x38);
	MX_Delayms(5);
	LCD_WriteCMD(0x38);
	MX_Delayms(5);
	LCD_WriteCMD(0x38);
	MX_Delayms(5);
	LCD_WriteCMD(0x08);
	MX_Delayms(5);

	LCD_WriteCMD(0x01);
	MX_Delayms(5);
	LCD_WriteCMD(0x06);
	MX_Delayms(5);
	LCD_WriteCMD(0x0C);
	MX_Delayms(5);
	/*LCD_WriteCMD(0x01);
	MX_Delayms(5);
	LCD_WriteCMD(0x06);
	MX_Delayms(5);
	LCD_WriteCMD(0x0C);
	MX_Delayms(5); */
		 /*
    delay1(15);   
    lcd_wcmd(0x01);      //清除LCD的显示内容            
    lcd_wcmd(0x38);      //16*2显示，5*7点阵，8位数据
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);

    lcd_wcmd(0x0c);      //开显示，不显示光标  
    delay1(5);

    lcd_wcmd(0x01);      //清除LCD的显示内容
    delay1(5);	 */
}

/*********************************************************************
-- 功能：初始化LCD设备，字符位置坐标
-- 参数：x=水平字符位置
-- 		 y=垂直字符位置
-- 返回：ture=定位成功，false=定位失败
-- 描述：xu,2013-08-14
**********************************************************************/
BOOL LCD_Position(int x, int y)
{
	LCD_WriteCMD(0x80+(y==1?0x40:0x00)+x);
	return 1;
}


/*********************************************************************
-- 功能：往LCD设备输出字符，左上角第一个字符坐标为(0,0)
-- 参数：xstart=字符水平位置起点
-- 		 ystart=字符垂直位置起点
-- 		 pstring=需要显示的字符
-- 		 cbstring=需要显示的字符数
-- 返回：
-- 描述：xu,2013-08-14
**********************************************************************/
BOOL LCD_TextOut(int xstart, int ystart, const char* pstring, int cbstring)
{
	int idx, cb;
	if(ystart >= _LCD_ROWS_)
		return 0;  
	idx = 0;
	cb = cbstring > _LCD_COLS_ ? _LCD_COLS_ : cbstring;
	LCD_Position(xstart, ystart);
//	LCD_WriteCMD(0x80);
	//LCD_DisplayChar('M');
	 // lcd_wdat(0x3a);
	for(idx = 0; idx < cb; idx++)
	{
		LCD_DisplayChar(pstring[idx]);
		//MX_Delayms(10);
	}
	return 1;
}

/*********************************************************************
-- 功能：往LCD设备以十六进制输出字符，输出字符数为cbdata*2，左上角第一个字符坐标为(0,0)
-- 参数：xstart=字符水平位置起点
-- 		 ystart=字符垂直位置起点
-- 		 vpdata=需要显示的字符
-- 		 cbdata=需要显示的字符数
-- 返回：
-- 描述：xu,2013-08-14
**********************************************************************/
BOOL LCD_TextOutHex(int xstart, int ystart, const BYTE* vpdata, int cbdata)
{
	int idx, cb;
	if(ystart >= _LCD_ROWS_)
		return 0;  
	idx = 0;
	cb = cbdata > _LCD_COLS_ ? _LCD_COLS_ : cbdata;
	LCD_Position(xstart, ystart);
//	LCD_WriteCMD(0x80);
	//LCD_DisplayChar('M');
	 // lcd_wdat(0x3a);
	for(idx = 0; idx < cb; idx++)
	{
		LCD_DisplayChar(mx_toHexChar(vpdata[idx]>>4));
		LCD_DisplayChar(mx_toHexChar(vpdata[idx]&0xF));
		//MX_Delayms(10);
	}
	return 1;
}

/*********************************************************************
-- 功能：往LCD设备输出字符，右对齐。左上角第一个字符坐标为(0,0)
-- 参数：xstart=字符水平位置起点，基于0
-- 		 ystart=字符垂直位置起点，基于0
-- 		 xspace=行所能显示的字符数
-- 		 cbstring=需要显示的数值
-- 返回：
-- 描述：xu,2013-08-22
**********************************************************************/
BOOL LCD_ValueOut(int xstart, int ystart, BYTE xspace, int val)
{
	int len = 0;
	char buf[_LCD_COLS_];
	int offset = 0;
	if(xspace <= 0)
		return 0;
	len = sprintf(buf, "%d", val);
	if(len > xspace)
	{
		offset = len-xspace;
		len = xspace;
	}
	LCD_TextOut(xstart-len, ystart, buf+offset, len);
	return 1;
}

BOOL LCD_ValueOut1(int xstart, int ystart, BYTE xspace, float val, BYTE dots)
{
	int len = 0;
	char buf[_LCD_COLS_];
	char fmt[8];
	int offset = 0;
	if(xspace <= 0)
		return 0;
	if(dots > 0)
	{
		len = sprintf(fmt, "%%0.%df", (int)dots);
		//LCD_TextOut(0,0,fmt,len);
		//LCD_ValueOut(0,1,8,len);
		len = sprintf(buf, fmt, val);
	}
	else
	{
		len = sprintf(buf, "%d", (int)val);
	}
	if(len > xspace)
	{
		offset = len-xspace;
		len = xspace;
	}
	LCD_TextOut(xstart-len, ystart, buf+offset, len);
	return 1;
}