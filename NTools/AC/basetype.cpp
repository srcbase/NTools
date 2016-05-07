#include "basetype.h"
#include "converter.h"
#include "base64.h"
namespace gtc {

const variant variant::null;
variant::variant(BEDataType datatype /*= BEDataTypeNone*/)
{
	variantInit(this);
	dataType = datatype;
}
variant::variant(const BSVariant &varSrc)
{
	variantInit(this);
	variantCopy(this, &varSrc);
}
variant::variant(const BSVariant *pvarSrc)
{
	variantInit(this);
	variantCopy(this, pvarSrc);
}
variant::variant(const variant &varSrc)
{
	variantInit(this);
	variantCopy(this, &varSrc);
}

variant::variant(PCWSTR pstrSrc)
{
	dataType = BEDataTypeString;
	bFreePtr = true;
	pwsVal = NULL;
	if(pstrSrc)
		pwsVal = new std::wstring(pstrSrc);
}
variant::variant(std::wstring &wsSrc, bool bInAlloc /*= true*/)
{
	dataType = BEDataTypeString;
	bFreePtr = bInAlloc;
	if(bFreePtr)
		pwsVal = new std::wstring(wsSrc);
	else
		pwsVal = &wsSrc;
}

variant::variant(const std::wstring &wsSrc)
{
	dataType = BEDataTypeString;
	bFreePtr = true;
	pwsVal = new std::wstring(wsSrc);

}
variant::variant(bool bSrc)
{
	dataType = BEDataTypeBoolean;
	bFreePtr = false;
	bVal = bSrc;
}
variant::variant(uint8_t nSrc)
{
	dataType = BEDataTypeByte;
	bFreePtr = false;
	nVal = nSrc;
}
variant::variant(int16_t iSrc)
{
	dataType = BEDataTypeInt16;
	bFreePtr = false;
	iVal = iSrc;
}
variant::variant(int32_t lSrc)
{
	dataType = BEDataTypeInt32;
	bFreePtr = false;
	lVal = lSrc;

}
variant::variant(int64_t llSrc, BEDataType type /*= BEDataTypeInt64*/)
{
	assert(type == BEDataTypeInt64 || type == BEDataTypeDate ||
		type == BEDataTypeTime || type == BEDataTypeDatetime);

	dataType = type;
	bFreePtr = false;
	llVal = llSrc;
}

variant::variant(float fltSrc)
{
	dataType = BEDataTypeFloat;
	bFreePtr = false;
	fltVal = fltSrc;
}
variant::variant(double dblSrc)
{
	dataType = BEDataTypeDouble;
	bFreePtr = false;
	dblVal = dblSrc;
}

//variant::variant(std::time_t dtSrc)
//{
//
//}
variant::variant(const std::tm &dtSrc)
{
	dataType = BEDataTypeDatetime;
	bFreePtr = false;
	std::tm tt = dtSrc;
	tmVal = std::mktime(&tt);
}
variant::variant(const gtc::datetime &dtSrc)
{
	if(dtSrc.hour() == 0 && dtSrc.minute() == 0 && dtSrc.second() == 0)
		dataType = BEDataTypeDate;
	else
		dataType = BEDataTypeDatetime;
	bFreePtr = false;
	tmVal = dtSrc.timeStamp().epochTime();
}
variant::variant(SBDataType &dataSrc, bool bInAlloc /*= true*/)
{
	dataType = BEDataTypeImage;
	bFreePtr = bInAlloc;
	if(bFreePtr)
		pbinaryVal = new SBDataType(dataSrc);
	else
		pbinaryVal = &dataSrc;
}
variant::variant(SWDataType &textSrc, bool bInAlloc /*= true*/)
{
	dataType = BEDataTypeText;
	bFreePtr = bInAlloc;
	if(bFreePtr)
		ptextVal = new SWDataType(textSrc);
	else
		ptextVal = &textSrc;
}
variant::variant(const SBDataType &dataSrc)
{
	dataType = BEDataTypeImage;
	bFreePtr = true;
	pbinaryVal = new SBDataType(dataSrc);
}
variant::variant(const SWDataType &textSrc)
{
	dataType = BEDataTypeText;
	bFreePtr = true;
	ptextVal = new SWDataType(textSrc);
}
variant::~variant()
{
	clear();
}
void variant::clear()
{
	variantClear(this);
}
void variant::changeType(BEDataType dataType)
{
	variantChangeType(this, dataType);
}
bool variant::isNull() const
{
	return *this == null;
}
bool variant::isEmpty() const
{
	switch(dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		return true;
		break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		if(pwsVal == nullptr) return true;
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		if(!pbinaryVal) return true;
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		if(!ptextVal) return true;
		break;
	default:
		assert(false);
		break;
	}
	return false;
}

std::size_t variant::size() const
{
	switch(dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		return 0;
		break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		return 1;
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		if(pwsVal == nullptr) return 0;
		else return pwsVal->length();
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
		return 2;
		break;
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
		return 4;
		break;
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
		return 8;
		break;
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
		return 4;
		break;
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
		return 8;
		break;
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		return sizeof(std::time_t);
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		if(!pbinaryVal) return 0;
		else return pbinaryVal->size();
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		if(!ptextVal) return 0;
		else return ptextVal->size();
		break;
	default:
		assert(false);
		break;
	}
	return 0;
}
bool variant::operator==(const variant &varSrc) const
{
	if(&varSrc == this) return true;
	if(varSrc.dataType != dataType) return false;
	switch(dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		return true;
		break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
		return varSrc.nVal == nVal;
		break;
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		return varSrc.bVal == bVal;
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		return (varSrc.pwsVal == nullptr && pwsVal == nullptr) || 
			(varSrc.pwsVal && pwsVal && *varSrc.pwsVal == *pwsVal);
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
		return varSrc.iVal == iVal;
		break;
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
		return varSrc.lVal == lVal;
		break;
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
		return varSrc.llVal == llVal;
		break;
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
		return fabs(varSrc.fltVal - fltVal) <= 0.0001;
		break;
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
		return fabs(varSrc.dblVal - dblVal) <= 0.0001;
		break;
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		return varSrc.tmVal == tmVal;
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		return (varSrc.pbinaryVal == nullptr && pbinaryVal == nullptr) || 
			(varSrc.pbinaryVal && pbinaryVal && varSrc.pbinaryVal->size() == pbinaryVal->size() && std::equal(pbinaryVal->begin(), pbinaryVal->end(), varSrc.pbinaryVal->begin()));
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		return (varSrc.ptextVal == nullptr && ptextVal == nullptr) || 
			(varSrc.ptextVal && ptextVal && varSrc.ptextVal->size() == ptextVal->size() && std::equal(ptextVal->begin(), ptextVal->end(), varSrc.ptextVal->begin()));
		break;
	default:
		assert(false);
		break;
	}
	return false;
}
bool variant::operator!=(const variant &varSrc) const
{
	return !(*this == varSrc);
}
//bool variant::operator==(const BSVariant *pvarSrc) const
//{
//	if(!pvarSrc) return false;
//	return operator==(*pvarSrc);
//}
variant& variant::operator=(const BSVariant &varSrc)
{
	if (static_cast<BSVariant *>(this) != &varSrc)
	{
		variantCopy(this, &varSrc);
	}
	return *this;
}
variant& variant::operator=(const BSVariant *pvarSrc)
{
	if (static_cast<const BSVariant *>(this) != pvarSrc)
	{
		variantCopy(this, pvarSrc);
	}
	return *this;
}
variant& variant::operator=(const variant &varSrc)
{
	if (this != &varSrc)
	{
		variantCopy(this, &varSrc);
	}
	return *this;
}

variant& variant::operator=(PCWSTR pstrSrc)
{
	clear();

	dataType = BEDataTypeString;
	if(!pstrSrc)
		pwsVal = nullptr;
	else
	{
		pwsVal = new std::wstring();
		bFreePtr = true;
		*pwsVal = pstrSrc;
	}
	return *this;
}

variant& variant::operator=(std::wstring &wsSrc)
{
	clear();

	dataType = BEDataTypeString;
	pwsVal = new std::wstring(wsSrc);
	bFreePtr = true;
	return *this;
}

variant& variant::operator=(const std::wstring &wsSrc)
{
	clear();

	dataType = BEDataTypeString;
	pwsVal = new std::wstring(wsSrc);
	bFreePtr = true;
	return *this;
}
variant& variant::operator=(std::wstring *pwsSrc)
{
	clear();

	dataType = BEDataTypeString;
	pwsVal = pwsSrc;
	bFreePtr = false;
	return *this;

}
variant& variant::operator=(uint8_t nSrc)
{
	clear();

	dataType = BEDataTypeByte;
	nVal = nSrc;
	return *this;
}
variant& variant::operator=(int16_t iSrc)
{
	clear();

	dataType = BEDataTypeInt16;
	iVal = iSrc;
	return *this;
}
variant& variant::operator=(int32_t lSrc)
{
	clear();

	dataType = BEDataTypeInt32;
	lVal = lSrc;
	return *this;
}
variant& variant::operator=(int64_t llSrc)
{
	clear();

	dataType = BEDataTypeInt64;
	llVal = llSrc;
	return *this;
}

variant& variant::operator=(float fltSrc)
{
	clear();

	dataType = BEDataTypeFloat;
	fltSrc = fltSrc;
	return *this;
}
variant& variant::operator=(double dblSrc)
{
	clear();

	dataType = BEDataTypeDouble;
	dblVal = dblSrc;
	return *this;
}

//const variant& operator=(std::time_t dtSrc);
variant& variant::operator=(std::tm &dtSrc)
{
	clear();

	dataType = BEDataTypeDatetime;
	tmVal = std::mktime(&dtSrc);
	return *this;
}
variant& variant::operator=(const gtc::datetime &dtSrc)
{
	clear();

	if(dtSrc.hour() == 0 && dtSrc.minute() == 0 && dtSrc.second() == 0)
		dataType = BEDataTypeDate;
	else
		dataType = BEDataTypeDatetime;
	tmVal = dtSrc.timeStamp().epochTime();
	return *this;
}
variant& variant::operator=(SBDataType &dataSrc)
{
	clear();

	dataType = BEDataTypeImage;
	pbinaryVal = new SBDataType(dataSrc);
	bFreePtr = true;
	return *this;
}
variant& variant::operator=(SWDataType &textSrc)
{
	clear();

	dataType = BEDataTypeText;
	ptextVal = new SWDataType(textSrc);
	bFreePtr = true;
	return *this;
}
variant& variant::operator=(const SBDataType &dataSrc)
{
	clear();

	dataType = BEDataTypeImage;
	pbinaryVal = new SBDataType(dataSrc);
	bFreePtr = true;
	return *this;
}
variant& variant::operator=(const SWDataType &textSrc)
{
	clear();

	dataType = BEDataTypeText;
	ptextVal = new SWDataType(textSrc);
	bFreePtr = true;
	return *this;
}

variant& variant::operator=(SBDataType *pdataSrc)
{
	clear();

	dataType = BEDataTypeImage;
	pbinaryVal = pdataSrc;
	bFreePtr = false;
	return *this;

}
variant& variant::operator=(SWDataType *ptextSrc)
{
	clear();

	dataType = BEDataTypeText;
	ptextVal = ptextSrc;
	bFreePtr = false;
	return *this;
}
//////////////////////////////////////////////////////////////////////////
void variantInit(BSVariant *pvarSrc)
{
	assert(pvarSrc);
	if(!pvarSrc) return;
	memset(pvarSrc, 0, sizeof(*pvarSrc));
}


bool variantClear(BSVariant *pvarSrc)
{
	assert(pvarSrc);
	if(!pvarSrc) return false;
	switch(pvarSrc->dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		{
		}
		break;
	//case BEDataTypeNULL:		// VT_NULL
	//	{
	//	}
	//	break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
		{
			pvarSrc->nVal = 0;
		}
		break;
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		pvarSrc->bVal = false;
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		if(pvarSrc->pwsVal)
		{
			if(pvarSrc->bFreePtr) delete pvarSrc->pwsVal;
			pvarSrc->pwsVal = nullptr;
		}
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
		pvarSrc->iVal = 0;
		break;
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
		pvarSrc->lVal = 0;
		break;
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
		pvarSrc->llVal = 0;
		break;
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
		pvarSrc->fltVal = 0.0f;
		break;
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
		pvarSrc->dblVal = 0.0;
		break;
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		memset(&pvarSrc->tmVal, 0, sizeof(pvarSrc->tmVal));
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		{
			if(pvarSrc->pbinaryVal)
			{
				if(pvarSrc->bFreePtr)
					delete pvarSrc->pbinaryVal;
				pvarSrc->pbinaryVal = nullptr;
			}
		}
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		{
			if(pvarSrc->ptextVal)
			{
				if(pvarSrc->bFreePtr)
					delete pvarSrc->ptextVal;
				pvarSrc->ptextVal = nullptr;
			}
		}
		break;
	default:
		assert(false);
		break;
	}
	variantInit(pvarSrc);
	return true;
}
bool variantCopy(BSVariant *pDest, const BSVariant *pSrc)
{
	assert(pDest && pSrc);
	if(!pDest || !pSrc) return false;
	variantClear(pDest);
	pDest->dataType = pSrc->dataType;
	pDest->bFreePtr = pSrc->bFreePtr;
	switch(pDest->dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		{
			pDest->scode = pSrc->scode;
		}
		break;
	//case BEDataTypeNULL:		// VT_NULL
	//	{
	//		pDest->scode = pSrc->scode;
	//	}
	//	break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
		{
			pDest->nVal = pSrc->nVal;
		}
		break;
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		{
			pDest->bVal = pSrc->bVal;
		}
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		{
			if (pSrc->pwsVal)
			{
				if(pDest->bFreePtr)
					pDest->pwsVal = new std::wstring(*pSrc->pwsVal);
				else
					pDest->pwsVal = pSrc->pwsVal;
			}
			else
				pDest->pwsVal = NULL;
		}
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
		{
			pDest->iVal = pSrc->iVal;
		}
		break;
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
		{
			pDest->lVal = pSrc->lVal;
		}
		break;
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
		{
			pDest->llVal = pSrc->llVal;
		}
		break;
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
		{
			pDest->fltVal = pSrc->fltVal;
		}
		break;
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
		{
			pDest->dblVal = pSrc->dblVal;
		}
		break;
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			pDest->tmVal = pSrc->tmVal;
		}
		break;
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			pDest->tmVal = pSrc->tmVal;
		}
		break;
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			pDest->tmVal = pSrc->tmVal;
		}
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		{
			if (pSrc->pbinaryVal)
			{
				if(pDest->bFreePtr)
					pDest->pbinaryVal = new SBDataType(*pSrc->pbinaryVal);
				else
					pDest->pbinaryVal = pSrc->pbinaryVal;
			}
			else
				pDest->pbinaryVal = NULL;
		}
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		{
			if(pSrc->ptextVal)
			{
				if(pDest->bFreePtr)
					pDest->ptextVal = new SWDataType(*pSrc->ptextVal);
				else
					pDest->ptextVal = pSrc->ptextVal;
			}
			else
				pDest->ptextVal = NULL;
		}
		break;
	default:
		assert(false);
		break;
	}
	return true;
}

bool variantChangeType(BSVariant *pDest, BEDataType dataType)
{
	assert(pDest);
	#pragma warning( push )
	#pragma warning ( disable : 4800)
	#pragma warning ( disable : 4900)
	switch(dataType)
	{
	case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
		{
			variantClear(pDest);
		}
		break;
	//case BEDataTypeNULL:		// VT_NULL
	//	{
	//
	//	}
	//	break;
	case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
		{
			uint8_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (uint8_t)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (uint8_t)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (uint8_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (uint8_t)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (uint8_t)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						val = (uint8_t)std::wcstol(pDest->pwsVal->c_str(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= 1);
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= 1)
							val = pDest->pbinaryVal->front();
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				{
					assert(pDest->ptextVal);
					if (pDest->ptextVal)
					{
						val = (uint8_t)std::wcstol(pDest->ptextVal->data(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->ptextVal;
						pDest->ptextVal = nullptr;
					}
				}
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->nVal = val;
		}
		break;
	case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
		{
			bool val = false;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (bool)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (bool)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (bool)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (bool)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (bool)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (bool)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						val = (bool)std::wcstol(pDest->pwsVal->c_str(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= 1);
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= 1)
							val = pDest->pbinaryVal->front() != 0;
						if(pDest->bFreePtr) 
							delete pDest->pbinaryVal;
						pDest->bFreePtr = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->bVal = val;
		}
		break;
	case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
		{
			wchar_t pbuf[20] = {0};
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//pDest->pwsVal = nullptr;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->nVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->bVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->iVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->lVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->llVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				std::swprintf(pbuf, _countof(pbuf), L"%0.4f", pDest->fltVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				std::swprintf(pbuf, _countof(pbuf), L"%0.8f", pDest->dblVal);
				pDest->bFreePtr = true;
				pDest->pwsVal = new std::wstring(pbuf);
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						wchar_t pbuf[40] = {0};
						std::swprintf(pbuf, _countof(pbuf), L"%04d-%02d-%02d", 
							ptm->tm_year, ptm->tm_mon, ptm->tm_mday);
						pDest->pwsVal = new std::wstring(pbuf);
					}
				}
				break;
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						std::swprintf(pbuf, _countof(pbuf), L"%02d:%02d:%02d", 
							ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
						pDest->bFreePtr = true;
						pDest->pwsVal = new std::wstring(pbuf);
					}
				}
				break;
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						std::swprintf(pbuf, _countof(pbuf), L"%04d-%02d-%02d %02d:%02d:%02d", 
							ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
						pDest->bFreePtr = true;
						pDest->pwsVal = new std::wstring(pbuf);
					}
				}
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					// base64编码
					assert(pDest->pbinaryVal);
					if (pDest->pbinaryVal)
					{
						std::wstring *val = new std::wstring(gtc::base64::encodeW(*pDest->pbinaryVal));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
						pDest->bFreePtr = true;
						pDest->pwsVal = val;
					}
					else
						pDest->pwsVal = nullptr;
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
		{
			int16_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (int16_t)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = (int16_t)pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (int16_t)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (int16_t)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (int16_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (int16_t)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (int16_t)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						val = (int16_t)std::wcstol(pDest->pwsVal->c_str(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->iVal = val;
		}
		break;
	case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
		{
			int32_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//pDest->lVal = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (int32_t)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = (int32_t)pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (int32_t)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (int32_t)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (int32_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (int32_t)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (int32_t)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						pDest->lVal = (int32_t)std::wcstol(pDest->pwsVal->c_str(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->lVal = val;
		}
		break;
	case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
		{
			int64_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (int64_t)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = (int64_t)pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (int64_t)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (int64_t)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (int64_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (int64_t)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (int64_t)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						val = (int64_t)std::wcstol(pDest->pwsVal->c_str(), NULL, 10);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				pDest->llVal = (int64_t)pDest->tmVal;
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() == sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() == sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->llVal = val;
		}
		break;
	case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
		{
			float val = 0.0f;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0.0f;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (float)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = (float)pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (float)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (float)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (float)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (float)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (float)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						val = (float)std::wcstod(pDest->pwsVal->c_str(), NULL);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
		{
			double val = 0.0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0.0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				val = (double)pDest->nVal;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				val = (double)pDest->bVal;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				val = (double)pDest->iVal;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				val = (double)pDest->lVal;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (double)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				val = (float)pDest->fltVal;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				val = (double)pDest->dblVal;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						pDest->dblVal = std::wcstod(pDest->pwsVal->c_str(), NULL);
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				assert(false && "BEDataTypeDatetime");
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->dblVal = val;
		}
		break;
	case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			std::time_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				assert(!L"to Date, but byte bool int16 int32");
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (std::time_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				assert(false && L"float double");
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						wchar_t *pEnd = NULL;
						int year, month, day, hour, minute, second;
						year = month = day = hour = minute = second = 0;
						std::wstring::size_type pos = pDest->pwsVal->find(L"-");
						if (pos != std::wstring::npos && pos > 0)
						{
							year = std::wcstol(pDest->pwsVal->c_str(), &pEnd, 10);
							pos = pDest->pwsVal->find(L"-", pos);
							if(pos == std::wstring::npos) break;
							month = std::wcstol(pEnd, &pEnd, 10);
							if(pEnd == NULL) break;
							day = std::wcstol(pEnd, &pEnd, 10);

							std::tm dt = {0};
							dt.tm_year = year;
							dt.tm_mon = month;
							dt.tm_mday = day;
							dt.tm_hour = hour;
							dt.tm_min = minute;
							dt.tm_sec = second;
							val = std::mktime(&dt);
						}
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				val = (std::time_t)pDest->tmVal;
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->tmVal = val;
		}
		break;
	case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			std::time_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				assert(false && L"byte bool int16 int32");
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (std::time_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				assert(false && L"float double");
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						wchar_t *pEnd = NULL;
						std::wstring wstr = *pDest->pwsVal;
						int year, month, day, hour, minute, second;
						year = month = day = hour = minute = second = 0;
						std::wstring::size_type pos = wstr.find(L" ");
						if (pos != std::wstring::npos)
							wstr = wstr.substr(pos+1);
						
						pos = wstr.find(L":");
						if (pos != std::wstring::npos && pos > 0)
						{
							hour = std::wcstol(wstr.c_str(), &pEnd, 10);
							pos = wstr.find(L":", pos);
							if(pos == std::wstring::npos) break;
							minute = std::wcstol(pEnd, &pEnd, 10);
							if(pEnd) second = std::wcstol(pEnd, &pEnd, 10);

							std::tm dt = {0};
							dt.tm_year = year;
							dt.tm_mon = month;
							dt.tm_mday = day;
							dt.tm_hour = hour;
							dt.tm_min = minute;
							dt.tm_sec = second;
							val = std::mktime(&dt);
						}
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				val = (std::time_t)pDest->tmVal;
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->tmVal = val;
		}
		break;
	case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
		{
			std::time_t val = 0;
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				//val = 0;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				assert(false && L"byte bool int16 int32");
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				val = (std::time_t)pDest->llVal;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				assert(false && L"float double");
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						wchar_t *pEnd = NULL;
						int year, month, day, hour, minute, second;
						year = month = day = hour = minute = second = 0;
						std::wstring::size_type pos = pDest->pwsVal->find(L"-");
						if (pos != std::wstring::npos)
						{
							if(pos == 0) break;
							year = std::wcstol(pDest->pwsVal->c_str(), &pEnd, 10);
							pos = pDest->pwsVal->find(L"-", pos);
							if(pos == std::wstring::npos) break;
							month = std::wcstol(pEnd, &pEnd, 10);
							pos = pDest->pwsVal->find(L" ", pos);
							if(pos == std::wstring::npos) break;
							day = std::wcstol(pEnd, &pEnd, 10);
						}
						std::wstring wstr;
						wstr = *pDest->pwsVal;
						pos = wstr.find(L" ");
						if(pos != std::wstring::npos)
							wstr = wstr.substr(pos+1);
						pos = wstr.find(L":");
						if (pos != std::wstring::npos)
						{
							hour = std::wcstol(wstr.c_str(), &pEnd, 10);
							pos = wstr.find(L":");
							if(pos == std::wstring::npos) break;
							minute = std::wcstol(pEnd, &pEnd, 10);
							if(pEnd) second = std::wcstol(pEnd, &pEnd, 10);
						}
						std::tm dt = {0};
						dt.tm_year = year;
						dt.tm_mon = month;
						dt.tm_mday = day;
						dt.tm_hour = hour;
						dt.tm_min = minute;
						dt.tm_sec = second;
						val = std::mktime(&dt);

						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
					}
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				val = (std::time_t)pDest->tmVal;
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					assert(pDest->pbinaryVal && pDest->pbinaryVal->size() >= sizeof(val));
					if(pDest->pbinaryVal)
					{
						if(pDest->pbinaryVal->size() >= sizeof(val))
							memcpy(&val, pDest->pbinaryVal->data(), sizeof(val));
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
					}
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				break;
			default:
				assert(false);
				break;
			}
			pDest->bFreePtr = false;
			pDest->tmVal = val;
		}
		break;
	case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
		{
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				pDest->pbinaryVal = nullptr;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				pDest->pbinaryVal = new SBDataType();
				pDest->bFreePtr = true;
				pDest->pbinaryVal->push_back(pDest->nVal);
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				pDest->pbinaryVal = new SBDataType();
				pDest->bFreePtr = true;
				pDest->pbinaryVal->push_back(pDest->bVal);
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->iVal, (uint8_t*)&pDest->iVal+sizeof(pDest->iVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->lVal, (uint8_t*)&pDest->lVal+sizeof(pDest->lVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->llVal, (uint8_t*)&pDest->llVal+sizeof(pDest->llVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->fltVal, (uint8_t*)&pDest->fltVal+sizeof(pDest->fltVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->dblVal, (uint8_t*)&pDest->dblVal+sizeof(pDest->dblVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						SBDataType *val = new SBDataType(base64::decodeToData(*pDest->pwsVal));
						//SBDataType *val = new SBDataType(convert::toData(pDest->pwsVal->c_str(), pDest->pwsVal->length()));
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
						pDest->pbinaryVal = val;
						pDest->bFreePtr = true;
					}
					else
						pDest->pbinaryVal = nullptr;
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				pDest->pbinaryVal = new SBDataType((uint8_t*)&pDest->tmVal, (uint8_t*)&pDest->tmVal+sizeof(pDest->tmVal));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				//pDest->pbinaryVal = new SBDataType(*pDest->pbinaryVal);
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				pDest->pbinaryVal = new SBDataType(base64::decodeToData(*pDest->pwsVal));
				//pDest->pbinaryVal = new SBDataType(convert::toData((PCWSTR)pDest->ptextVal->data(), pDest->ptextVal->size()));
				pDest->bFreePtr = true;
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
		{
			wchar_t pbuf[40] = {0};
			switch(pDest->dataType)
			{
			case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
				pDest->ptextVal = nullptr;
				break;
			case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->nVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->bVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->iVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->lVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
				std::swprintf(pbuf, _countof(pbuf), L"%d", pDest->llVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
				std::swprintf(pbuf, _countof(pbuf), L"%0.4", pDest->fltVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
				std::swprintf(pbuf, _countof(pbuf), L"%0.8", pDest->fltVal);
				pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
				pDest->bFreePtr = true;
				break;
			case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
				{
					assert(pDest->pwsVal);
					if (pDest->pwsVal)
					{
						SWDataType *val = new SWDataType(pDest->pwsVal->begin(), pDest->pwsVal->end());
						if(pDest->bFreePtr)
							delete pDest->pwsVal;
						pDest->pwsVal = nullptr;
						pDest->ptextVal = val;
						pDest->bFreePtr = true;
					}
					else
						pDest->ptextVal = nullptr;
				}
				break;
			case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						std::swprintf(pbuf, _countof(pbuf), L"%04d-%02d-%02d", 
							ptm->tm_year, ptm->tm_mon, ptm->tm_mday);
						pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
						pDest->bFreePtr = true;
					}
				}
				break;
			case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						std::swprintf(pbuf, _countof(pbuf), L"%02d:%02d:%02d", 
							ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
						pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
						pDest->bFreePtr = true;
					}
				}
				break;
			case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
				{
					if(std::tm *ptm = std::localtime(&pDest->tmVal))
					{
						std::swprintf(pbuf, _countof(pbuf), L"%04d-%02d-%02d %02d:%02d:%02d", 
							ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
						pDest->ptextVal = new SWDataType(std::begin(pbuf), std::begin(pbuf)+std::wcslen(pbuf));
						pDest->bFreePtr = true;
					}
				}
				break;
			case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
				{
					// base64编码
					assert(pDest->pbinaryVal);
					if (pDest->pbinaryVal)
					{
						std::wstring wstr = gtc::base64::encodeW(*pDest->pbinaryVal);
						if(pDest->bFreePtr)
							delete pDest->pbinaryVal;
						pDest->pbinaryVal = nullptr;
						pDest->ptextVal = new SWDataType(wstr.begin(), wstr.end());
						pDest->bFreePtr = true;
					}
					else
						pDest->ptextVal = nullptr;
				}
				break;
			case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
				//pDest->pbinaryVal = new SBDataType(convert::toData(pDest->ptextVal->data(), pDest->ptextVal->size()));
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	default:
		assert(false);
		break;
	}	
	pDest->dataType = dataType;
	#pragma warning( pop )
	return true;
}
}