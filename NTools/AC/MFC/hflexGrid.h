#pragma once
#include <afxshelllistCtrl.h>
#include <afxctl.h>
#include "AC/sysupport.h"
#include "AC/generalobj.h"
#include "db/dbtype.h"

#ifdef _USED_OFFICE_
	#include "office/office.h"
#endif




namespace acm
{

/////////////////////////////////////////////////////////////////////////////////////////////
// CNMSHFGrid
// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被覆盖。

/////////////////////////////////////////////////////////////////////////////
// CNMSHFGrid 包装类

class CNMSHFGrid : public CWnd
{
protected:
	DECLARE_DYNCREATE(CNMSHFGrid)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xECD9B64, 0x23AA, 0x11D0, { 0xB3, 0x51, 0x0, 0xA0, 0xC9, 0x5, 0x5D, 0x8E } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID, 
		CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
		UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
			pPersist, bStorage, bstrLicKey); 
	}

	// 特性
public:
	enum
	{
		flexOLEDragManual = 0,
		flexOLEDragAutomatic = 1
	}OLEDragConstants;
	enum
	{
		flexOLEDropNone = 0,
		flexOLEDropManual = 1
	}OLEDropConstants;
	enum
	{
		flexEnter = 0,
		flexLeave = 1,
		flexOver = 2
	}DragOverConstants;
	enum
	{
		flexCFText = 1,
		flexCFBitmap = 2,
		flexCFMetafile = 3,
		flexCFDIB = 8,
		flexCFPalette = 9,
		flexCFEMetafile = 14,
		flexCFFiles = 15,
		flexCFRTF = -16639
	}ClipBoardConstants;
	enum
	{
		flexOLEDropEffectNone = 0,
		flexOLEDropEffectCopy = 1,
		flexOLEDropEffectMove = 2,
		flexOLEDropEffectScroll = 0x80000000
	}OLEDropEffectConstants;
	enum
	{
		flexerrIllegaFunctionCall = 5,
		flexerrObjIllegalUse = 425,
		flexerrClipInvalidFormat = 461,
		flexerrDataObjectLocked = 672,
		flexerrExpectedAnArgument = 673,
		flexerrRecursiveOLEDrag = 674,
		flexerrUserFormatNotBinArray = 675,
		flexerrDataNotSetForFormat = 676,
		flexerrUnknownError = 600,
		flexerrSubscript = 381,
		flexerrBadValue = 380,
		flexerrGetNotSupported = 394,
		flexerrSetNotPermitted = 387,
		flexerrOutOfMemory = 7,
		flexerrVB30000 = 30000,
		flexerrVB30001 = 30001,
		flexerrVB30002 = 30002,
		flexerrVB30004 = 30004,
		flexerrVB30005 = 30005,
		flexerrVB30006 = 30006,
		flexerrVB30008 = 30008,
		flexerrVB30009 = 30009,
		flexerrVB30010 = 30010,
		flexerrVB30011 = 30011,
		flexerrVB30013 = 30013,
		flexerrVB30014 = 30014,
		flexerrVB30015 = 30015,
		flexerrVB30016 = 30016,
		flexerrVB30017 = 30017,
		flexerrFORMATSTRING = 30018,
		flexerrBadBandNumber = 30019,
		flexerrBadBandColIndex = 30020,
		flexerrNotAvailableWithBandedData = 30021,
		flexerrUnknownBinding = 30022,
		flexerrHROWsNotReleased = 30023
	}ErrorConstants;
	enum
	{
		flexFlat = 0,
		flex3D = 1
	}AppearanceSettings;
	enum
	{
		flexBorderNone = 0,
		flexBorderSingle = 1
	}BorderStyleSettings;
	enum
	{
		flexFocusNone = 0,
		flexFocusLight = 1,
		flexFocusHeavy = 2
	}FocusRectSettings;
	enum
	{
		flexHighlightNever = 0,
		flexHighlightAlways = 1,
		flexHighlightWithFocus = 2
	}HighLightSettings;
	enum
	{
		flexScrollBarNone = 0,
		flexScrollBarHorizontal = 1,
		flexScrollBarVertical = 2,
		flexScrollBarBoth = 3
	}ScrollBarsSettings;
	enum
	{
		flexTextFlat = 0,
		flexTextRaised = 1,
		flexTextInset = 2,
		flexTextRaisedLight = 3,
		flexTextInsetLight = 4
	}TextStyleSettings;
	enum
	{
		flexFillSingle = 0,
		flexFillRepeat = 1
	}FillStyleSettings;
	enum
	{
		flexGridNone = 0,
		flexGridFlat = 1,
		flexGridInset = 2,
		flexGridRaised = 3,
		flexGridDashes = 4,
		flexGridDots = 5
	}GridLineSettings;
	enum
	{
		flexSelectionFree = 0,
		flexSelectionByRow = 1,
		flexSelectionByColumn = 2
	}SelectionModeSettings;
	enum
	{
		flexMergeNever = 0,
		flexMergeFree = 1,
		flexMergeRestrictRows = 2,
		flexMergeRestrictColumns = 3,
		flexMergeRestrictAll = 4
	}MergeCellsSettings;
	enum
	{
		flexPictureColor = 0,
		flexPictureMonochrome = 1
	}PictureTypeSettings;
	enum
	{
		flexResizeNone = 0,
		flexResizeColumns = 1,
		flexResizeRows = 2,
		flexResizeBoth = 3
	}AllowUserResizeSettings;
	enum
	{
		flexDefault = 0,
		flexArrow = 1,
		flexCross = 2,
		flexIBeam = 3,
		flexIcon = 4,
		flexSize = 5,
		flexSizeNESW = 6,
		flexSizeNS = 7,
		flexSizeNWSE = 8,
		flexSizeEW = 9,
		flexUpArrow = 10,
		flexHourglass = 11,
		flexNoDrop = 12,
		flexArrowHourGlass = 13,
		flexArrowQuestion = 14,
		flexSizeAll = 15,
		flexCustom = 99
	}MousePointerSettings;
	enum
	{
		flexSortNone = 0,
		flexSortGenericAscending = 1,
		flexSortGenericDescending = 2,
		flexSortNumericAscending = 3,
		flexSortNumericDescending = 4,
		flexSortStringNoCaseAscending = 5,
		flexSortStringNoCaseDescending = 6,
		flexSortStringAscending = 7,
		flexSortStringDescending = 8
	}SortSettings;
	enum
	{
		flexAlignLeftTop = 0,
		flexAlignLeftCenter = 1,
		flexAlignLeftBottom = 2,
		flexAlignCenterTop = 3,
		flexAlignCenterCenter = 4,
		flexAlignCenterBottom = 5,
		flexAlignRightTop = 6,
		flexAlignRightCenter = 7,
		flexAlignRightBottom = 8,
		flexAlignGeneral = 9
	}AlignmentSettings;
	enum
	{
		flexSingleLine = 0,
		flexWordBreak = 1,
		flexWordEllipsis = 2,
		flexWordBreakEllipsis = 3
	}WordWrapSettings;
	enum
	{
		flexBandDisplayHorizontal = 0,
		flexBandDisplayVertical = 1
	}BandDisplaySettings;
	enum
	{
		flexCellTypeStandard = 0,
		flexCellTypeFixed = 1,
		flexCellTypeHeader = 2,
		flexCellTypeIndent = 3,
		flexCellTypeUnpopulated = 4
	}CellTypeSettings;
	enum
	{
		flexRowSizeIndividual = 0,
		flexRowSizeAll = 1
	}RowSizingSettings;
	enum
	{
		flexColHeaderOff = 0,
		flexColHeaderOn = 1
	}ColHeaderSettings;


	// 操作
public:

	// IMSHFlexGrid

	// Functions
	//

	long get_Rows()
	{
		long result;
		InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Rows(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Cols(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_Cols(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_FixedRows()
	{
		long result;
		InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_FixedRows(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_FixedCols()
	{
		long result;
		InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_FixedCols(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_Version()
	{
		short result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	CString get_FormatString()
	{
		CString result;
		InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_FormatString(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_TopRow()
	{
		long result;
		InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_TopRow(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_LeftCol()
	{
		long result;
		InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_LeftCol(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Row()
	{
		long result;
		InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Row(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Col()
	{
		long result;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Col(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_RowSel()
	{
		long result;
		InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RowSel(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ColSel()
	{
		long result;
		InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ColSel(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Text()
	{
		CString result;
		InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Text(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_BackColor()
	{
		unsigned long result;
		InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_ForeColor()
	{
		unsigned long result;
		InvokeHelper(DISPID_FORECOLOR, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_ForeColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(DISPID_FORECOLOR, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_BackColorBand(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8e, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_BackColorBand(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x8e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_ForeColorBand(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8f, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_ForeColorBand(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x8f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_BackColorHeader(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x90, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_BackColorHeader(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x90, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_ForeColorHeader(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x91, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_ForeColorHeader(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x91, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_BackColorIndent(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa1, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_BackColorIndent(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0xa1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_BackColorFixed()
	{
		unsigned long result;
		InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColorFixed(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0xe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_ForeColorFixed()
	{
		unsigned long result;
		InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_ForeColorFixed(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0xf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_BackColorSel()
	{
		unsigned long result;
		InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColorSel(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x10, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_ForeColorSel()
	{
		unsigned long result;
		InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_ForeColorSel(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x11, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_BackColorBkg()
	{
		unsigned long result;
		InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColorBkg(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x12, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_BackColorUnpopulated()
	{
		unsigned long result;
		InvokeHelper(0xa5, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_BackColorUnpopulated(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0xa5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_WordWrap()
	{
		BOOL result;
		InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_WordWrap(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x13, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Font()
	{
		LPDISPATCH result;
		InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void putref_Font(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	float get_FontWidth()
	{
		float result;
		InvokeHelper(0x54, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_FontWidth(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x54, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_FontFixed()
	{
		LPDISPATCH result;
		InvokeHelper(0x83, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void putref_FontFixed(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(0x83, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	float get_FontWidthFixed()
	{
		float result;
		InvokeHelper(0x99, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_FontWidthFixed(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x99, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_FontBand(long BandNumber)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x92, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, BandNumber);
		return result;
	}
	void putref_FontBand(long BandNumber, LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_DISPATCH ;
		InvokeHelper(0x92, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	float get_FontWidthBand(long BandNumber)
	{
		float result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x93, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_FontWidthBand(long BandNumber, float newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R4 ;
		InvokeHelper(0x93, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	LPDISPATCH get_FontHeader(long BandNumber)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x85, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, BandNumber);
		return result;
	}
	void putref_FontHeader(long BandNumber, LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_DISPATCH ;
		InvokeHelper(0x85, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	float get_FontWidthHeader(long BandNumber)
	{
		float result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9b, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_FontWidthHeader(long BandNumber, float newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R4 ;
		InvokeHelper(0x9b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	CString get_CellFontName()
	{
		CString result;
		InvokeHelper(0x4d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_CellFontName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x4d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_CellFontSize()
	{
		float result;
		InvokeHelper(0x4e, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_CellFontSize(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x4e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_CellFontBold()
	{
		BOOL result;
		InvokeHelper(0x4f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_CellFontBold(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x4f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_CellFontItalic()
	{
		BOOL result;
		InvokeHelper(0x50, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_CellFontItalic(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x50, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_CellFontUnderline()
	{
		BOOL result;
		InvokeHelper(0x51, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_CellFontUnderline(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x51, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_CellFontStrikeThrough()
	{
		BOOL result;
		InvokeHelper(0x52, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_CellFontStrikeThrough(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x52, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_CellFontWidth()
	{
		float result;
		InvokeHelper(0x53, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_CellFontWidth(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x53, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_TextStyle()
	{
		long result;
		InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_TextStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_TextStyleFixed()
	{
		long result;
		InvokeHelper(0x15, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_TextStyleFixed(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x15, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_TextStyleBand(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x97, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_TextStyleBand(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x97, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_TextStyleHeader(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_TextStyleHeader(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x9f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	BOOL get_ScrollTrack()
	{
		BOOL result;
		InvokeHelper(0x16, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ScrollTrack(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x16, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_FocusRect()
	{
		long result;
		InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_FocusRect(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x17, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_HighLight()
	{
		long result;
		InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_HighLight(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Redraw()
	{
		BOOL result;
		InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Redraw(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x19, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ScrollBars()
	{
		long result;
		InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ScrollBars(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_MouseRow()
	{
		long result;
		InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_MouseCol()
	{
		long result;
		InvokeHelper(0x1c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_CellLeft()
	{
		long result;
		InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_CellTop()
	{
		long result;
		InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_CellWidth()
	{
		long result;
		InvokeHelper(0x1f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_CellHeight()
	{
		long result;
		InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_RowHeightMin()
	{
		long result;
		InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RowHeightMin(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x21, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_FillStyle()
	{
		long result;
		InvokeHelper(DISPID_FILLSTYLE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_FillStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(DISPID_FILLSTYLE, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_GridLines()
	{
		long result;
		InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_GridLines(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x22, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_GridLinesFixed()
	{
		long result;
		InvokeHelper(0x23, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_GridLinesFixed(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x23, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_GridColor()
	{
		unsigned long result;
		InvokeHelper(0x24, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_GridColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x24, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_GridColorFixed()
	{
		unsigned long result;
		InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_GridColorFixed(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_GridColorUnpopulated()
	{
		unsigned long result;
		InvokeHelper(0xa7, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_GridColorUnpopulated(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0xa7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_GridColorBand(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x95, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridColorBand(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x95, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_GridColorHeader(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9d, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridColorHeader(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0x9d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_GridColorIndent(long BandNumber)
	{
		unsigned long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa3, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridColorIndent(long BandNumber, unsigned long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_UI4 ;
		InvokeHelper(0xa3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	unsigned long get_CellBackColor()
	{
		unsigned long result;
		InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_CellBackColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_CellForeColor()
	{
		unsigned long result;
		InvokeHelper(0x27, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_CellForeColor(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x27, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_CellAlignment()
	{
		short result;
		InvokeHelper(0x28, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_CellAlignment(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x28, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_CellTextStyle()
	{
		long result;
		InvokeHelper(0x29, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_CellTextStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x29, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_CellPictureAlignment()
	{
		short result;
		InvokeHelper(0x2b, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_CellPictureAlignment(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x2b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Clip()
	{
		CString result;
		InvokeHelper(0x2d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Clip(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x2d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void put_Sort(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x2e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_SelectionMode()
	{
		long result;
		InvokeHelper(0x2f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_SelectionMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_MergeCells()
	{
		long result;
		InvokeHelper(0x30, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MergeCells(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x30, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_AllowBigSelection()
	{
		BOOL result;
		InvokeHelper(0x33, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_AllowBigSelection(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x33, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_AllowUserResizing()
	{
		long result;
		InvokeHelper(0x34, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_AllowUserResizing(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x34, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BorderStyle()
	{
		long result;
		InvokeHelper(DISPID_BORDERSTYLE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BorderStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(DISPID_BORDERSTYLE, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_hWnd()
	{
		long result;
		InvokeHelper(DISPID_HWND, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL get_Enabled()
	{
		BOOL result;
		InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Enabled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Appearance()
	{
		long result;
		InvokeHelper(DISPID_APPEARANCE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Appearance(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(DISPID_APPEARANCE, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_MousePointer()
	{
		long result;
		InvokeHelper(0xfffffdf7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MousePointer(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xfffffdf7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_MouseIcon()
	{
		LPDISPATCH result;
		InvokeHelper(0xfffffdf6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void putref_MouseIcon(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(0xfffffdf6, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	long get_PictureType()
	{
		long result;
		InvokeHelper(0x32, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_PictureType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x32, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Picture()
	{
		LPDISPATCH result;
		InvokeHelper(0xfffffdf5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_CellPicture()
	{
		LPDISPATCH result;
		InvokeHelper(0x2a, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void putref_CellPicture(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(0x2a, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	CString get_TextArray(long Index)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x37, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, Index);
		return result;
	}
	void put_TextArray(long Index, LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x37, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	short get_ColAlignment(long Index)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x38, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Index);
		return result;
	}
	void put_ColAlignment(long Index, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x38, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	short get_ColAlignmentFixed(long Index)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x31, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Index);
		return result;
	}
	void put_ColAlignmentFixed(long Index, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x31, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	short get_ColAlignmentBand(long BandNumber, long BandColIndex)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x98, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void put_ColAlignmentBand(long BandNumber, long BandColIndex, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0x98, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, BandColIndex, newValue);
	}
	short get_ColAlignmentHeader(long BandNumber, long BandColIndex)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xa0, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void put_ColAlignmentHeader(long BandNumber, long BandColIndex, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0xa0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, BandColIndex, newValue);
	}
	long get_ColWidth(long Index, long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x39, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index, BandNumber);
		return result;
	}
	void put_ColWidth(long Index, long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x39, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, BandNumber, newValue);
	}
	long get_RowHeight(long Index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index);
		return result;
	}
	void put_RowHeight(long Index, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x3a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	BOOL get_MergeRow(long Index)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, Index);
		return result;
	}
	void put_MergeRow(long Index, BOOL newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x3b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	BOOL get_MergeCol(long Index)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, Index);
		return result;
	}
	void put_MergeCol(long Index, BOOL newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x3c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	void put_RowPosition(long Index, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x3d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	void put_ColPosition(long Index, long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x3e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, BandNumber, newValue);
	}
	long get_RowData(long Index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index);
		return result;
	}
	void put_RowData(long Index, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x3f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	long get_ColData(long Index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x40, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index);
		return result;
	}
	void put_ColData(long Index, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x40, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	CString get_TextMatrix(long Row, long Col)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x41, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, Row, Col);
		return result;
	}
	void put_TextMatrix(long Row, long Col, LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x41, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Row, Col, newValue);
	}
	void AddItem(LPCTSTR Item, VARIANT Index)
	{
		static BYTE parms[] = VTS_BSTR VTS_VARIANT ;
		InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Item, &Index);
	}
	void RemoveItem(long Index)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Index);
	}
	void Clear()
	{
		InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Refresh()
	{
		InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ClearStructure()
	{
		InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	LPUNKNOWN get_DataSource()
	{
		LPUNKNOWN result;
		InvokeHelper(0x4c, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	void putref_DataSource(LPUNKNOWN newValue)
	{
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x4c, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_DataMember()
	{
		CString result;
		InvokeHelper(0x86, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_DataMember(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x86, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_RowIsVisible(long Index)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x55, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, Index);
		return result;
	}
	BOOL get_ColIsVisible(long Index)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x56, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, Index);
		return result;
	}
	long get_RowPos(long Index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x57, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index);
		return result;
	}
	long get_ColPos(long Index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x58, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Index);
		return result;
	}
	short get_GridLineWidth()
	{
		short result;
		InvokeHelper(0x59, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_GridLineWidth(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x59, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_GridLineWidthFixed()
	{
		short result;
		InvokeHelper(0x9a, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_GridLineWidthFixed(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x9a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_GridLineWidthUnpopulated()
	{
		short result;
		InvokeHelper(0xa8, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	void put_GridLineWidthUnpopulated(short newValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xa8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	short get_GridLineWidthBand(long BandNumber)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x96, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLineWidthBand(long BandNumber, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x96, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	short get_GridLineWidthHeader(long BandNumber)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9e, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLineWidthHeader(long BandNumber, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x9e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	short get_GridLineWidthIndent(long BandNumber)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa4, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLineWidthIndent(long BandNumber, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0xa4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	short get_FixedAlignment(long Index)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5a, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Index);
		return result;
	}
	void put_FixedAlignment(long Index, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x5a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	CString get_FontName()
	{
		CString result;
		InvokeHelper(0x5b, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_FontName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_FontSize()
	{
		float result;
		InvokeHelper(0x5c, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_FontSize(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x5c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_FontBold()
	{
		BOOL result;
		InvokeHelper(0x5d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_FontBold(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x5d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_FontItalic()
	{
		BOOL result;
		InvokeHelper(0x5e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_FontItalic(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x5e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_FontStrikethru()
	{
		BOOL result;
		InvokeHelper(0x5f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_FontStrikethru(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x5f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_FontStrikeThrough()
	{
		BOOL result;
		InvokeHelper(0x87, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_FontStrikeThrough(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x87, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_FontUnderline()
	{
		BOOL result;
		InvokeHelper(0x60, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_FontUnderline(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x60, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_RightToLeft()
	{
		BOOL result;
		InvokeHelper(0xfffffd9d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_RightToLeft(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xfffffd9d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Recordset()
	{
		LPDISPATCH result;
		InvokeHelper(0x69, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void putref_Recordset(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(0x69, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	VARIANT get_Rowset()
	{
		VARIANT result;
		InvokeHelper(0x6d, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
		return result;
	}
	void putref_Rowset(VARIANT newValue)
	{
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x6d, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, &newValue);
	}
	long get_ColHeader(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x71, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_ColHeader(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x71, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	CString get_ColHeaderCaption(long BandNumber, long BandColIndex)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x72, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void put_ColHeaderCaption(long BandNumber, long BandColIndex, LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x72, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, BandColIndex, newValue);
	}
	long BandColIndex()
	{
		long result;
		InvokeHelper(0x73, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_BandData(long BandData)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandData);
		return result;
	}
	void put_BandData(long BandData, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x76, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandData, newValue);
	}
	long get_BandDisplay()
	{
		long result;
		InvokeHelper(0x77, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BandDisplay(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x77, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_BandExpandable(long BandNumber)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x78, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_BandExpandable(long BandNumber, BOOL newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x78, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_GridLinesBand(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLinesBand(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x7c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_GridLinesHeader(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLinesHeader(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x9c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_GridLinesIndent(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_GridLinesIndent(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xa2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_GridLinesUnpopulated()
	{
		long result;
		InvokeHelper(0xa6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_GridLinesUnpopulated(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BandIndent(long BandNumber)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, BandNumber);
		return result;
	}
	void put_BandIndent(long BandNumber, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, newValue);
	}
	long get_BandLevel()
	{
		long result;
		InvokeHelper(0x7e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_Bands()
	{
		long result;
		InvokeHelper(0x7f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_CellType()
	{
		long result;
		InvokeHelper(0x80, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_RowSizingMode()
	{
		long result;
		InvokeHelper(0x82, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RowSizingMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x82, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_DataField(long BandNumber, long BandColIndex)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x81, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void CollectBandFieldNameInfo(long force_collection)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60020109, DISPATCH_METHOD, VT_EMPTY, NULL, parms, force_collection);
	}
	void GetBandName(long BandNumber, BSTR * BandName)
	{
		static BYTE parms[] = VTS_I4 VTS_PBSTR ;
		InvokeHelper(0x6002010a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber, BandName);
	}
	void CountColumnsInBand(long BandNumber, long * Cols)
	{
		static BYTE parms[] = VTS_I4 VTS_PI4 ;
		InvokeHelper(0x6002010b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber, Cols);
	}
	void GetColumnInBandInfo(long BandNumber, long UIIndex, BSTR * ColName, BSTR * Caption, BOOL * Hidden, long * RowsetColIndex, short * Alignment)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PBSTR VTS_PBSTR VTS_PBOOL VTS_PI4 VTS_PI2 ;
		InvokeHelper(0x6002010c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber, UIIndex, ColName, Caption, Hidden, RowsetColIndex, Alignment);
	}
	void SetColumnInBandInfo(long BandNumber, long UIIndex, LPWSTR ColName, LPWSTR Caption, BOOL Hidden, long RowsetColIndex, short Alignment)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_UNKNOWN VTS_UNKNOWN VTS_BOOL VTS_I4 VTS_I2 ;
		InvokeHelper(0x6002010d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber, UIIndex, ColName, Caption, Hidden, RowsetColIndex, Alignment);
	}
	void ApplyBandFieldNameInfoChanges()
	{
		InvokeHelper(0x6002010e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL get_RowExpandable()
	{
		BOOL result;
		InvokeHelper(0x88, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	BOOL get_RowExpanded()
	{
		BOOL result;
		InvokeHelper(0x89, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_RowExpanded(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x89, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void CollapseAll(long BandNumber)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber);
	}
	void ExpandAll(long BandNumber)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, BandNumber);
	}
	short get_ColWordWrapOption(long Index)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4b, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Index);
		return result;
	}
	void put_ColWordWrapOption(long Index, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x4b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	short get_ColWordWrapOptionBand(long BandNumber, long BandColIndex)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xa9, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void put_ColWordWrapOptionBand(long BandNumber, long BandColIndex, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0xa9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, BandColIndex, newValue);
	}
	short get_ColWordWrapOptionHeader(long BandNumber, long BandColIndex)
	{
		short result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xaa, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, BandNumber, BandColIndex);
		return result;
	}
	void put_ColWordWrapOptionHeader(long BandNumber, long BandColIndex, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0xaa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, BandNumber, BandColIndex, newValue);
	}
	short get_ColWordWrapOptionFixed(long Index)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xab, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Index);
		return result;
	}
	void put_ColWordWrapOptionFixed(long Index, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0xab, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	long get_OLEDropMode()
	{
		long result;
		InvokeHelper(0x60f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_OLEDropMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void OLEDrag()
	{
		InvokeHelper(0x610, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// Properties
	//



};

/////////////////////////////////////////////////////////////////////////////////////////////
// CNHFlexGrid
#define SHESTGRID_DOC_VERSION		1		// 网格配置信息文件版本号
// 序号列的宽度
#define SHESTGRID_SN_WIDTH			550		// 
// 标题行的高度
#define SHESTGRID_HEADER_HEIGHT		260		// 
// 最小行高
#define SHESTGRID_ROW_HEIGHTMIN		220		// 
// 单行选中时的前景色
#define SHESTGRID_ROWSEL_FORECOLOR	0xAC6A00
// 
#define DEF_TEXT_SELECT			L"√"
/*
// Below styles MUST be present in a report ctrl
#define MUST_STYLE			(WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT)
#define MUST_EX_STYLE		(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES)

// Below styles MUST NOT be present in a report ctrl
#define MUST_NOT_STYLE		(LVS_EDITLABELS | LVS_ICON | LVS_SMALLICON | LVS_LIST | LVS_NOSCROLL)
#define MUST_NOT_EX_STYLE	(0)
*/
class CNHFlexGrid : public CNMSHFGrid
{
	DECLARE_DYNAMIC(CNHFlexGrid)
public:
	// 行选中模式
	typedef enum {
		BEGridSelectFlagRowLight	= 0,	// 行选中时以底色区分
		BEGridSelectFlagRowSN				// 行选中时以粗体字+序号含“●”字符以区分
	}BEGridSelectFlag;
public:
	CNHFlexGrid();
	virtual ~CNHFlexGrid();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	// 属性
	bool haveSN(void) const {							return m_bHaveSN;						}
	void enableSN(bool vhad) {							m_bHaveSN = vhad;						}
	BEGridSelectFlag getSelectFlag(void) const {		return m_euSelectFlag;					}
	void setSelectFlag(BEGridSelectFlag vrowsel) {
		m_euSelectFlag = vrowsel;
		if(this->GetSafeHwnd() == NULL) return;
		switch(m_euSelectFlag)
		{
		case BEGridSelectFlagRowLight:
			{
				this->put_FocusRect(flexFocusNone);
				this->put_SelectionMode(flexSelectionByRow);
				this->put_HighLight(flexHighlightAlways);
			}
			break;
		case BEGridSelectFlagRowSN:
			{
				this->put_FocusRect(flexFocusNone);
				this->put_SelectionMode(flexSelectionFree);
				this->put_HighLight(flexHighlightNever);
			}
			break;
		}
	}
	bool haveParentHeader(void) const {
		for(std::size_t idx = 0; idx < this->m_arrHeadinfo.size(); idx++)
		{
			gtc::gridColHeader* pheader = m_arrHeadinfo[idx];
			if(pheader != NULL)
			{
				if(!pheader->getParentName().empty() && pheader->getParentName() != pheader->getName())
					return true;
			}
		}
		return false;
	}
	std::wstring getFromValue(void) const {					return m_strFromValue;									}
	void setFromValue(const wchar_t *vpfromvalue) {			m_strFromValue = vpfromvalue?vpfromvalue:L"";			}
	std::wstring getFieldsValue(void) const {				return m_strFieldsValue;								}
	//void SetFieldsValue(LPCTSTR vpfields) {				m_strFieldsValue = vpfields;							}
	std::wstring getSortValue(void) const {					return m_strSortValue;									}
	void setSortValue(const wchar_t *vpsortvalue) {			m_strSortValue = vpsortvalue?vpsortvalue:L"";			}
	void setSortValue(const std::wstring &vpsortvalue) {			m_strSortValue = vpsortvalue;					}
	std::wstring getWhereValue(void) const {					return m_strWhereValue;								}
	void setWhereValue(const wchar_t *vpwherevalue) {			m_strWhereValue = vpwherevalue?vpwherevalue:L"";	}
	void setWhereValue(const std::wstring &vpwherevalue) {			m_strWhereValue = vpwherevalue;					}
	std::wstring getIDFieldName(void) const {				return m_strIDFldName;									}
	void setIDFieldName(const wchar_t *vpkeyfield) {			
		m_strIDFldName = vpkeyfield?vpkeyfield:L"";
		//std::wstring sname = m_strIDFldName;
		//gtc::convert::toUpper(sname);
		//m_mapHeadIndex[sname] = doGetHeadIndex(m_strFieldsValue);

		m_lIDFldIndex = -1;
		m_bRefreshFieldIndex = true;
	}
	std::map<std::wstring,int32_t>& getMapHeadindex() {		return m_mapHeadIndex;				}
	long getRow(void) {									return get_Row();						}
	long getCol(void) {									return get_Col();						}
	long getRows(void) {								return get_Rows();						}
	long getCols(void) {								return get_Cols(0);						}
	long getFixedRows(void) {							return get_FixedRows();					}
	long getFixedCols(void) {							return get_FixedCols();					}
	long getRowID(long vrow) {
		if(vrow >= this->get_FixedRows() && vrow < get_Rows())
			return this->get_RowData(vrow);
		return -1;
	}
	long getTopRow(void) {								return get_TopRow();					}
	void setTopRow(long vtop) {							put_TopRow(vtop);						}
	long getRowHeight(void) const {						return m_lRowHeight;					}
	void setRowHeight(long vheight) {					m_lRowHeight = vheight;					}
	std::wstring getTextMatrix(long row, long col) {			return this->get_TextMatrix(row, col).GetBuffer();	}
public:
	// 公共方法
	// 初始化标题及网格信息
	bool doInitHeader(void);
	// 清除数据
	void doClearData(void);
	// 清除数据及网格信息
	void doClear(void);
	// 选中行
	void doSelectedRow(long vrow);
	long doSelectedRowByRowData(long vrowdata);
	// 
	// 判断是否有网格数据
	bool doIsEmpty(void);
	// 追加新行,并返回新行索引
	long doAddRow(void) {
		if(!this->doIsEmpty())
			this->put_Rows(this->get_Rows()+1);
		return this->get_Rows()-1;
	}
	// 得到查询语句
	CString doGetSqlSelectString(LPCTSTR vwhere = NULL, LPCTSTR vsort = NULL);
	// 设定网格数据
	long doSetData(LPDISPATCH vpdata);
	long doSetData(db::dataSet *pdataSet);
#ifdef _USED_OFFICE_
	// 导出数据，并生成excel文件
	void doOutData(LPCTSTR vfilepath = NULL, LPCTSTR vphead = NULL, LPCTSTR vpoffal = NULL);
#endif
	// 追加移行数据
	//void doAppendData(CMap<CString,LPCTSTR,COleVariant*,COleVariant*> vmapdata);
	// 增加列信息；Key=字段名，Value=字段值
	gtc::gridColHeader* doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, const wchar_t *vpidfldname, BEDataType vtype, BEAlignMode valign, int32_t vwidth = 1000, const wchar_t *vpparname = nullptr);
	// 增加列信息（简版）；字段对齐方式根据数据类型来定
	gtc::gridColHeader* doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, BEDataType vtype, int32_t vwidth = 1000);
	gtc::gridColHeader* doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, const wchar_t *vpidfldname, BEDataType vtype, int32_t vwidth = 1000);
	// 增加颜色列
	//CBRangeItemAdmin* doAddColor(LPCTSTR vpfieldname, long vmin, long vmax, long vcolor);
	// 添加附加字段，格式为：“值 AS 字段名”，该字段名必须存在于m_arrHeadinfo中
	void doAddAdditionalField(const wchar_t *vpfldformat);
	// 根据列索引得到列头信息
	gtc::gridColHeader* doGetHeaderinfo(long vcol) {
		if(vcol >= this->get_FixedCols() && vcol < this->get_Cols(0))
		{
			return this->m_arrHeadinfo[vcol - this->get_FixedCols()];
		}
		return NULL;
	}
	// 设定网格内容
	void doSetCellValue(int32_t vrow, int32_t vcol, COleVariant& vval);
	void doSetCellValue(int32_t vrow, int32_t vcol, gtc::variant var);
	void doSetCellValue(int32_t vrow, const wchar_t *vpfldname, COleVariant& vval);
	void doSetCellValue(int32_t vrow, const wchar_t *vpfldname, bool vval) {			this->doSetCellValue(vrow, vpfldname, int(vval));		}
	void doSetCellValue(int32_t vrow, const wchar_t *vpfldname, int vval);
	void doSetCellValue(int32_t vrow, const wchar_t *vpfldname, float vval);
	void doSetCellValue(int32_t vrow, const wchar_t *vpfldname, const wchar_t *vval);
	// 调用者负责释放返回变量的内存
	bool doGetCellValue(int32_t vrow, int32_t vcol, COleVariant& vvar);
	bool doGetCellValue(int32_t vrow, const wchar_t *vpfldname, COleVariant &var);
	std::wstring doGetCellValue(int32_t vrow, int32_t vcol);
	std::wstring doGetCellValue(int32_t vrow, const wchar_t *vpfldname);
	bool doIsChecked(int32_t vrow, const wchar_t *vpfldname) {
		COleVariant var;
		if(doGetCellValue(vrow, vpfldname, var) && var.vt > 1)
		{
			var.ChangeType(VT_I4);
			return var.lVal != 0;
		}
		return false;
	}
	// 移动数据行，visnext=true下移，false上移，并返回移动后的数据行
	int32_t doMoveRowData(int32_t vrow, bool visnext);
	//  将控件vctrlid移到cell（vrow,vcol）处
	void doMoveControlToCell(CWnd* vpwnd, UINT vctrlid, int32_t vrow, int32_t vcol);
	// 查找数据，并返回数据行
	int32_t doFindValue(int32_t vid);
	// 查看指定列中的数据是否已存在
	bool doExistsValue(const wchar_t *pfldname, const wchar_t *pvalue);
private:
	// static 
	// BOOL型图片 
	static bool mc_havePicture;
	static CPictureHolder mc_picSelect, mc_picOnly, mc_picUnSelect;
	// 为了实现标题行的列合并，不得已在奇偶行的数据中插入一个空格
	static std::wstring doCorrectCellText(const wchar_t *vpcell, const gtc::gridColHeader* vpheader, bool visodd) {
		if(vpheader == nullptr || visodd || vpheader->isMerged())
			return vpcell;
		std::wstring cell;
		switch(vpheader->getAlign())
		{
		case BEAlignModeAllLeft:			//= DT_LEFT|DT_SINGLELINE|DT_VCENTER,
		case BEAlignModeLeftTop:			//= DT_LEFT|DT_TOP,			// 图片顶部左对齐
		case BEAlignModeLeftCenter:			//= DT_LEFT|DT_VCENTER,		// 图片中间左对齐
		case BEAlignModeLeftBottom:			//= DT_LEFT|DT_BOTTOM,		// 
			cell = std::wstring(vpcell) + L" ";
			break;
		case BEAlignModeAllRight:			//= DT_RIGHT|DT_SINGLELINE|DT_VCENTER,
		case BEAlignModeRightTop:			//= DT_RIGHT|DT_TOP,		// 图片顶部右对齐
		case BEAlignModeRightCenter:		//= DT_RIGHT|DT_VCENTER,	// 图片中间右对齐
			//case BEAlignModeRightBottom:		//= DT_RIGHT				// 图片底部右对齐
			cell = std::wstring(L" ") + vpcell;
			break;
		case BEAlignModeAllCenter:			//= DT_CENTER|DT_SINGLELINE|DT_VCENTER,
		case BEAlignModeCenterTop:			//= DT_CENTER|DT_TOP,		// 图片顶部居中
			//case BEAlignModeCenterCenter:		//= DT_CENTER,				// 图片中间居中
		case BEAlignModeCenterBottom:		//= DT_CENTER|DT_BOTTOM,	// 图片底部居中
			cell = std::wstring(L" ") + vpcell;
			cell += L" ";
			break;
		}
		return cell;
	}
private:
	// 根据字段名得到其在m_arrHeadinfo中的索引
	int32_t doGetHeadIndex(std::wstring vfieldname) const{
		//vfieldname.MakeUpper();
		//long idx = -1;
		//m_mapHeadIndex.Lookup(vfieldname, idx);
		//return idx;
		gtc::convert::toUpper(vfieldname);
		std::map<std::wstring, int32_t>::const_iterator iter = m_mapHeadIndex.find(vfieldname);
		if(iter != m_mapHeadIndex.cend())
			return iter->second;
		return -1;
	}
	// 根据字段名得到其在记录集中的索引
	int32_t doGetFieldIndex(const std::wstring &vfieldname) const{
		//vfieldname.MakeUpper();
		//long idx = -1;
		//if(m_mapHeadIndex.Lookup(vfieldname, idx) && idx >= 0 && idx < m_arrHeadinfo.GetCount())
		//{
		//	gtc::gridColHeader* phead = m_arrHeadinfo[idx];
		//	if(phead != NULL)
		//	{
		//		if(vfieldname.CompareNoCase(phead->GetFieldName()) == 0)
		//			idx = phead->GetFieldIndex();
		//		else if(vfieldname.CompareNoCase(phead->GetIDFieldName()) == 0)
		//			idx = phead->GetIDFieldIndex();
		//	}
		//}
		//return idx;
		int32_t idx = -1;
		int32_t headidx = doGetHeadIndex(vfieldname);
		if (headidx >= 0 && headidx < m_arrHeadinfo.size())
		{
			gtc::gridColHeader* phead = m_arrHeadinfo[headidx];
			if (phead != nullptr)
			{
				if(phead->getFieldName() == vfieldname)
					idx = phead->getFieldIndex();
				if(phead->getIDFieldName() == vfieldname)
					idx = phead->getIDFieldIndex();
			}
		}
		return idx;
	}
private:
	// 头信息及外观项
	std::wstring					m_strFileName;				// 网格配置信息文件的文件名，文件保存在AppPath\Headinfo\*.ssg
	uint32_t						m_dwFileVersion;			// 文件版本号
	gtc::gridColHeader::PtrArrayType	m_arrHeadinfo;			// 网格标题，m_strFldName和m_strIDFldName在本数组中具有唯一性

	std::map<std::wstring,int32_t>	m_mapHeadIndex;	// m_strFldName或m_strIDFldName名称为Key(字段名全部转换到大写)，其所在m_arrHeadinfo的索引为Value
	// 颜色采用友元函数控制
	//CARangeItemAdmin			m_admColors;				// 条件值对应的单元格前景色（字段名已经存在于m_arrHeadinfo中），key=字段名m_strFldName或m_strIDFldName，value=CBRangeItemAdmin*
	std::vector<std::wstring>		m_arrAdditionalField;		// 附加字段的定义；如：值 AS 字段名，该字段名必须存在于m_arrHeadinfo中
	bool				m_bHaveSN;					// 是否有行序号
	BEGridSelectFlag	m_euSelectFlag;				// 行选择方式
	// 是否可编辑
	bool				m_bIsWrited;				// 控件是否可编辑；默认 FALSE
	int32_t				m_lWriteRows;				// 可编辑行数；默认 15
	// 记录集查询支持
	std::wstring		m_strFromValue;				// from后的表名或视图名等
	std::wstring		m_strFieldsValue;			// 被筛选的字段 格式如：f1,f2,f3
	std::wstring		m_strSortValue;				// SQL排 序
	std::wstring		m_strWhereValue;			// SQL条 件
	// 
	int32_t				m_lIDFldIndex;				// 记录的主键字段名在记录集中的索引，是整型值
	std::wstring		m_strIDFldName;				// 记录的主键字段名，是整型值
	// 是否可查找，即：通过查找数据库的到列值
	// 系统采用int型索引而非string型索引遍历数据记录集
	// 辅助功能
	int32_t				m_lOldRow;					// 老的行
	// 是否需要重新生成字段索引值（m_lKeyFldName，gtc::gridColHeader.m_lFldIndex，gtc::gridColHeader.m_lIDFldIndex）
	bool				m_bRefreshFieldIndex;		// 

	// 控件是否有数据，内部用
	bool				m_bIsEmptyData;
	// 数据行的高度
	int32_t				m_lRowHeight;
};

}