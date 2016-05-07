#include "rapidjsonWrap.h"
#include "AC/stringUtil.h"
#include "AC/baseException.h"

namespace gtc {
static std::wstring _errorInfoJson(BEJsonCode code)
{
	switch(code)
	{
	case BEJsonCodeOk:					//= 0,	// 成功
		return L"成功！";
		break;
	case BEJsonCodeInvalidArgument:		//,		// 无效参数
		return L"无效参数";
		break;
	case BEJsonCodeKeyNotFound:			//,		// 对象不存在
		return L"对象不存在";
		break;
	case BEJsonCodeTypeError:			//,		// 类型不匹配
		return L"类型不匹配";
		break;
	}
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"未知的状态码：%d", code);
	return pbuf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	rapidJsonWrap  类		2015-10-29
//	针对rapidjson的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
rapidJsonWrap::rapidJsonWrap(const std::string &strjson)
	: m_pVal(NULL)
	, m_pArrayVal(NULL), m_iValCurIndex(-1)
{
	m_doc.Parse(strjson.c_str());
}

rapidJsonWrap::rapidJsonWrap(rapidjson::Value *pVal)
	: m_pVal(pVal)
	, m_pArrayVal(NULL), m_iValCurIndex(-1)
{
	assert(m_pVal);
}
/*static*/ rapidjson::Value* rapidJsonWrap::nextObject(rapidjson::Value *pCurVal, const std::string &strKey)
{
	assert(pCurVal && !strKey.empty());
	if(!pCurVal || strKey.empty()) return NULL;
	rapidjson::Document::MemberIterator iter = pCurVal->FindMember(strKey.c_str());
	if (iter != pCurVal->MemberEnd())
	{
		return &iter->value;
	}
	return NULL;
}

rapidjson::Value* rapidJsonWrap::findValue(const std::string &strKey)
{
	if(strKey.empty()) JSON_EXCEPT(BEJsonCodeInvalidArgument);
	rapidjson::Document::MemberIterator iter;
	wchar_t pbuf[40] = {0};
	rapidjson::Value *pVal = NULL;

	if (!m_pVal)
	{
		if (strKey.find(':') == std::string::npos)
		{
			iter = m_doc.FindMember(strKey.c_str());
			if(iter == m_doc.MemberEnd())
			{
				std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(strKey.c_str()).c_str());
				JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
			}
			else
				return &iter->value;
		}
		else
		{
			SStringArrayType vec = stringUtil::split(strKey, ":");
			iter = m_doc.FindMember(vec[0].c_str());
			if(iter == m_doc.MemberEnd())
			{
				std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(vec[0].c_str()).c_str() );
				JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
			}
			else
				pVal = &iter->value;
			for (std::size_t idx = 1; idx < vec.size(); ++idx)
			{
				pVal = nextObject(pVal, vec[idx]);
				if (!pVal)
				{
					std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(vec[idx].c_str()).c_str());
					JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
				}
			}

		}
		return pVal;
	}
	if (strKey.find(':') == std::string::npos)
	{
		iter = m_pVal->FindMember(strKey.c_str());
		if(iter == m_pVal->MemberEnd())
		{
			std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(strKey.c_str()).c_str());
			JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
		}
		else
			pVal = &iter->value;
	}
	else
	{
		SStringArrayType vec = stringUtil::split(strKey, ":");
		iter = m_pVal->FindMember(vec[0].c_str());
		if(iter == m_pVal->MemberEnd())
		{
			std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(vec[0].c_str()).c_str() );
			JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
		}
		else
			pVal = &iter->value;
		for (std::size_t idx = 1; idx < vec.size(); ++idx)
		{
			pVal = nextObject(pVal, vec[idx]);
			if (!pVal)
			{
				std::swprintf(pbuf, _countof(pbuf), L"名为“%s”的对象不存在！", gtc::stringUtil::toWChar(vec[idx].c_str()).c_str());
				JSON_EXCEPT2(BEJsonCodeKeyNotFound, pbuf);
			}
		}

	}
	return pVal;
}
void rapidJsonWrap::selectEmpty()
{
	m_pVal = NULL;
	m_iValCurIndex = -1;
	m_pArrayVal = NULL;
}
rapidjson::Value* rapidJsonWrap::selectObject(const std::string &strPath)
{
	//m_pVal = NULL;
	//m_iValCurIndex = -1;
	//m_pArrayVal = NULL;
	assert(!strPath.empty());
	if(strPath.empty()) return NULL;
	SStringArrayType vec = stringUtil::split(strPath.c_str(), ":");
	if (vec.size() == 1)
	{
		if(m_pVal)
		{
			m_pVal = findValue(vec[0]);
		}
		else
		{
			rapidjson::Document::MemberIterator iter = m_doc.FindMember(vec[0].c_str());
			if(iter == m_doc.MemberEnd()) return NULL;
			m_pVal = &iter->value;
		}
		return m_pVal;
	}
	return selectObject(vec);
}
rapidjson::Value* rapidJsonWrap::selectObject(const SStringArrayType &vecObjectPath)
{
	m_pVal = NULL;
	m_iValCurIndex = -1;
	m_pArrayVal = NULL;
	assert(vecObjectPath.size() > 0);
	if(vecObjectPath.size() == 0) return NULL;
	rapidjson::Value *pval = NULL;
	rapidjson::Document::MemberIterator iter = m_doc.FindMember(vecObjectPath[0].c_str());
	if(iter == m_doc.MemberEnd()) return NULL;
	pval = &iter->value;

	for (std::size_t idx = 1; idx < vecObjectPath.size() && pval; ++idx)
	{
		pval = nextObject(pval, vecObjectPath[idx]);
	}

	m_pVal = pval;
	return m_pVal;
}

rapidjson::Value* rapidJsonWrap::nextItem()
{
	if (m_iValCurIndex == -1)
	{
		if(!m_pVal || !m_pVal->IsArray() || m_pVal->Size() == 0) return NULL;
		m_iValCurIndex = 0;
		m_pArrayVal = m_pVal;
		m_pVal = &(*m_pArrayVal)[m_iValCurIndex];
		return m_pVal;
	}
	if(!m_pArrayVal) return NULL;
	++m_iValCurIndex;
	if(m_iValCurIndex >= (int)m_pArrayVal->Size())
	{
		m_pVal = m_pArrayVal;	// 还原
		return NULL;
	}
	m_pVal = &(*m_pArrayVal)[m_iValCurIndex];
	return m_pVal;
}
bool rapidJsonWrap::asBoolean(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return false;
	if(pVal->IsBool()) pVal->GetBool();
	else if(pVal->IsString()) return std::strtol(pVal->GetString(), NULL, 10) != 0;
	else if(pVal->IsUint()) return pVal->GetUint() != 0;
	else if(pVal->IsInt64()) return pVal->GetInt64() != 0;
	else if(pVal->IsUint64()) return pVal->GetUint64() != 0;
	else if(pVal->IsNull()) return 0;
	else JSON_EXCEPT(BEJsonCodeTypeError);
	return false;
}
int rapidJsonWrap::asInt(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return 0;
	if(pVal->IsInt()) return pVal->GetInt();
	else if(pVal->IsString()) return std::strtol(pVal->GetString(), NULL, 10);
	else if(pVal->IsBool()) return pVal->GetBool();
	else if(pVal->IsUint()) return pVal->GetUint();
	else if(pVal->IsInt64()) return pVal->GetInt64();
	else if(pVal->IsUint64()) return pVal->GetUint64();
	else if(pVal->IsDouble()) return pVal->GetDouble();
	else if(pVal->IsNull()) return 0;
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return 0;
}
unsigned int rapidJsonWrap::asUInt(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return 0;
	if(pVal->IsUint()) return pVal->GetUint();
	else if(pVal->IsString()) return std::strtoul(pVal->GetString(), NULL, 10);
	else if(pVal->IsBool()) return pVal->GetBool();
	else if(pVal->IsInt()) return pVal->GetInt();
	else if(pVal->IsInt64()) return pVal->GetInt64();
	else if(pVal->IsUint64()) return pVal->GetUint64();
	else if(pVal->IsDouble()) return pVal->GetDouble();
	else if(pVal->IsNull()) return 0;
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return 0;
}

int64_t rapidJsonWrap::asInt64(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return 0;
	if(pVal->IsInt64()) return pVal->GetInt64();
	else if(pVal->IsString()) return std::strtol(pVal->GetString(), NULL, 10);
	else if(pVal->IsBool()) return pVal->GetBool();
	else if(pVal->IsInt()) return pVal->GetInt();
	else if(pVal->IsUint()) return pVal->GetUint();
	else if(pVal->IsUint64()) return pVal->GetUint64();
	else if(pVal->IsDouble()) return pVal->GetDouble();
	else if(pVal->IsNull()) return 0;
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return 0;
}
uint64_t rapidJsonWrap::asUInt64(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return 0;
	if(pVal->IsUint64()) return pVal->GetUint64();
	else if(pVal->IsString()) return std::strtoul(pVal->GetString(), NULL, 10);
	else if(pVal->IsBool()) return pVal->GetBool();
	else if(pVal->IsInt()) return pVal->GetInt();
	else if(pVal->IsUint()) return pVal->GetUint();
	else if(pVal->IsInt64()) return pVal->GetInt64();
	else if(pVal->IsDouble()) return pVal->GetDouble();
	else if(pVal->IsNull()) return 0;
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return 0;
}

double rapidJsonWrap::asDouble(const std::string &strKey)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return 0;
	if(pVal->IsDouble()) return pVal->GetDouble();
	else if(pVal->IsString()) return std::strtod(pVal->GetString(), NULL);
	else if(pVal->IsBool()) return pVal->GetBool();
	else if(pVal->IsInt()) return pVal->GetInt();
	else if(pVal->IsUint()) return pVal->GetUint();
	else if(pVal->IsInt64()) return pVal->GetInt64();
	else if(pVal->IsUint64()) return pVal->GetUint64();
	else if(pVal->IsNull()) return 0.0;
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return 0.0;
}

std::wstring rapidJsonWrap::asString(const std::string &strKey, UINT codePage /*= CP_UTF8*/)
{
	rapidjson::Value *pVal = findValue(strKey);
	if(!pVal) return L"";
	wchar_t pbuf[64] = {0};
	if(pVal->IsString()) return gtc::stringUtil::toWChar(pVal->GetString(), codePage);
	else if(pVal->IsBool()) return pVal->GetBool() ? L"true" : L"false";
	else if(pVal->IsInt())
	{
		std::swprintf(pbuf, _countof(pbuf), L"%d", pVal->GetInt());
		return pbuf;
	}
	else if(pVal->IsUint())
	{
		std::swprintf(pbuf, _countof(pbuf), L"%u", pVal->GetUint());
		return pbuf;
	}
	else if(pVal->IsInt64())
	{
		std::swprintf(pbuf, _countof(pbuf), L"%lld" /*%I64d*/, pVal->GetInt64());
		return pbuf;
	}
	else if(pVal->IsUint64())
	{
		std::swprintf(pbuf, _countof(pbuf), L"%llu" /*%I64u*/, pVal->GetUint64());
		return pbuf;
	}
	else if(pVal->IsDouble())
	{
		std::swprintf(pbuf, _countof(pbuf), L"%f", pVal->GetDouble());
		return pbuf;
	}
	else if(pVal->IsNull()) return L"null";
	else JSON_EXCEPT(BEJsonCodeTypeError);

	return L"";

}
}	// gtc