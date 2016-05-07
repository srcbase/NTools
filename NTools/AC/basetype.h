#pragma once
#include "AC/basedef.h"
#include "AC/datetime.h"
namespace gtc {

//////////////////////////////////////////////////////////////////////////
// 错误码
typedef enum BEVariantSCode {
	BEVariantSCodeOk				= 0,			// 
	BEVariantSCodeNilPointer		= 0x00001000	// 
}BEVariantSCode;
//////////////////////////////////////////////////////////////////////////
// 
typedef struct BSVariant {
	BEDataType	dataType;		// 数据类型
	bool		bFreePtr;		// 标示 BSVariant中指针类型按new（true）还是引用指针（false）
	union {
		long			scode;		// BEDataTypeNone BEDataTypeNULL 错误码	
		uint8_t			nVal;		// BEDataTypeByte		
		bool			bVal;		// BEDataTypeBoolean		
		std::wstring	*pwsVal;	// BEDataTypeString		
		int16_t			iVal;		// BEDataTypeInt16		
		int32_t			lVal;		// BEDataTypeInt32		
		int64_t			llVal;		// BEDataTypeInt64		
		float			fltVal;		// BEDataTypeFloat		
		double			dblVal;		// BEDataTypeDouble		
		std::time_t		tmVal;		// BEDataTypeDate BEDataTypeTime BEDataTypeDatetime		
		SBDataType		*pbinaryVal;// BEDataTypeImage		
		SWDataType		*ptextVal;	// BEDataTypeText		
	};
}BSVariant;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	variant类	2014-09-14
//	变体类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class variant : public BSVariant
{
public:
	static const variant null;
public:
	variant(BEDataType dataType = BEDataTypeNone);
	variant(const BSVariant	&varSrc);
	variant(const BSVariant *pvarSrc);
	variant(const variant &varSrc);

	variant(PCWSTR pstrSrc);
	variant(std::wstring &wsSrc, bool bInAlloc = true);
	variant(const std::wstring &wsSrc);

	variant(uint8_t nSrc);
	variant(bool bSrc);
	variant(int16_t iSrc);
	variant(int32_t lSrc);
	variant(int64_t llSrc, BEDataType type = BEDataTypeInt64);

	variant(float fltSrc);
	variant(double dblSrc);

	//variant(std::time_t dtSrc);
	variant(const std::tm &dtSrc);
	variant(const gtc::datetime &dtSrc);

	variant(SBDataType &dataSrc, bool bInAlloc = true);
	variant(SWDataType &textSrc, bool bInAlloc = true);
	variant(const SBDataType &dataSrc);
	variant(const SWDataType &textSrc);

	~variant();
public:
	void clear();
	void changeType(BEDataType dataType);
	bool isEmpty() const;
	bool isNull() const;
	// 字节数或字符数或数组维度
	std::size_t size() const;

	bool operator==(const variant &varSrc) const;
	bool operator!=(const variant &varSrc) const;
	//bool operator==(const BSVariant *pvarSrc) const;

public:
	variant& operator=(const BSVariant &varSrc);
	variant& operator=(const BSVariant *pvarSrc);
	variant& operator=(const variant &varSrc);

	variant& operator=(PCWSTR pstrSrc);
	variant& operator=(std::wstring &wsSrc);
	variant& operator=(const std::wstring &wsSrc);
	variant& operator=(std::wstring *pwsSrc);

	variant& operator=(uint8_t nSrc);
	variant& operator=(int16_t iSrc);
	variant& operator=(int32_t lSrc);
	variant& operator=(int64_t llSrc);

	variant& operator=(float fltSrc);
	variant& operator=(double dblSrc);

	//const variant& operator=(std::time_t dtSrc);
	variant& operator=(std::tm &dtSrc);
	variant& operator=(const gtc::datetime &dtSrc);

	variant& operator=(SBDataType &dataSrc);
	variant& operator=(SWDataType &textSrc);
	variant& operator=(const SBDataType &dataSrc);
	variant& operator=(const SWDataType &textSrc);
	variant& operator=(SBDataType *pdataSrc);
	variant& operator=(SWDataType *ptextSrc);

};

// 初始化
static void variantInit(BSVariant *pvarSrc);
// 
static bool variantClear(BSVariant *pvarSrc);
// 
static bool variantCopy(BSVariant *pDest, const BSVariant *pSrc);
// 
static bool variantChangeType(BSVariant *pDest, BEDataType dataType);
}